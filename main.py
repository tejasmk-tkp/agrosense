from fastapi import FastAPI
from fastapi.middleware.cors import CORSMiddleware
from fastapi_mqtt.config import MQTTConfig
from fastapi_mqtt.fastmqtt import FastMQTT

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

temperature = list()
pressure = list()
soil_moisture = list()
light_intensity = list()
uv_intensity = list()
mq2 = list()
mq135 = list()

''' /esp1/temp '''
@mqtt.on_connect()
def connect(client, flags, rc, properties):
    mqtt.client.subscribe("/esp1/temp") #subscribing mqtt topic

@mqtt.on_message()
async def message(client, topic, payload, qos, properties):
    temperature.append(payload.decode('utf-8'))

@mqtt.subscribe("/esp1/temp")
async def message_from_topic(client, topic, payload, qos, properties):
    temperature.append(payload.decode('utf-8'))

''' /esp1/pressure '''
@mqtt.on_connect()
def connect(client, flags, rc, properties):
    mqtt.client.subscribe("/esp1/pressure")

@mqtt.on_message()
async def message(client, topic, payload, qos, properties):
    pressure.append(payload.decode('utf-8'))

@mqtt.subscribe("/esp1/pressure")
async def message_from_topic(client, topic, payload, qos, properties):
    pressure.append(payload.decode('utf-8'))

''' /esp1/soil_moisture '''
@mqtt.on_connect()
def connect(client, flags, rc, properties):
    mqtt.client.subscribe("/esp1/soil_moisture")

@mqtt.on_message()
async def message(client, topic, payload, qos, properties):
    soil_moisture.append(payload.decode('utf-8'))

@mqtt.subscribe("/esp1/soil_moisture")
async def message_from_topic(client, topic, payload, qos, properties):
    soil_moisture.append(payload.decode('utf-8'))

''' /esp1/light_intensity '''
@mqtt.on_connect()
def connect(client, flags, rc, properties):
    mqtt.client.subscribe("/esp1/light_intensity")

@mqtt.on_message()
async def message(client, topic, payload, qos, properties):
    light_intensity.append(payload.decode('utf-8'))

@mqtt.subscribe("/esp1/light_intensity")
async def message_from_topic(client, topic, payload, qos, properties):
    light_intensity.append(payload.decode('utf-8'))

''' /esp1/uv_intensity '''
@mqtt.on_connect()
def connect(client, flags, rc, properties):
    mqtt.client.subscribe("/esp1/uv_intensity")

@mqtt.on_message()
async def message(client, topic, payload, qos, properties):
    uv_intensity.append(payload.decode('utf-8'))

@mqtt.subscribe("/esp1/uv_intensity")
async def message_from_topic(client, topic, payload, qos, properties):
    uv_intensity.append(payload.decode('utf-8'))

''' /esp1/mq2 '''
@mqtt.on_connect()
def connect(client, flags, rc, properties):
    mqtt.client.subscribe("/esp1/mq2")

@mqtt.on_message()
async def message(client, topic, payload, qos, properties):
    mq2.append(payload.decode('utf-8'))

@mqtt.subscribe("/esp1/mq2")
async def message_from_topic(client, topic, payload, qos, properties):
    mq2.append(payload.decode('utf-8'))

''' /esp1/mq135 '''
@mqtt.on_connect()
def connect(client, flags, rc, properties):
    mqtt.client.subscribe("/esp1/mq135")

@mqtt.on_message()
async def message(client, topic, payload, qos, properties):
    mq135.append(payload.decode('utf-8'))

@mqtt.subscribe("/esp1/mq135")
async def message_from_topic(client, topic, payload, qos, properties):
    mq135.append(payload.decode('utf-8'))

''' ------------------------------------------------------ '''

''' Connect and Disconnect events are not working properly '''
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

@app.get("/temperature")
async def get_temperature():
    return {"temperature": temperature}

@app.get("/pressure")
async def get_pressure():
    return {"pressure": pressure}

@app.get("/soil_moisture")
async def get_soil_moisture():
    return {"soil_moisture": soil_moisture}

@app.get("/light_intensity")
async def get_light_intensity():
    return {"light_intensity": light_intensity}

@app.get("/uv_intensity")
async def get_uv_intensity():
    return {"uv_intensity": uv_intensity}

@app.get("/mq2")
async def get_mq2():
    return {"mq2": mq2}

@app.get("/mq135")
async def get_mq135():
    return {"mq135": mq135}
