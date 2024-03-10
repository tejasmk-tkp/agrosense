from time import sleep

from fastapi import FastAPI
from fastapi.middleware.cors import CORSMiddleware
from fastapi_mqtt.config import MQTTConfig
from fastapi_mqtt.fastmqtt import FastMQTT
import predictions as pred
import weather
import threading

app = FastAPI()

origins = [
    "http://localhost.tiangolo.com",
    "https://localhost.tiangolo.com",
    "http://localhost",
    "http://localhost:8080",
]

app.add_middleware(
    CORSMiddleware,
    allow_origins=origins,
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

mqtt_config = MQTTConfig(    
    host="broker.emqx.io",
    port=1883,
    keepalive=60,
    username="emqx",
    password="public",
    )

mqtt = FastMQTT(config=mqtt_config)

mqtt.init_app(app)

data = []

def process_data(payload):
    d = list(map(float, payload.decode('utf-8').split()))
    cloud_cover, solar_radiation = pred.weather_prediction(d[0], d[1])
    N, P, K = pred.NPK_prediction(d[0], d[1])
    weather_pre = weather.weather_predict(d[0], d[1])
    data.append({
        "temperature": d[0],
        "humidity": d[1],
        "soil_moisture": d[2],
        "light_intensity": d[3],
        "uv_intensity": d[4],
        "mq2_val": d[5],
        "mq135_val": d[6],
        "cloudcover": cloud_cover[0],
        "weather": weather_pre[0],
        "solarradiation": solar_radiation[0],
        "N": N[0],
        "P": P[0],
        "K": K[0]
    })

def process_payload(payload):
    threading.Thread(target=process_data, args=(payload,)).start()

''' /esp '''
def connect(client, flags, rc, properties):
    mqtt.client.subscribe("/esp") #subscribing mqtt topic
    print("Connected")

@mqtt.on_message()
async def message(client, topic, payload, qos, properties):
    try:
        process_payload(payload)

    except Exception as e:
        print(e)

@mqtt.subscribe("/esp")
async def message_from_topic(client, topic, payload, qos, properties):
    try:
        process_payload(payload)
        sleep(1)
    except Exception as e:
        print(e)

@mqtt.on_disconnect()
def disconnect(client, packet, exc=None):
    return

@mqtt.on_subscribe()
def subscribe(client, mid, qos, properties):
    return
''' ------------------------------------------------------ '''

@app.get("/")
def root():
    return {"Hello": "World"}

@app.get("/data")
async def get_data():
    global data  # Add this line to access the global variable 'data'
    return data