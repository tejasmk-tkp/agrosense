#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT11.h>

/*
Sensors
  BMP180 - SCL, SDA
  DHT11 - 15
  Soil Sensor - 36
  LDR - 21
  UV Sensor - 12
  MQ2 - 14
  MQ135 - 27
  Rain - 26
*/

#define UNUSED(x) (void)(x)

const int soil_moisture_sensor = 36;
const int ldrPin = 21;
const int uv_sensor = 12;
const int mq2 = 14; //Flamable Gases
const int mq135 = 27; //CO, NH3, SO2
const int relay = 23;

DHT11 dht11(2);

// WiFi
const char *ssid = "realme narzo N55"; // Enter your Wi-Fi name
const char *password = "d9bc36kz";  // Enter Wi-Fi password

// MQTT Broker
const char *mqtt_broker = "broker.emqx.io";
//const char *topic = "/esp1";
const char *mqtt_username = "emqx";
const char *mqtt_password = "public";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);
String client_id;

int temperature = 0;
int humidity = 0;

void setup() {
    // Set software serial baud to 115200;
    Serial.begin(115200);
    // Connecting to a WiFi network
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.println("Connecting to WiFi..");
    }
    Serial.println("Connected to the Wi-Fi network");
    //connecting to a mqtt broker
    client.setServer(mqtt_broker, mqtt_port);
    client.setCallback(callback);

    while (!client.connected()) {
        
        client_id = "espClient-";// + String(WiFi.macAddress());
        //Serial.println("The client %s connects to the public MQTT broker\n", client_id.c_str());
        if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
            Serial.println("Public EMQX MQTT broker connected");
        } else {
            Serial.print("failed with state ");
            Serial.print(client.state());
            delay(2000);
        }
    }
}

void callback(char *topic, byte *payload, unsigned int length) {
    Serial.print("Message arrived in topic: ");
    Serial.println(topic);
    Serial.print("Message:");
    for (int i = 0; i < length; i++) {
        Serial.print((char) payload[i]);
    }
    Serial.println();
    Serial.println("-----------------------");
}

void loop() {

  float soil_moisture = analogRead(soil_moisture_sensor);
  float light_intensity = analogRead(ldrPin);
  float uv_intensity = analogRead(uv_sensor);
  int mq2_val = analogRead(mq2);
  int mq135_val = analogRead(mq135);
  dht11.readTemperatureHumidity(temperature, humidity);

  String data_str = String(temperature) + " " + String(humidity) + " " + 
                     String(soil_moisture) + " " + String(light_intensity) + " " + 
                     String(uv_intensity) + " " + String(mq2_val) + " " + String(mq135_val);

  Serial.println(data_str);

  if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
    client.publish("/esp", data_str.c_str());
    Serial.println("Published Data!");
  }
  client.loop();
  delay(1000);

  if (soil_moisture < 20) {
    digitalWrite(relay, HIGH);
  }
  else {
    digitalWrite(relay, LOW);
  }
}