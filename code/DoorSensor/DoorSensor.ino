#define DOOR_SENSOR_PIN  21  // ESP32 pin GIOP19 connected to door sensor's pin

int currentDoorState; // current state of door sensor
int lastDoorState;    // previous state of door sensor

void setup() {
  Serial.begin(9600);                     // initialize serial

// Door sensor
  pinMode(DOOR_SENSOR_PIN, INPUT_PULLUP); // set ESP32 pin to input pull-up mode

  currentDoorState = digitalRead(DOOR_SENSOR_PIN); // read state

//  PIR motion sensor
  pinMode(22, INPUT_PULLUP);
}

void loop() {
  lastDoorState = currentDoorState;              // save the last state
  currentDoorState  = digitalRead(DOOR_SENSOR_PIN); // read new state

//  PIR motion sensor detect
  int PIR = digitalRead(22);

// Door Sensor connectivity
  if (lastDoorState == LOW && currentDoorState == HIGH) { // state change: LOW -> HIGH
    Serial.println("The door-opening event is detected");
    // TODO: turn on alarm, light or send notification ...
  }
  else
  if (lastDoorState == HIGH && currentDoorState == LOW) { // state change: HIGH -> LOW
    Serial.println("The door-closing event is detected");
    // TODO: turn off alarm, light or send notification ...
  }

}
