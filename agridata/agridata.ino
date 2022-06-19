#include "secrets.h"
#include "network.h"
#include <SDI12.h>

const bool TEST = false;
const char sim_pin_code[] = SECRET_PINNUMBER;

String base_topic = String(BASE_TOPIC) + "/" + String(DEVICE_ID);
const char mqtt_broker[] = MQTT_BROKER;
const int mqtt_port = MQTT_PORT;
const unsigned long interval = 180 * 60 * 1000;
unsigned long previousMillis = interval;

#define SERIAL_BAUD 9600 /*!< The baud rate for the output serial port */
#define DATA_PIN 7         /*!< The pin of the SDI-12 data bus */

/** Define the SDI-12 bus */
SDI12 mySDI12(DATA_PIN);

/** Define network client */
NetworkClient networkClient(sim_pin_code, mqtt_broker, mqtt_port, base_topic.c_str());


// keeps track of the wait time for each active addresses
uint8_t waitTime = 0;

// keeps track of the time each sensor was started
uint32_t millisStarted = 0;

// keeps track of the time each sensor will be ready
uint32_t millisReady = 0;

// keeps track of the number of results expected
uint8_t returnedResults = 0;

float results[9];

/**
 * @brief converts allowable address characters ('0'-'9', 'a'-'z', 'A'-'Z') to a
 * decimal number between 0 and 61 (inclusive) to cover the 62 possible
 * addresses.
 */
byte charToDec(char i) {
  if ((i >= '0') && (i <= '9')) return i - '0';
  if ((i >= 'a') && (i <= 'z')) return i - 'a' + 10;
  if ((i >= 'A') && (i <= 'Z'))
    return i - 'A' + 36;
  else
    return i;
}

/**
 * @brief maps a decimal number between 0 and 61 (inclusive) to allowable
 * address characters '0'-'9', 'a'-'z', 'A'-'Z',
 */
char decToChar(byte i) {
  if (i < 10) return i + '0';
  if ((i >= 10) && (i < 36)) return i + 'a' - 10;
  if ((i >= 36) && (i <= 62))
    return i + 'A' - 36;
  else
    return i;
}

/**
 * @brief gets identification information from a sensor, and prints it to the serial
 * port
 *
 * @param i a character between '0'-'9', 'a'-'z', or 'A'-'Z'.
 */
void printInfo(char i) {
  String command = "";
  command += (char)i;
  command += "I!";
  mySDI12.sendCommand(command);
  delay(100);

  String sdiResponse = mySDI12.readStringUntil('\n');
  sdiResponse.trim();
  // allccccccccmmmmmmvvvxxx...xx<CR><LF>
  Serial.println("Sensor Address: " + sdiResponse.substring(0, 1));
  Serial.println("Protocol Version: " + String(sdiResponse.substring(1, 3).toFloat() / 10));
  Serial.println("Sensor Vendor: " + sdiResponse.substring(3, 11));
  Serial.println("Sensor Model: " + sdiResponse.substring(11, 17));
  Serial.println("Sensor Version: " + sdiResponse.substring(17, 20));
  Serial.println("Sensor ID: " + sdiResponse.substring(20));
}

bool getResults(char i, int resultsExpected) {
  uint8_t resultsReceived = 0;
  uint8_t cmd_number = 0;
  while (resultsReceived < resultsExpected && cmd_number <= 9) {
    String command = "";
    // in this example we will only take the 'DO' measurement
    command = "";
    command += i;
    command += "D";
    command += cmd_number;
    command += "!";  // SDI-12 command to get data [address][D][dataOption][!]
    mySDI12.sendCommand(command);

    uint32_t start = millis();
    while (mySDI12.available() < 3 && (millis() - start) < 1500) {}
    mySDI12.read();           // ignore the repeated SDI12 address
    char c = mySDI12.peek();  // check if there's a '+' and toss if so
    if (c == '+') { mySDI12.read(); }

    while (mySDI12.available()) {
      char c = mySDI12.peek();
      if (c == '-' || (c >= '0' && c <= '9') || c == '.') {
        float result = mySDI12.parseFloat(SKIP_NONE);
        if (result != -9999) {
          if (resultsReceived > 0) {
            results[resultsReceived - 1] = result;
          }
          resultsReceived++;
        }
      }
      else if (c == '+') {
        mySDI12.read();
      }
      else {
        mySDI12.read();
      }
      delay(10);  // 1 character ~ 7.5ms
    }
    if (resultsReceived < resultsExpected) { Serial.print(", "); }
    cmd_number++;
  }
  mySDI12.clearBuffer();

  return resultsReceived == resultsExpected;
}

int startMeasurement(char i, String meas_type = "") {
  mySDI12.clearBuffer();
  String command = "";
  command += i;
  command += "M";
  command += meas_type;
  command += "!";  // SDI-12 measurement command format  [address]['M'][!]
  mySDI12.sendCommand(command);
  delay(30);

  // wait for acknowledgement with format [address][ttt (3 char, seconds)][number of
  // measurments available, 0-9]
  String sdiResponse = mySDI12.readStringUntil('\n');
  sdiResponse.trim();

  // find out how long we have to wait (in seconds).
  uint8_t wait = sdiResponse.substring(1, 4).toInt();

  // Set up the number of results to expect
  int numResults = sdiResponse.substring(4).toInt();

  uint8_t sensorNum = charToDec(i);  // e.g. convert '0' to 0, 'a' to 10, 'Z' to 61.
  waitTime = wait;
  millisStarted = millis();
  if (wait == 0) {
    millisReady = millis();
  }
  else {
    millisReady = millis() + wait * 1000;
  }
  Serial.print("Waiting " + String(wait) + " seconds for sensor " + String(sensorNum) + " to be ready...");

  return numResults;
}

void setup() {
  Serial.begin(SERIAL_BAUD);

  // Allow a serial connection to the computer
  delay(3000);

  Serial.print("Sending messages at an interval of: ");
  Serial.print(interval);
  Serial.println(" milliseconds.");

  Serial.println("Opening SDI-12 bus...");
  mySDI12.begin();
  delay(500);  // allow things to settle

  Serial.print("Timeout value: ");
  Serial.println(mySDI12.TIMEOUT);

  char addr = decToChar((byte)0);
  printInfo(addr);
  Serial.println();

  Serial.println("Setting up Network Client.");
  networkClient.connect();
  Serial.println("Network Client started.");
}

void loop() {
  if (millis() - previousMillis >= interval) {
    previousMillis = millis();
    Serial.println("Starting measurement");
    char addr = decToChar((byte)0);
    startMeasurement(addr);

    while (millis() <= millisReady) {
      Serial.print(".");
      delay(1000);
    }

    Serial.println();
    Serial.print("Time Elapsed (s): ");
    Serial.println(millis() / 1000);
    Serial.println("Printing results.");
    getResults(addr, 9);
    Serial.println();


    Serial.println("Displaying results");
    Data data;
    data.temperature_kelvin = results[0];
    data.nitrate_mg_P_L = results[1];
    data.nitrate_mV = results[2];
    data.specificConductivity_mS_P_cm = results[3];
    data.salinity_psu = results[4];
    data.totalDissolvedSolids_g_P_L = results[5];
    data.rawConductivity_uS_P_cm = results[6];
    data.pH_units = results[7];
    data.pH_mV = results[8];
    data.referece_mV = results[9];

    Serial.println(data.toString());
    networkClient.send_data(data, base_topic);

    Serial.println();
  }
}