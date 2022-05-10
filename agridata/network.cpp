#include "network.h"

/**
 * @brief Send data payload to the mqtt broker
 *
 * @param data
 */
void NetworkClient::send_data(Data data, String base_topic) {
    // Make sure we are connected to the network and mqtt broker
    connect();

    // Publish data
    send_message(base_topic + "measurement", data.toJson());
};

/**
 * @brief Send a message to the mqtt broker
 *
 * @param topic The topic to publish to
 * @param message The message to publish
 */
void NetworkClient::send_message(String topic, String message) {
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
        delay(5000);

        // double check if we are connected to the network
        if (nb_access.status() != NB_READY || gprs.status() != GPRS_READY) {
            connectNB();
        }
    }
    Serial.println("");
};
