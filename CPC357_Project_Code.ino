/*
  ESP32 publish telemetry data to VOne Cloud (Infrared sensor)
*/

#include "VOneMqttClient.h"
#include <ESP32Servo.h>

//define device id
const char* InfraredSensor1 = "db1f4521-e6b1-46bb-9fd4-b83b63704204";  
const char* InfraredSensor2 = "2b36c47c-b47a-4a32-9c88-bce8bf5d4c38";
const char* InfraredSensor3 = "267bce84-1bb6-4ad6-ac35-14115bd97805";  
const char* InfraredSensor4 = "819e016b-de52-4c45-92b2-048c3a133b06";
const char* EmergencyButton1 = "ef210f82-bcf6-4bce-87c0-05d08121f698";
const char* EmergencyButton2 = "a3a7da3a-8307-408a-9f42-e77e9ac10291";
const char* ServoGate = "27f41a01-64fa-4687-9214-b5e8bafc7dc4";

Servo servo;

//Used Pins
const int InfraredPin1 = 14;      //Right side Maker Port
const int InfraredPin2 = 21 ;
const int InfraredPin3 = 7;      //Right side Maker Port
const int InfraredPin4 = 42;
const int ButtonPin1 = 39;
const int ButtonPin2 = 40;
const int ServoPin = 41;
//input sensor


const int minPulse = 500;  // Default min pulse width for SG90 servo
const int maxPulse = 2500; // Default max pulse width for SG90 servo
const int OpenAngle = 90;  // Maximum angle for the servo
const int CloseAngle = 0;  // Angle to close the gate
int currentAngle = 0; // Current angle of the servo

//Create an instance of VOneMqttClient
VOneMqttClient voneClient;

//last message time
unsigned long lastMsgTime = 0;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup() {

  setup_wifi();
  voneClient.setup();

  //sensor
  pinMode(InfraredPin1, INPUT_PULLUP);
  pinMode(InfraredPin2, INPUT_PULLUP);
  pinMode(InfraredPin3, INPUT_PULLUP);
  pinMode(InfraredPin4, INPUT_PULLUP);
  pinMode(ButtonPin1, INPUT_PULLUP);
  pinMode(ButtonPin2, INPUT_PULLUP);

  servo.attach(ServoPin, minPulse, maxPulse);
  servo.setPeriodHertz(50); // Set PWM frequency
  servo.write(currentAngle); // Initialize servo at 0 degrees
  delay(1000); // Wait for the servo to stabilize

  Serial.println("Servo control initialized.");
}

void loop() {

  if (!voneClient.connected()) {
    voneClient.reconnect();
    voneClient.publishDeviceStatusEvent(InfraredSensor1, true);
    voneClient.publishDeviceStatusEvent(InfraredSensor2, true);
    voneClient.publishDeviceStatusEvent(InfraredSensor3, true);
    voneClient.publishDeviceStatusEvent(InfraredSensor4, true);
    voneClient.publishDeviceStatusEvent(EmergencyButton1, true);
    voneClient.publishDeviceStatusEvent(EmergencyButton2, true);
    voneClient.publishDeviceStatusEvent(ServoGate, true);
  }
  voneClient.loop();
  
  unsigned long cur = millis();
  if (cur - lastMsgTime > INTERVAL) {
    lastMsgTime = cur;
    
    //Publish telemetry data
    int InfraredVal1 = !digitalRead(InfraredPin1);
    voneClient.publishTelemetryData(InfraredSensor1, "Car1", InfraredVal1);

    int InfraredVal2 = !digitalRead(InfraredPin2);
    voneClient.publishTelemetryData(InfraredSensor2, "Car2", InfraredVal2);

    int InfraredVal3 = !digitalRead(InfraredPin3);
    voneClient.publishTelemetryData(InfraredSensor3, "Car3", InfraredVal3);

    int InfraredVal4 = !digitalRead(InfraredPin4);
    voneClient.publishTelemetryData(InfraredSensor4, "Car4", InfraredVal4);

    int EmergencyVal1 = digitalRead(ButtonPin1);
    if(EmergencyVal1 == HIGH)
    {
      EmergencyVal1 = 0;
    }
    else
    {
       EmergencyVal1 = 1;
    }
    voneClient.publishTelemetryData(EmergencyButton1, "Button1", EmergencyVal1);  

    int EmergencyVal2 = digitalRead(ButtonPin2);
    if(EmergencyVal2 == HIGH)
    {
      EmergencyVal2 = 0;
    }
    else
    {
       EmergencyVal2 = 1;
    }
    voneClient.publishTelemetryData(EmergencyButton2, "Button2", EmergencyVal2); 


    if (InfraredVal1 == 1 && InfraredVal2 == 1 && InfraredVal3 == 1 && InfraredVal4 == 1){
       if (currentAngle != CloseAngle) {
        servo.write(CloseAngle);
        currentAngle = CloseAngle;
        Serial.println("All parking full. Gate closed.");
      }

    } else {
      if (currentAngle != OpenAngle) {
        servo.write(OpenAngle);
        currentAngle = OpenAngle;
        Serial.println("At least one parking clear. Gate opened.");
      }
    }
    voneClient.publishTelemetryData(ServoGate, "Servo", currentAngle);
  }
}