#include <ESP8266WiFi.h>  // Use <WiFi.h> for ESP32
#include <PubSubClient.h>
#include <Servo.h>

// WiFi credentials
const char* ssid = "ABB_Gym_IOT";
const char* password = "welcome2abb";

// MQTT broker details
const char* mqtt_server = "maqiatto.com";
const int mqtt_port = 8883;  // Use 8883 for secure connections
const char* mqtt_user = "kale.kalsong@gmail.com";
const char* mqtt_password = "Kals0ngM4ann9n!?";

// MQTT topics
const char* topic_forward = "kale.kalsong@gmail.com/forward";
const char* topic_reverse = "kale.kalsong@gmail.com/reverse";
const char* topic_left = "kale.kalsong@gmail.com/left";
const char* topic_right = "kale.kalsong@gmail.com/right";

// Initialize WiFi and MQTT client
WiFiClient espClient;
PubSubClient client(espClient);

// Servo and motor pins
const int servoPin = D1;
const int motorPin1 = D2;
const int motorPin2 = D3;
const int motorSpeedPin = D4;

Servo myServo;
int currentAngle = 90;  // Start at 90 degrees (center)

void setup() {
  Serial.begin(115200);
  
  // Connect to WiFi
  setup_wifi();
  
  // Setup MQTT
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  
  // Attach the servo
  myServo.attach(servoPin);
  myServo.write(currentAngle);

  // Set motor pins as outputs
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  pinMode(motorSpeedPin, OUTPUT);
}

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

void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  message.trim();
  
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  Serial.println(message);

  if (String(topic) == topic_left) {
    if (currentAngle > 0) {
      currentAngle -= 10;
      if (currentAngle < 0) currentAngle = 0;
      myServo.write(currentAngle);
      Serial.print("Turning Left: ");
      Serial.println(currentAngle);
    }
  } else if (String(topic) == topic_right) {
    if (currentAngle < 180) {
      currentAngle += 10;
      if (currentAngle > 180) currentAngle = 180;
      myServo.write(currentAngle);
      Serial.print("Turning Right: ");
      Serial.println(currentAngle);
    }
  } else if (String(topic) == topic_forward) {
    analogWrite(motorSpeedPin, 100);  // Full speed
    digitalWrite(motorPin1, HIGH);
    digitalWrite(motorPin2, LOW);
    Serial.println("Moving Forward");
  } else if (String(topic) == topic_reverse) {
    analogWrite(motorSpeedPin, 100);  // Full speed
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, HIGH);
    Serial.println("Reversing");
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ArduinoClient", mqtt_user, mqtt_password)) {
      Serial.println("connected");
      // Subscribe to the relevant topics
      client.subscribe(topic_forward);
      client.subscribe(topic_reverse);
      client.subscribe(topic_left);
      client.subscribe(topic_right);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  Serial.println("Hererere");
  Serial.println(client.state());
  client.loop();
}
