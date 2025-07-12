#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <time.h>
#include <FS.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>

// Pines y configuración
#define DHTPIN 4
#define DHTTYPE DHT22
#define MQ135_AO 39
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
DHT dht(DHTPIN, DHTTYPE);

const char *ssid = "MIWIFI_Xejf_2.4";
const char *password = "QaecC6Np";
const char *serverName = "http://192.168.1.162:5000/datos";
const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3600;
const int daylightOffset_sec = 3600;

// Forzar sin padding
#pragma pack(push, 1)
struct SensorDataBin
{
  uint16_t year;
  uint8_t month;
  uint8_t day;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
  float temperatura;
  float humedad;
  int32_t mq135;
};
#pragma pack(pop)

AsyncWebServer server(80);

void setup()
{
  Serial.begin(115200);
  dht.begin();

  // Imprimir tamaño de la estructura (debe ser 19)
  Serial.print("sizeof(SensorDataBin): ");
  Serial.println(sizeof(SensorDataBin));

  // Iniciar SPIFFS
  if (!SPIFFS.begin(true))
  {
    Serial.println("Error al montar SPIFFS");
    while (true);
  }

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println(F("No se encontró la pantalla SSD1306"));
    while (true);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Iniciando WiFi...");
  display.display();
  delay(1000);

  WiFi.begin(ssid, password);
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Conectando WiFi...");
  display.display();

  int intentos = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    intentos++;
    if (intentos > 40)
    {
      display.println("Sin WiFi!");
      display.display();
      Serial.println("No se pudo conectar a WiFi.");
      break;
    }
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("\nWiFi conectado.");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());

    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("WiFi conectado!");
    display.print("IP: ");
    display.println(WiFi.localIP());
    display.display();
    delay(1000);

    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  }

  // Endpoint para descargar el archivo binario
  server.on("/download", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    if (SPIFFS.exists("/datos.bin")) {
      AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/datos.bin", "application/octet-stream");
      response->addHeader("Content-Disposition", "attachment; filename=\"datos.bin\"");
      request->send(response);
    } else {
      request->send(404, "text/plain", "Archivo no encontrado");
    } });

  // Endpoint para limpiar el archivo binario
  server.on("/clear", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    if (SPIFFS.exists("/datos.bin")) {
      SPIFFS.remove("/datos.bin");
      request->send(200, "text/plain", "Archivo datos.bin eliminado correctamente");
    } else {
      request->send(200, "text/plain", "Archivo datos.bin no existe");
    } });

  server.begin();
}

void loop()
{
  float temperatura = dht.readTemperature();
  float humedad = dht.readHumidity();
  int32_t mq135 = analogRead(MQ135_AO);

  struct tm timeinfo;
  if (!getLocalTime(&timeinfo))
  {
    Serial.println("Error obteniendo hora");
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Error NTP");
    display.display();
    delay(2000);
    return;
  }

  char fecha[16];
  strftime(fecha, sizeof(fecha), "%d/%m/%Y", &timeinfo);
  char hora[10];
  strftime(hora, sizeof(hora), "%H:%M:%S", &timeinfo);

  Serial.printf("Fecha: %s  Hora: %s\n", fecha, hora);
  Serial.print("Temp: ");
  Serial.print(temperatura);
  Serial.print(" C\tHum: ");
  Serial.print(humedad);
  Serial.print(" %\tMQ135: ");
  Serial.println(mq135);

  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Fecha: ");
  display.println(fecha);
  display.print("Hora: ");
  display.println(hora);

  if (isnan(temperatura) || isnan(humedad))
  {
    display.println("Error en DHT22");
  }
  else
  {
    display.print("T: ");
    display.print(temperatura, 1);
    display.print("C  H: ");
    display.print(humedad, 1);
    display.println("%");
  }
  display.print("MQ135: ");
  display.println(mq135);

  // Guardar los datos en binario
  SensorDataBin data;
  data.year = timeinfo.tm_year + 1900;
  data.month = timeinfo.tm_mon + 1;
  data.day = timeinfo.tm_mday;
  data.hour = timeinfo.tm_hour;
  data.minute = timeinfo.tm_min;
  data.second = timeinfo.tm_sec;
  data.temperatura = temperatura;
  data.humedad = humedad;
  data.mq135 = mq135;

  File file = SPIFFS.open("/datos.bin", FILE_APPEND);
  if (!file)
  {
    Serial.println("No se pudo abrir datos.bin para escribir.");
  }
  else
  {
    file.write((uint8_t *)&data, sizeof(SensorDataBin));
    file.close();
    Serial.println("Datos guardados en binario (SPIFFS).");
  }

  // Enviar por HTTP si hay WiFi
  String httpMsg = "";
  int httpResponseCode = -1;

  if (WiFi.status() == WL_CONNECTED)
  {
    HTTPClient http;
    http.begin(serverName);
    http.addHeader("Content-Type", "application/json");

    String postData = "{";
    postData += "\"fecha\":\"" + String(fecha) + "\",";
    postData += "\"hora\":\"" + String(hora) + "\",";
    postData += "\"temperatura\":" + (isnan(temperatura) ? "null" : String(temperatura, 2)) + ",";
    postData += "\"humedad\":" + (isnan(humedad) ? "null" : String(humedad, 2)) + ",";
    postData += "\"mq135\":" + String(mq135);
    postData += "}";

    httpResponseCode = http.POST(postData);

    Serial.print("Código de respuesta HTTP: ");
    Serial.println(httpResponseCode);
    Serial.println(http.errorToString(httpResponseCode));

    display.setCursor(0, 56);
    if (httpResponseCode > 0)
    {
      httpMsg = "HTTP OK: " + String(httpResponseCode);
      display.print(httpMsg);
    }
    else
    {
      httpMsg = "HTTP FAIL!";
      display.print(httpMsg);
    }
    display.display();
    delay(1000);

    http.end();
  }
  else
  {
    Serial.println("WiFi NO conectado, no se envían datos");
    display.setCursor(0, 56);
    httpMsg = "Sin WiFi, no HTTP";
    display.print(httpMsg);
    display.display();
    delay(1000);
  }

  delay(2000);
}