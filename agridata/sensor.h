#ifndef Sensor_h
#define Sensor_h

#include <Arduino.h>
#include "data.h"
#include <SDI12.h>


/**
 * @brief The Sensor class
 *
 * The Sensor object.
 */
class Sensor {
public:
    /**
    * @brief Construct a new Sensor object
    *
    * @param data_pin
    */
    Sensor(const int data_pin);

    /**
     * @brief Start the sensor and establish communication
     *
     */
    void start();

    /**
     * @brief Get the Data object
     *
     * @return Data
     */
    Data getData();

    /**
     * @brief Checks if the sensor is still active
     *
     * @return boolean
     */
    boolean active();
private:
    /**
    * @brief gets identification information from a sensor, and prints it to the serial
    * port
    *
    * @param i a character between which is the id '0'-'9', 'a'-'z', or 'A'-'Z'.
    */
    void printInfo();

    /**
     * @brief Commands the sensor to take a measurement
     *
     */
    void takeMeasurement();

    /**
     * @brief Gets the value from the measurement at that id
     *
     * @param id the id for the measurement
     * @return double the value of the measurement
     */
    double readSensor(String id);

    SDI12 mySDI12;
};

# endif
