// libraries
#include <MKRNB.h>
#include <ArduinoMqttClient.h>

#include "secrets.h"

const char PINNUMBER[] = SECRET_PINNUMBER;
const char broker[] = BROKER;
const String id_topic = String(BASE_TOPIC) + "/" + String(DEVICE_ID) + "/";
const int port = PORT;
const unsigned long interval = MINUTE_DELAY * 60 * 1000;
unsigned long previousMillis = interval;

// setup network access
NBClient client;
GPRS gprs;
NB nbAccess;

// mqtt client
MqttClient mqttClient(client);

// Data storage
struct Data {
  float battery_voltage;
  float clockBattery_voltage;
  float temperature_celsius;
  float nitrate_mg_P_L;
  float nitrate_mV;
  float speciicConductivity_mS_P_cm;
  float salinity_psu;
  float totalDissolvedSolids_g_P_L;
  float rawCoductivity_uS_P_cm;
  float pH_units;
  float pH_mV;
  float referece_mV;
} data;

void setup() {
  // initialize serial communications and wait for port to open:
  Serial.begin(9600);

  Serial.println("Starting AgriData Sensor Node.");


  Serial.println("Warming up....");
  Serial.print("Sending messages at an interval of: ");
  Serial.print(interval);
  Serial.println(" milliseconds.");
  Serial.println("Connecting to network.");
  //Connects to network to use getTime()
  if (nbAccess.status() != NB_READY || gprs.status() != GPRS_READY) {
    connectNB();
  }
  Serial.println("Connected to network.");

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
    Serial.println("******Running Task - STARTED******");
    // save the last time a message was sent
    previousMillis = currentMillis;
    if (nbAccess.status() != NB_READY || gprs.status() != GPRS_READY) {
      connectNB();
    }
    if (!mqttClient.connected()) {
      // MQTT client is disconnected, connect
      connectMQTT();
    }

    // Collect and publish data
    getData();
    publishData();


    Serial.println("******Running Task - COMPLETED******");
  }
}

void getData() {
  Serial.println("******Collecting Data - STARTED******");
  data.battery_voltage = random(10, 30);
  data.clockBattery_voltage = random(10, 30);
  data.temperature_celsius = random(10, 30);
  data.nitrate_mg_P_L = random(10, 30);
  data.nitrate_mV = random(10, 30);
  data.speciicConductivity_mS_P_cm = random(10, 30);
  data.salinity_psu = random(10, 30);
  data.totalDissolvedSolids_g_P_L = random(10, 30);
  data.rawCoductivity_uS_P_cm = random(10, 30);
  data.pH_units = random(10, 30);
  data.pH_mV = random(10, 30);
  data.referece_mV = random(10, 30);
  Serial.println("******Collecting Data - FINISHED******");
}

void publishData() {
  Serial.println("******Publishing Data - STARTED******");
  publishMessage(id_topic + "battery_voltage", data.battery_voltage);
  publishMessage(id_topic + "clockBattery_voltage", data.clockBattery_voltage);
  publishMessage(id_topic + "temperature_celsius", data.temperature_celsius);
  publishMessage(id_topic + "nitrate_mg_P_L", data.nitrate_mg_P_L);
  publishMessage(id_topic + "nitrate_mV", data.nitrate_mV);
  publishMessage(id_topic + "speciicConductivity_mS_P_cm", data.speciicConductivity_mS_P_cm);
  publishMessage(id_topic + "salinity_psu", data.salinity_psu);
  publishMessage(id_topic + "totalDissolvedSolids_g_P_L", data.totalDissolvedSolids_g_P_L);
  publishMessage(id_topic + "rawCoductivity_uS_P_cm", data.rawCoductivity_uS_P_cm);
  publishMessage(id_topic + "pH_units", data.pH_units);
  publishMessage(id_topic + "pH_mV", data.pH_mV);
  publishMessage(id_topic + "referece_mV", data.referece_mV);
  Serial.println("******Publishing Data - FINISHED******");
}

void publishMessage(String topic, float message) {
  // publish data to mqtt broker
  mqttClient.beginMessage(topic);
  mqttClient.print(message);
  mqttClient.endMessage();
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
