#include "sensor.h"

Sensor::Sensor(const int data_pin) {
    SDI12 mySDI12(data_pin);
    this->mySDI12;
};


void Sensor::printInfo() {
    String command = "?I!";
    mySDI12.sendCommand(command);
    delay(100);

    String sdiResponse = mySDI12.readStringUntil('\n');
    sdiResponse.trim();
    // allccccccccmmmmmmvvvxxx...xx<CR><LF>

    Serial.println("Sensor Address, Protocol Version, Sensor Vendor, Sensor Model, "
        "Sensor Version, Sensor ID");
    Serial.print(sdiResponse.substring(0, 1));  // address
    Serial.print(", ");
    Serial.print(sdiResponse.substring(1, 3).toFloat() / 10);  // SDI-12 version number
    Serial.print(", ");
    Serial.print(sdiResponse.substring(3, 11));  // vendor id
    Serial.print(", ");
    Serial.print(sdiResponse.substring(11, 17));  // sensor model
    Serial.print(", ");
    Serial.print(sdiResponse.substring(17, 20));  // sensor version
    Serial.print(", ");
    Serial.print(sdiResponse.substring(20));  // sensor id
    Serial.print(", ");
}

void Sensor::start() {
    Serial.println("Starting Sensor.");

    Serial.println("Opening SDI-12 bus...");
    mySDI12.begin();

    delay(3000);  // allow things to settle

    Serial.print("Timeout value: ");
    Serial.println(mySDI12.TIMEOUT);

    printInfo();
    Serial.println();
    Serial.println();

    delay(100);

    Serial.println("Sensor Started.");
};

void Sensor::takeMeasurement() {
    String command = "?M!";
    mySDI12.sendCommand(command);

    // Wait the required amount of time until the measurements are reading
    String sdiResponse = mySDI12.readStringUntil('\n');
    sdiResponse.trim();
    int time = sdiResponse.substring(1, sdiResponse.length() - 1).toInt();
    delay(1000 * time);
};

double Sensor::readSensor(String id) {
    String command = "?D" + id + "!";
    mySDI12.sendCommand(command);
    delay(100);

    String sdiResponse = mySDI12.readStringUntil('\n');
    sdiResponse.trim();
    return sdiResponse.substring(2, sdiResponse.length()).toDouble();
}


Data Sensor::getData() {
    Serial.println("Sensor");
    takeMeasurement();
    Data data;

    data.temperature_kelvin = readSensor("0");
    data.nitrate_mg_P_L = readSensor("1");
    data.nitrate_mV = readSensor("2");
    data.specificConductivity_mS_P_cm = readSensor("3");
    data.salinity_psu = readSensor("4");
    data.totalDissolvedSolids_g_P_L = readSensor("5");
    data.rawConductivity_uS_P_cm = readSensor("6");
    data.pH_units = readSensor("7");
    data.pH_mV = readSensor("8");
    data.referece_mV = readSensor("9");


    Serial.println(data.toString());
    return data;
};

boolean Sensor::active() {
    mySDI12.sendCommand("?a");
    delay(100);

    String sdiResponse = mySDI12.readStringUntil('\n');
    sdiResponse.trim();
    if (sdiResponse.substring(0, 1) == "0") {
        return false;
    }
    else {
        return true;
    }
}