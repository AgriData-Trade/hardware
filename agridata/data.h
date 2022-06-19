#include <Arduino.h>

#ifndef Data_h
#define Data_h

struct Data {
    String temperature_kelvin;
    String nitrate_mg_P_L;
    String nitrate_mV;
    String specificConductivity_mS_P_cm;
    String salinity_psu;
    String totalDissolvedSolids_g_P_L;
    String rawConductivity_uS_P_cm;
    String pH_units;
    String pH_mV;
    String referece_mV;

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
