#include <Wire.h>
#include <BH1750.h>
#include <WiFiNINA.h>
#include <PubSubClient.h>

// ---------- Wi-Fi ----------
const char* WIFI_SSID = "task2.1P";
const char* WIFI_PASSWORD = "arjun123";

// ---------- HiveMQ Cloud ----------
const char* MQTT_SERVER = "e0a2ffec7f624d149150762d9bbd7d81.s1.eu.hivemq.cloud";
const int MQTT_PORT = 8883;

const char* MQTT_USERNAME = "hivemq.webclient.1787817208740";
const char* MQTT_PASSWORD = "St5mqIG5@ai!bUbpbQudPkAddRoOnIDW";

const char* MQTT_TOPIC = "Terrarium/light";

//  Objects 
BH1750 lightMeter;

WiFiSSLClient wifiClient;
PubSubClient mqttClient(wifiClient);


// Wi-Fi connection 
void connectWiFi() {

  Serial.print("Connecting to Wi-Fi");

  while (WiFi.begin(WIFI_SSID, WIFI_PASSWORD) != WL_CONNECTED) {

    Serial.print(".");
    delay(3000);
  }

  Serial.println();
  Serial.println("Wi-Fi connected!");

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}


//  MQTT connection 
void connectMQTT() {

  while (!mqttClient.connected()) {

    Serial.print("Connecting to HiveMQ...");

    String clientID = "Nano33IoT-" + String(random(0xffff), HEX);

    if (mqttClient.connect(
          clientID.c_str(),
          MQTT_USERNAME,
          MQTT_PASSWORD
        )) {

      Serial.println("connected!");
    }

    else {

      Serial.print("failed, state = ");
      Serial.println(mqttClient.state());

      delay(5000);
    }
  }
}


//  Setup 
void setup() {

  Serial.begin(115200);

  delay(2000);

  // Start I2C
  Wire.begin();

  // Start BH1750
  if (!lightMeter.begin()) {

    Serial.println("BH1750 sensor not detected!");

    while (1) {
      delay(1000);
    }
  }

  Serial.println("BH1750 ready!");

  // Connect to Wi-Fi
  connectWiFi();

  // Configure MQTT
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
}


//  Main loop 
void loop() {

  // Reconnect Wi-Fi if necessary
  if (WiFi.status() != WL_CONNECTED) {

    connectWiFi();
  }


  // Reconnect MQTT if necessary
  if (!mqttClient.connected()) {

    connectMQTT();
  }


  // Keep MQTT connection alive
  mqttClient.loop();


  // Read light level
  float lux = lightMeter.readLightLevel();


  // Display lux value
  Serial.print("Light level: ");
  Serial.print(lux);
  Serial.println(" lux");


  // Convert lux value to text
  char message[20];

  snprintf(message, sizeof(message), "%.2f", lux);


  // Publish lux value to HiveMQ
  mqttClient.publish(MQTT_TOPIC, message);


  // Display published value
  Serial.print("Published: ");
  Serial.println(message);

  // Wait 5 seconds
  delay(5000);
}