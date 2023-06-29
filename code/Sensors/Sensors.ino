#include <WiFi.h>
#include <PubSubClient.h>

// MQTT Broker Settings
const char* mqtt_server = "192.168.91.127";
const char* pirTopic = "UoP_CO_326_E18_04_PIRSensor";
const char* doorTopic = "UoP_CO_326_E18_04_DoorSensor";
const int mqtt_port = 1883;

// Wi-Fi Settings
const char* ssid = "nadeeJ";
const char* password = "jaya1122";

// Sensor Pin
const int sensorPin = 19;
int sensorPIRValue = 0;

// Door Sensor Pin
const int doorSensorPin = 21;
int doorSensorValue = 0;

int currentDoorState; // current state of door sensor
int lastDoorState;    // previous state of door sensor

WiFiClient espClient;
PubSubClient client(espClient);

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message: ");
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }

  Serial.println();
  Serial.println(" - - - - - - - - - - - -");

  pinMode(doorSensorPin, INPUT_PULLUP); // set ESP32 pin to input pull-up mode

  currentDoorState = digitalRead(doorSensorPin); // read state

}

void setup() {
  Serial.begin(115200);
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi!");

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  while (!client.connected()) {
    String client_id = "esp32-client-";
    client_id += String(WiFi.macAddress());
    
    Serial.printf("The client %s connects to mosquitto mqtt broker\n", client_id.c_str());        
  
    if (client.connect(client_id.c_str())) {
      Serial.println("Connected to MQTT broker");
      client.subscribe(pirTopic);
      client.subscribe(doorTopic);
    } else {
      Serial.print("Failed with state ");
      Serial.print(client.state());
      Serial.println(" Retrying...");
      delay(2000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  
  client.loop();

  sensorPIRValue = digitalRead(sensorPin);
  
  lastDoorState = currentDoorState;              // save the last state
  currentDoorState  = digitalRead(doorSensorPin); // read new state

  if (sensorPIRValue == 1) {
    client.publish(pirTopic, "Motion detected");
  } else {
    client.publish(pirTopic, "Motion not detected");
  }

  //client.publish(doorTopic, String(currentDoorState).c_str(), String(lastDoorState).c_str());
  //client.publish(doorTopic, String(lastDoorState).c_str());

  if (currentDoorState == HIGH) {
    client.publish(doorTopic, "Door opened");
  } 
  else
  if (currentDoorState == LOW) {
    client.publish(doorTopic, "Door closed");
  } 

  delay(1000);
}

void reconnect() {
  while (!client.connected()) {
    String client_id = "esp32-client-";
    client_id += String(WiFi.macAddress());
    
    Serial.printf("The client %s connects to mosquitto mqtt broker\n", client_id.c_str());
  
    if (client.connect(client_id.c_str())) {
      Serial.println("Connected to MQTT broker");
      client.subscribe(pirTopic);
      client.subscribe(doorTopic);
    } else {
      Serial.print("Failed with state ");
      Serial.print(client.state());
      Serial.println(" Retrying...");
      delay(2000);
    }
  }
}