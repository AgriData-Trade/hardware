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

// setup network access
NBClient client;
GPRS gprs;
NB nbAccess;

// mqtt client
MqttClient mqttClient(client);

void setup() {
  // initialize serial communications and wait for port to open:
  Serial.begin(9600);

  Serial.println("Starting AgriData Sensor Node.");


  Serial.println("Warming up....");
  Serial.println("Connecting to network.");
  //Connects to network to use getTime()
  if (nbAccess.status() != NB_READY || gprs.status() != GPRS_READY) {
    connectNB();
  }
  Serial.println("Connected to network.");

  Serial.print("Sending messages at an interval of: ");
  Serial.print(interval);
  Serial.println(" milliseconds.");

  Serial.println("Connecting to mqtt broker.");
  if (!mqttClient.connected()) {
    // MQTT client is disconnected, connect
    connectMQTT();
  }

  // generate random number seed
  randomSeed(analogRead(0));
}

void loop() {
  // to avoid having delays in loop, we'll use the strategy from BlinkWithoutDelay
  // see: File -> Examples -> 02.Digital -> BlinkWithoutDelay for more info
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    Serial.println("******Sending Data to MQTT - STARTED******");
    // save the last time a message was sent
    previousMillis = currentMillis;
    if (nbAccess.status() != NB_READY || gprs.status() != GPRS_READY) {
    connectNB();
  }
    if (!mqttClient.connected()) {
    // MQTT client is disconnected, connect
    connectMQTT();
  }

    int temp = random(10, 30);
    Serial.print("Sending message to topic: (");
    Serial.print(topic + "temperature, ");
    Serial.print(temp);
    Serial.println(")");
    mqttClient.beginMessage(topic + "temperature");
    mqttClient.print(temp);
    mqttClient.endMessage();
    Serial.println("******Sending Data to MQTT - COMPLETED******");
  }
}

void connectNB() {
  Serial.println("******Connecting to Cellular Network - STARTED******");

  while ((nbAccess.begin() != NB_READY) ||
         (gprs.attachGPRS() != GPRS_READY)) {
    // failed, retry
    Serial.print("NB");
    delay(1000);
  }

  Serial.println("******Connecting to Cellular Network - COMPLETED******");
  Serial.println();
}

void connectMQTT() {
 Serial.println("******Connecting to MQTT Broker - STARTED******");
  Serial.print(broker);

  while (!mqttClient.connect(broker, port)) {
    // failed, retry
    Serial.print(".");
    Serial.println(mqttClient.connectError());
    delay(5000);
    if (nbAccess.status() != NB_READY || gprs.status() != GPRS_READY) {
    connectNB();
  } 
  }
  Serial.println("\n******Connecting to MQTT Broker - COMPLETED******");
  Serial.println();
}
