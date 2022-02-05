#include "network.h"

/**
 * @brief Send data payload to the mqtt broker
 *
 * @param data
 */
void NetworkClient::send_data(Data data) {
    // Make sure we are connected to the network and mqtt broker
    connect();

    // Publish data
    send_message(*base_topic + "battery_voltage", data.battery_voltage);
    send_message(*base_topic + "clockBattery_voltage", data.clockBattery_voltage);
    send_message(*base_topic + "temperature_celsius", data.temperature_celsius);
    send_message(*base_topic + "nitrate_mg_P_L", data.nitrate_mg_P_L);
    send_message(*base_topic + "nitrate_mV", data.nitrate_mV);
    send_message(*base_topic + "specificConductivity_mS_P_cm", data.specificConductivity_mS_P_cm);
    send_message(*base_topic + "salinity_psu", data.salinity_psu);
    send_message(*base_topic + "totalDissolvedSolids_g_P_L", data.totalDissolvedSolids_g_P_L);
    send_message(*base_topic + "rawCoductivity_uS_P_cm", data.rawCoductivity_uS_P_cm);
    send_message(*base_topic + "pH_units", data.pH_units);
    send_message(*base_topic + "pH_mV", data.pH_mV);
    send_message(*base_topic + "referece_mV", data.referece_mV);
};

/**
 * @brief Send a message to the mqtt broker
 *
 * @param topic The topic to publish to
 * @param message The message to publish
 */
void NetworkClient::send_message(String topic, float message) {
    // publish data to mqtt broker
    mqtt_client.beginMessage(topic);
    mqtt_client.print(message);
    mqtt_client.endMessage();
};

/**
 * @brief Connect to the network and mqtt broker
 */
void NetworkClient::connect() {
    if (nb_access.status() != NB_READY || gprs.status() != GPRS_READY) {
        connectNB();
    }
    if (!mqtt_client.connected()) {
        connectMQTT();
    }
};

/**
 * @brief Connect to the network
 */
void NetworkClient::connectNB() {
    Serial.print("Connecting to Network");

    while ((nb_access.begin() != NB_READY) ||
        (gprs.attachGPRS() != GPRS_READY)) {
        // failed, retry
        Serial.print(".");
        delay(1000);
    }
    Serial.println("");
};

/**
 * @brief Connect to the mqtt broker
 */
void NetworkClient::connectMQTT() {
    Serial.print("Connecting to MQTT");
    while (!mqtt_client.connect(mqtt_broker, mqtt_port)) {
        // failed, retry
        Serial.print(".");
        Serial.println(mqtt_client.connectError());
        delay(5000);

        // double check if we are connected to the network
        if (nb_access.status() != NB_READY || gprs.status() != GPRS_READY) {
            connectNB();
        }
    }
    Serial.println("");
};