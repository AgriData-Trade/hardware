// libraries
#include <MKRNB.h>
#include <ArduinoMqttClient.h>

#include "secrets.h"

const char PINNUMBER[] = SECRET_PINNUMBER;
const char broker[] = BROKER;
const String topic = String(BASE_TOPIC) + "/" + String(DEVICE_ID) + "/";
const int port = PORT;
const unsigned long interval = MINUTE_DELAY * 60 * 1000;
unsigned long previousMillis = interval;

// initialize the library instance
NBClient client;
GPRS gprs;
NB nbAccess;

MqttClient mqttClient(client);

void setup() {
  // initialize serial communications and wait for port to open:
  Serial.begin(9600);
  // while (!Serial) {
  //   ; // wait for serial port to connect. Needed for native USB port only
  // }

  Serial.println("Starting AgriData Sensor Node.");


  Serial.println("Warming up....");
  // connection state
  boolean connected = false;

  while (!connected) {
    if ((nbAccess.begin() == NB_READY) &&
      (gprs.attachGPRS() == GPRS_READY)) {
      connected = true;
    }
    else {
      Serial.println("Not connected");
      delay(1000);
    }
  }

  Serial.println("Connected to Cellular network.");
  Serial.print("Sending messages at an interval of: ");
  Serial.print(interval);
  Serial.println(" milliseconds.");

  Serial.println("Connecting to mqtt broker.");
    if (!mqttClient.connect(broker, port)) {
      Serial.print("MQTT connection failed! Error code = ");
      Serial.println(mqttClient.connectError());
    }
    mqttClient.setTimeout(0);
    Serial.println("Connected to mqtt broker.");

    // generate random number seed
    randomSeed(analogRead(0));
}

void loop() {
  // to avoid having delays in loop, we'll use the strategy from BlinkWithoutDelay
  // see: File -> Examples -> 02.Digital -> BlinkWithoutDelay for more info
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // save the last time a message was sent
    previousMillis = currentMillis;


    int temp = random(10, 30);
    Serial.print("Sending message to topic: (");
    Serial.print(topic + "temperature, ");
    Serial.print(temp);
    Serial.println(")");
    mqttClient.beginMessage(topic + "temperature");
    mqttClient.print(temp);
    mqttClient.endMessage();
    Serial.println("Sent Message.\n");

  }
}
