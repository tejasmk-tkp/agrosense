#include <DHTesp.h>
#include <Adafruit_BMP085.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Ticker.h>

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

const int dhtPin = 15;
const int soil_moisture_sensor = 36;
const int ldrPin = 21;
const int uv_sensor = 12;
const int mq2 = 14; //Flamable Gases
const int mq135 = 27; //CO, NH3, SO2

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

char temperature_str[100];
char pressure_str[100];
char soil_moisture_str[100];
char light_intensity_str[100];
char uv_intensity_str[100];
char mq2_val_str[100];
char mq135_val_str[100];

Adafruit_BMP085 bmp;
DHTesp dht;

void tempTask(void *pvParameters);
bool getTemperature();
void triggerGetTemp();

/** Task handle for the light value read task */
TaskHandle_t tempTaskHandle = NULL;
/** Ticker for temperature reading */
Ticker tempTicker;
/** Comfort profile */
ComfortState cf;
/** Flag if task should run */
bool tasksEnabled = false;
/** Pin number for DHT11 data pin */


bool initTemp() {
  byte resultValue = 0;
  UNUSED(resultValue);
  // Initialize temperature sensor
	dht.setup(dhtPin, DHTesp::DHT11);
	Serial.println("DHT initiated");

  // Start task to get temperature
	xTaskCreatePinnedToCore(
			tempTask,                       /* Function to implement the task */
			"tempTask ",                    /* Name of the task */
			4000,                           /* Stack size in words */
			NULL,                           /* Task input parameter */
			5,                              /* Priority of the task */
			&tempTaskHandle,                /* Task handle. */
			1);                             /* Core where the task should run */

  if (tempTaskHandle == NULL) {
    Serial.println("Failed to start task for temperature update");
    return false;
  } else {
    // Start update of environment data every 20 seconds
    tempTicker.attach(20, triggerGetTemp);
  }
  return true;
}

/**
 * triggerGetTemp
 * Sets flag dhtUpdated to true for handling in loop()
 * called by Ticker getTempTimer
 */
void triggerGetTemp() {
  if (tempTaskHandle != NULL) {
	   xTaskResumeFromISR(tempTaskHandle);
  }
}

/**
 * Task to reads temperature from DHT11 sensor
 * @param pvParameters
 *    pointer to task parameters
 */
void tempTask(void *pvParameters) {
	Serial.println("tempTask loop started");
	while (1) // tempTask loop
  {
    if (tasksEnabled) {
      // Get temperature values
			getTemperature();
		}
    // Got sleep again
		vTaskSuspend(NULL);
	}
}

/**
 * getTemperature
 * Reads temperature from DHT11 sensor
 * @return bool
 *    true if temperature could be aquired
 *    false if aquisition failed
*/
 bool getTemperature() {
	// Reading temperature for humidity takes about 250 milliseconds!
	// Sensor readings may also be up to 2 seconds 'old' (it's a very slow sensor)
  TempAndHumidity newValues = dht.getTempAndHumidity();
	// Check if any reads failed and exit early (to try again).
	if (dht.getStatus() != 0) {
		Serial.println("DHT11 error status: " + String(dht.getStatusString()));
		return false;
	}

	float heatIndex = dht.computeHeatIndex(newValues.temperature, newValues.humidity);
  float dewPoint = dht.computeDewPoint(newValues.temperature, newValues.humidity);
  float cr = dht.getComfortRatio(cf, newValues.temperature, newValues.humidity);
  UNUSED(cr);

  String comfortStatus;
  switch(cf) {
    case Comfort_OK:
      comfortStatus = "Comfort_OK";
      break;
    case Comfort_TooHot:
      comfortStatus = "Comfort_TooHot";
      break;
    case Comfort_TooCold:
      comfortStatus = "Comfort_TooCold";
      break;
    case Comfort_TooDry:
      comfortStatus = "Comfort_TooDry";
      break;
    case Comfort_TooHumid:
      comfortStatus = "Comfort_TooHumid";
      break;
    case Comfort_HotAndHumid:
      comfortStatus = "Comfort_HotAndHumid";
      break;
    case Comfort_HotAndDry:
      comfortStatus = "Comfort_HotAndDry";
      break;
    case Comfort_ColdAndHumid:
      comfortStatus = "Comfort_ColdAndHumid";
      break;
    case Comfort_ColdAndDry:
      comfortStatus = "Comfort_ColdAndDry";
      break;
    default:
      comfortStatus = "Unknown:";
      break;
  };

  Serial.println(" T:" + String(newValues.temperature) + " H:" + String(newValues.humidity) + " I:" + String(heatIndex) + " D:" + String(dewPoint) + " " + comfortStatus);
	return true;//String(newValues.temperature), String(newValues.humidity), String(heatIndex), String(dewPoint);
}

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
        
        client_id = "espClient-" + String(WiFi.macAddress());
        Serial.printf("The client %s connects to the public MQTT broker\n", client_id.c_str());
        if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
            Serial.println("Public EMQX MQTT broker connected");
        } else {
            Serial.print("failed with state ");
            Serial.print(client.state());
            delay(2000);
        }
    }
    // Publish and subscribe
    //client.publish(topic, "Hi, I'm ESP32 ^^");
    //client.subscribe(topic);
    if (!bmp.begin()) {
	  Serial.println("Could not find a valid BMP085 sensor, check wiring!");
	  while (1) {}
    }

  initTemp();
  // Signal end of setup() to tasks
  tasksEnabled = true;
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

  float temperature = bmp.readTemperature();
  sprintf(temperature_str, "%f", temperature);
  Serial.print("Temperature: ");
  Serial.println(temperature_str);

  float pressure = bmp.readPressure();
  sprintf(pressure_str, "%f", pressure);
  Serial.print("Pressure: ");
  Serial.println(pressure_str);

  if (!tasksEnabled) {
  // Wait 2 seconds to let system settle down
  delay(2000);
  // Enable task that will read values from the DHT sensor
  tasksEnabled = true;
  if (tempTaskHandle != NULL) {
		vTaskResume(tempTaskHandle);
	  }
  }
  yield();

  float soil_moisture = analogRead(soil_moisture_sensor);
  sprintf(soil_moisture_str, "%f", soil_moisture);
  Serial.print("Soil Moisture: ");
  Serial.println(soil_moisture_str);

  float light_intensity = analogRead(ldrPin);
  sprintf(light_intensity_str, "%f", light_intensity);
  Serial.print("Light Intensity: ");
  Serial.println(light_intensity_str);

  float uv_intensity = analogRead(uv_sensor);
  sprintf(uv_intensity_str, "%f", uv_intensity);
  Serial.print("UV Intensity: ");
  Serial.println(uv_intensity_str);

  float mq2_val = analogRead(mq2);
  sprintf(mq2_val_str, "%f", mq2_val);
  Serial.print("MQ2: ");
  Serial.println(mq2_val_str);

  float mq135_val = analogRead(mq135);
  sprintf(mq135_val_str, "%f", mq135_val);
  Serial.print("MQ135: ");
  Serial.println(mq135_val_str);

  getTemperature();
  
  //Serial.println(client.connect(client_id.c_str(), mqtt_username, mqtt_password));
  if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) client.publish("/esp1/temp", temperature_str);
  if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) client.publish("/esp1/pressure", pressure_str);
  if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) client.publish("/esp1/soil_moisture", soil_moisture_str);
  if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) client.publish("/esp1/light_intensity", light_intensity_str);
  if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) client.publish("/esp1/uv_intensity", uv_intensity_str);
  if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) client.publish("/esp1/mq2", mq2_val_str);
  if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) client.publish("/esp1/mq135", mq135_val_str);
  //if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) client.publish("/esp1/humidity", humidity_str);
  client.loop();
  delay(500);
}