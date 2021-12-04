// libraries
#include <MKRNB.h>
#include <ArduinoMqttClient.h>

#include "secrets.h"

const char PINNUMBER[] = SECRET_PINNUMBER;
const char broker[] = BROKER;
const String topic = String(BASE_TOPIC) + "/" + String(DEVICE_ID) + "/";
const int port = PORT;
const unsigned long interval = HOUR_DELAY * 60 * 60 * 1000;
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

  Serial.println("connecting to the mqtt broker...");

  Serial.println(broker);

  if (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());

    while (1);
  }

  Serial.println("Connected to the MQTT broker!");
  Serial.println();

  // send startup message with location coords "lat, long"
  mqttClient.beginMessage(topic + "location");
  mqttClient.print("-41.314220, 174.822680");
  mqttClient.endMessage();

  // generate random number seed
  randomSeed(analogRead(0));
}

void loop() {
  // call poll() regularly to allow the library to send MQTT keep alives which
 // avoids being disconnected by the broker
  mqttClient.poll();

  // to avoid having delays in loop, we'll use the strategy from BlinkWithoutDelay
  // see: File -> Examples -> 02.Digital -> BlinkWithoutDelay for more info
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // save the last time a message was sent
    previousMillis = currentMillis;

    Serial.print("Sending message to topic: ");
    Serial.println(topic + "temperature");
    Serial.print("hello ");

    int temp = random(10, 30);
    mqttClient.beginMessage(topic);
    mqttClient.print(temp);
    mqttClient.endMessage();

    Serial.println();
  }
}
