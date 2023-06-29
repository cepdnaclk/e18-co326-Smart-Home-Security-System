#include <WiFi.h>
#include <PubSubClient.h>

// MQTT Broker Settings
const char* mqtt_server = "192.168.91.127";
const char* mqtt_topic = "UoP_CO_326_E18_04_DoorSensor";
const int mqtt_port = 1883;

// Wi-Fi Settings
const char* ssid = "nadeeJ";
const char* password = "jaya1122";



// Sensor Pin
const int DoorSensorPin = 21;

int currentDoorState; // current state of door sensor
int lastDoorState;    // previous state of door sensor

//int DoorSensorValue = 0;

// Actuator Pin
//const int actuatorPin = 22;

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
  //client.publish(mqtt_topic, "Hello From ESP32!");
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

  pinMode(DoorSensorPin, INPUT_PULLUP); // set ESP32 pin to input pull-up mode

  currentDoorState = digitalRead(DoorSensorPin); // read state

}

 
void loop() {
  // Read sensor value
  lastDoorState = currentDoorState;              // save the last state
  currentDoorState  = digitalRead(DoorSensorPin); // read new state

  // Publish sensor data to MQTT broker
  if (lastDoorState == LOW && currentDoorState == HIGH) { // state change: LOW -> HIGH
    client.publish(mqtt_topic, "The door-opening event is detected");
    // TODO: turn on alarm, light or send notification ...
  }
  else
  if (lastDoorState == HIGH && currentDoorState == LOW) { // state change: HIGH -> LOW
    client.publish(mqtt_topic, "The door-closing event is detected");
    // TODO: turn off alarm, light or send notification ...
  }
  
  //String payload = String(sensorValue);
  //client.publish(mqtt_topic, payload.c_str());

  delay(1000);

  client.loop();

}











