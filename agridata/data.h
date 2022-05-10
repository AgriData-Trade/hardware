#include <Arduino.h>

#ifndef Data_h
#define Data_h

struct Data {
    float temperature_kelvin;
    float nitrate_mg_P_L;
    float nitrate_mV;
    float specificConductivity_mS_P_cm;
    float salinity_psu;
    float totalDissolvedSolids_g_P_L;
    float rawConductivity_uS_P_cm;
    float pH_units;
    float pH_mV;
    float referece_mV;

    /**
     * @brief Displays the data in a human readable format.
     *
     * @return String
     */
    String toString();

    /**
     * @brief Turn the data into a json object
     *
     * @return String
     */
    String toJson();
};

# endif
