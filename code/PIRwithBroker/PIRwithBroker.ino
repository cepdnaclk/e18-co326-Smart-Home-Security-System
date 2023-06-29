#include <WiFi.h>
#include <PubSubClient.h>

// MQTT Broker Settings
const char* mqtt_server = "192.168.91.94";
const char* mqtt_topic = "UoP_CO_326_E18_04_PIRSensor";
const int mqtt_port = 1883;

// Wi-Fi Settings
const char* ssid = "nadeeJ";
const char* password = "jaya1122";



// Sensor Pin
const int sensorPin = 19;
int sensorPIRValue = 0;

// Actuator Pin
const int actuatorPin = 22;

WiFiClient espClient; //wifi client, MQTT client
PubSubClient client(espClient);



void setup() {
  // Initialize Serial
  Serial.begin(115200);


  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi!");


  // Connect to MQTT Broker
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  
  while (!client.connected()) {
    String client_id = "esp32-client-";
    client_id += String(WiFi.macAddress());
    
    Serial.printf("The client %s connects to mosquitto mqtt broker\n", client_id.c_str());        
    
    
    if (client.connect(client_id.c_str())) {
      Serial.println("Public emqx mqtt broker connected");
    } else {
      Serial.print("failed with state ");
      Serial.print(client.state());
      Serial.println(" Retrying...");
      delay(2000);
    }
  }

  // publish and subscribe
  client.publish(mqtt_topic, "Hello From ESP32!");
  client.subscribe(mqtt_topic);
  
}


void callback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message:");
  
  for (int i = 0; i < length; i++) {
    Serial.print((char) payload[i]);
  }

  Serial.println();
  Serial.println(" - - - - - - - - - - - -");
 
}

 
void loop() {
  // Read PIR sensor value
  sensorPIRValue = digitalRead(sensorPin);

  // Read DOOR sensor value
  

  // Publish sensor data to MQTT broker
  if (sensorPIRValue == 1) {
    client.publish(mqtt_topic, "Motion detected");
  } else {
    client.publish(mqtt_topic, "Motion not detected");
  }
  
  //String payload = String(sensorValue);
  //client.publish(mqtt_topic, payload.c_str());
  

  delay(1000);

  client.loop();
}
