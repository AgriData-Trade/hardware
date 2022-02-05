#include <ArduinoRS485.h>
#include <ArduinoModbus.h>

#include "secrets.h"
#include "network.h"

const char sim_pin_code[] = SECRET_PINNUMBER;

String base_topic = String(BASE_TOPIC) + "/" + String(DEVICE_ID) + "/";

const char mqtt_broker[] = MQTT_BROKER;
const int mqtt_port = MQTT_PORT;
const unsigned long interval = MINUTE_DELAY * 60 * 1000;

unsigned long previousMillis = interval;

NetworkClient client(sim_pin_code, mqtt_broker, mqtt_port, base_topic.c_str());

void setup() {
  // initialize serial communications and wait for port to open:
  Serial.begin(9600);

  Serial.println("Starting AgriData Sensor Node.");
  Serial.println("Warming up....");


  Serial.print("Sending messages at an interval of: ");
  Serial.print(interval);
  Serial.println(" milliseconds.");

  Serial.println("Setting up Modbus Client.");
  if (!ModbusRTUClient.begin(9600)) {
    Serial.println("Failed to start Modbus RTU Client!");
    while (1);
  }
  Serial.println("Modbus Client started.");

  Serial.println("Setting up Network Client,");
  //client.connect();
  Serial.println("Network Client started.");
}

void loop() {

  unsigned long currentMillis = millis();

  // check to see if it's time to send another message
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    Serial.println("Hey");
    Serial.println("Done");
    Data data = getData();
    client.send_data(data);
  }
}

/**
 * @brief Get the data from the sensor
 *
 * @return The data from the sensor
 */
Data getData() {
  Data data;

  // send a Holding registers read request to (slave) id 1, for 2 registers
  if (!ModbusRTUClient.requestFrom(1, HOLDING_REGISTERS, 0x00, 12)) {
    Serial.print("failed to read registers! ");
    Serial.println(ModbusRTUClient.lastError());
  }

  data.battery_voltage = ModbusRTUClient.read();
  data.clockBattery_voltage = ModbusRTUClient.read();
  data.temperature_celsius = ModbusRTUClient.read();
  data.nitrate_mg_P_L = ModbusRTUClient.read();
  data.nitrate_mV = ModbusRTUClient.read();
  data.specificConductivity_mS_P_cm = ModbusRTUClient.read();
  data.salinity_psu = ModbusRTUClient.read();
  data.totalDissolvedSolids_g_P_L = ModbusRTUClient.read();
  data.rawCoductivity_uS_P_cm = ModbusRTUClient.read();
  data.pH_units = ModbusRTUClient.read();
  data.pH_mV = ModbusRTUClient.read();
  data.referece_mV = ModbusRTUClient.read();
  return data;
}
