#include <Servo.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Update these with your WiFi and Maqiatto MQTT broker details
const char* ssid = "";
const char* password = "";
const char* mqtt_server = "maqiatto.com";
const int mqtt_port = 1883;
const char* mqtt_user = "";
const char* mqtt_password = "";
 
// Topics
const char* topic_left = "/left";
const char* topic_right = "/right";
const char* topic_drive = "/drive";
const char* topic_reverse = "/reverse";
const char* topic_speed = "/speed";
const char* topic_stop = "/stop";
const char* topic_center = "/center";
 
//Definitions 
#define motorPinRightDir  0//D2
#define motorPinRightSpeed 5//D1
const int servoPin = D4; 
const int dir = 0;
int speed = 255;
Servo myServo;
WiFiClient espClient;
PubSubClient client(espClient);
int motorSpeed = 0; // Max speed (range: 0-255)
 
// Function to set up WiFi
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
 
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
 
// Function to stop the motor
void stopMotor() {
  analogWrite(motorPinRightDir, 0);
  Serial.println("Motor stopped");
}
 
// Callback function when a message arrives
void callback(char* topic, byte* payload, unsigned int length) {
  payload[length] = '\0'; // Null-terminate the payload
  String message = String((char*)payload);
 
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  Serial.println(message);
 
//Controller, checking messages sent by mqtt and executing the corresponding function

  if (String(topic) == topic_left) {
    myServo.write(0); 
    Serial.println("Servo moved to 0 degrees");
  } else if (String(topic) == topic_right) {
    myServo.write(180); 
    Serial.println("Servo moved to 180 degrees");
  } else if (String(topic) == topic_drive) {
    digitalWrite(motorPinRightDir, 1);
    analogWrite(motorPinRightSpeed, speed);
    Serial.println("Motor moving forward");
  } else if (String(topic) == topic_reverse) {
    digitalWrite(motorPinRightDir, dir);
    analogWrite(motorPinRightSpeed, speed);
    Serial.println("Motor moving in reverse");
  } else if (String(topic) == topic_speed) {
    motorSpeed = message.toInt();
  } else if (String(topic) == topic_stop) {
    digitalWrite(motorPinRightDir, LOW);
    analogWrite(motorPinRightSpeed, 0);
  } else if (String(topic) == topic_center) {
    myServo.write(90); 
    Serial.println("Servo moved to Center");
  }
}
 
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ArduinoClient", mqtt_user, mqtt_password)) {
      Serial.println("connected");
      // Subscribe to the topics
      client.subscribe(topic_left);
      client.subscribe(topic_right);
      client.subscribe(topic_drive);
      client.subscribe(topic_reverse);
      client.subscribe(topic_speed);
      client.subscribe(topic_stop);
      client.subscribe(topic_center);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
 
void setup() {
  pinMode(motorPinRightDir, OUTPUT);
  pinMode(motorPinRightSpeed, OUTPUT);
  myServo.attach(servoPin);
 
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}
 
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  int dir = 0;
  int speed = 255;
}