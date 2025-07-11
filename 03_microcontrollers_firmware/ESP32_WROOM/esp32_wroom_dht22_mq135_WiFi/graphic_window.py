import pandas as pd
import matplotlib.pyplot as plt

# Lee los datos del archivo CSV
data = pd.read_csv("datos.csv", header=None, names=['fecha','hora','temperatura','humedad','mq135'])

# Combina la fecha y la hora en una sola columna tipo datetime
data['datetime'] = pd.to_datetime(data['fecha'] + ' ' + data['hora'], errors='coerce')

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