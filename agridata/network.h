#ifndef Network_h
#define Network_h

#include <Arduino.h>
#include <MKRNB.h>
#include <ArduinoMqttClient.h>
#include "data.h"


/**
 * @brief The Network class
 *
 * This class is used to connect to the network and send data to the mqtt broker.
 */
class NetworkClient {
public:
    NetworkClient(const char* sim_pin_code, const char* mqtt_broker, int mqtt_port, const char* base_topic) : sim_pin_code(sim_pin_code), mqtt_broker(mqtt_broker), mqtt_port(mqtt_port), base_topic(base_topic), network_client(NBClient()), gprs(GPRS()), nb_access(NB()), mqtt_client(MqttClient(network_client)) {};

    void send_data(Data data, String base_topic);
    void connect();
private:
    void connectNB();
    void connectMQTT();
    void send_message(String topic, String message);

    const char* sim_pin_code;
    const char* mqtt_broker;
    int mqtt_port;
    const  char* base_topic;

    NBClient network_client;
    GPRS gprs;
    NB nb_access;
    MqttClient mqtt_client;
};

# endif
