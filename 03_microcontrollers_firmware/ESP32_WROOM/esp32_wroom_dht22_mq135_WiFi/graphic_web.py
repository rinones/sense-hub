import streamlit as st
import pandas as pd

st.title("Gráfico en vivo de sensores")

# Refresca la app cada 3 segundos
st.rerun()()

try:
    df = pd.read_csv("datos.csv")
    df["fecha_hora"] = pd.to_datetime(df["fecha"] + " " + df["hora"], format="%d/%m/%Y %H:%M:%S")
    st.line_chart(
        data=df.set_index("fecha_hora")[["temperatura", "humedad", "mq135"]],
        use_container_width=True
    )
except Exception as e:
    st.write(f"Esperando datos... {e}")

# O usando st_autorefresh (requiere streamlit >= 1.10):
from streamlit_autorefresh import st_autorefresh

st_autorefresh(interval=3000, key="refresh")