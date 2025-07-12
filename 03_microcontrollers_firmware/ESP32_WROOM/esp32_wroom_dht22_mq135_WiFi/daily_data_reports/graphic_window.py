import struct
import pandas as pd
import matplotlib.pyplot as plt
from datetime import datetime
import os

# La estructura debe ser exactamente igual a la de C++ (sin padding, 19 bytes)
record_struct = struct.Struct('<H5Bffi')
print("Tamaño del struct (debe ser 19):", record_struct.size)

data_list = []

# Verifica que el archivo existe
if not os.path.exists("datos.bin"):
    raise FileNotFoundError("El archivo datos.bin no se encuentra en el directorio actual.")

# Verifica que el archivo sea múltiplo de 19 bytes
file_size = os.path.getsize("datos.bin")
if file_size % record_struct.size != 0:
    print("¡Advertencia! El archivo datos.bin no es múltiplo del tamaño esperado (19 bytes). Puede estar corrupto o tener datos de una versión anterior.")

with open("datos.bin", "rb") as f:
    while True:
        bytes_leidos = f.read(record_struct.size)
        if not bytes_leidos or len(bytes_leidos) < record_struct.size:
            break
        try:
            year, month, day, hour, minute, second, temperatura, humedad, mq135 = record_struct.unpack(bytes_leidos)
            fecha_hora = datetime(year, month, day, hour, minute, second)
            data_list.append([fecha_hora, temperatura, humedad, mq135])
        except Exception as e:
            print("Error interpretando un registro:", e)

if not data_list:
    raise ValueError("No se encontraron registros válidos en datos.bin.")

data = pd.DataFrame(data_list, columns=['datetime', 'temperatura', 'humedad', 'mq135'])

plt.figure(figsize=(12,6))

plt.subplot(2,1,1)
plt.plot(data['datetime'], data['temperatura'], label='Temperatura (°C)', color='tab:red')
plt.plot(data['datetime'], data['humedad'], label='Humedad (%)', color='tab:blue')
plt.legend()
plt.title('Temperatura y Humedad')
plt.xlabel('Fecha y Hora')
plt.ylabel('Valor')
plt.grid()

plt.subplot(2,1,2)
plt.plot(data['datetime'], data['mq135'], label='MQ135', color='tab:green')
plt.legend()
plt.title('Valor MQ135')
plt.xlabel('Fecha y Hora')
plt.ylabel('Valor')
plt.grid()

plt.tight_layout()
plt.show()