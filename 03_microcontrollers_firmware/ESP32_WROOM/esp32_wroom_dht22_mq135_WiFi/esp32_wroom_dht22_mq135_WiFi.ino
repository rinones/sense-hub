#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <time.h>

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

// IP de tu PC, debe estar en la misma red WiFi
const char *serverName = "http://192.168.1.162:5000/datos";

// Zona horaria para España peninsular (Getxo)
const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3600;
const int daylightOffset_sec = 3600;

void setup()
{
  Serial.begin(115200);
  dht.begin();
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println(F("No se encontró la pantalla SSD1306"));
    while (true)
      ;
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

    // Configura NTP
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  }
}

void loop()
{
  float temperatura = dht.readTemperature();
  float humedad = dht.readHumidity();
  int mq135 = analogRead(MQ135_AO);

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

  // Aquí empieza la lógica del mensaje dinámico según si la transmisión HTTP fue correcta o no
  String httpMsg = "";
  int httpResponseCode = -1;

  // Enviar datos por HTTP POST si WiFi está conectado
  if (WiFi.status() == WL_CONNECTED)
  {
    HTTPClient http;
    http.begin(serverName); // URL del servidor Flask
    http.addHeader("Content-Type", "application/json");

    // Construimos el JSON con los datos
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

    display.setCursor(0, 56); // Última línea
    if (httpResponseCode > 0)
    {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      httpMsg = "HTTP OK: " + String(httpResponseCode);
      display.print(httpMsg);
    }
    else
    {
      Serial.print("Error en la petición HTTP: ");
      Serial.println(httpResponseCode);
      httpMsg = "HTTP FAIL!";
      display.print(httpMsg);
    }
    display.display();
    delay(1000); // Mostrar mensaje 1 segundo

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

  // Fin de la lógica dinámica del mensaje

  delay(2000); // Espera 2 segundos antes de la próxima lectura
}