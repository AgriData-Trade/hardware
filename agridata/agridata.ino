#include "secrets.h"
#include "network.h"
#include "sensor.h"

boolean TEST = false;

const char sim_pin_code[] = SECRET_PINNUMBER;

String base_topic = String(BASE_TOPIC) + "/" + String(DEVICE_ID) + "/";

const char mqtt_broker[] = MQTT_BROKER;
const int mqtt_port = MQTT_PORT;
const unsigned long interval = MINUTE_DELAY * 60 * 1000;

unsigned long previousMillis = interval;

Sensor mySensor(DATA_PIN);
NetworkClient networkClient(sim_pin_code, mqtt_broker, mqtt_port, base_topic.c_str());

void setup() {
  // initialize serial communications and wait for port to open:
  Serial.begin(9600);

  Serial.println("Starting AgriData Sensor Node.");
  Serial.println("Warming up....");

  if (!TEST) {
    mySensor.start();
  }


  Serial.print("Sending messages at an interval of: ");
  Serial.print(interval);
  Serial.println(" milliseconds.");

  Serial.println("Setting up Network Client,");
  networkClient.connect();
  Serial.println("Network Client started.");
}

void loop() {

  unsigned long currentMillis = millis();

  // check to see if it's time to send another message
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    Data data;
    if (TEST) {
      data = testData();
      networkClient.send_data(data, base_topic);
    }
    else {
      if (mySensor.active()) {
        Serial.println("Collecting data.");
        data = mySensor.getData();
        Serial.println("Collected data.");
      }
      else {
        Serial.println("Sensor not active.\nAttempting to wake it up. ");
        mySensor.start();
        int counter = 0;
        while (!mySensor.active()) {
          Serial.print(".");
          delay(1000);
          counter++;

          if (counter > interval / 1000) {
            Serial.println("Sensor not active.\nSending test data.");
            data = testData();
            networkClient.send_data(data, base_topic);
            counter = 0;
          }
        }

        Serial.println("Sensor active.");
      }
      Serial.println();
      Serial.println();
    }
  }
}


/**
 * @brief Get test data
 *
 * @return Test data
 */
Data testData() {
  Serial.println("Using test data.");
  Data data;

  data.temperature_kelvin = -1;
  data.nitrate_mg_P_L = -1;
  data.nitrate_mV = -1;
  data.specificConductivity_mS_P_cm = -1;
  data.salinity_psu = -1;
  data.totalDissolvedSolids_g_P_L = -1;
  data.rawConductivity_uS_P_cm = -1;
  data.pH_units = -1;
  data.pH_mV = -1;
  data.referece_mV = -1;
  return data;
}