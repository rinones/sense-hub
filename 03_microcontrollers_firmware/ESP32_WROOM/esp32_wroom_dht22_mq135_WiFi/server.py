from flask import Flask, request
import csv

app = Flask(__name__)

@app.route('/datos', methods=['POST'])
def recibir_datos():
    data = request.get_json()
    if not data:
        return "No se recibieron datos", 400

    # Abre el archivo en modo append y agrega una fila nueva
    with open("datos.csv", "a", newline='') as f:
        writer = csv.writer(f)
        writer.writerow([
            data.get("fecha", ""),
            data.get("hora", ""),
            data.get("temperatura", ""),
            data.get("humedad", ""),
            data.get("mq135", "")
        ])
    return "OK", 200

if __name__ == "__main__":
    # host='0.0.0.0' permite que el ESP32 acceda desde la red local
    app.run(host='0.0.0.0', port=5000)