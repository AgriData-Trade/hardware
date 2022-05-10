#include "data.h"

String Data::toString() {
    String dataString = "";
    dataString += "Temperature (Kelvin): " + String(temperature_kelvin) + "\n";
    dataString += "NO3- (mg/L-N): " + String(nitrate_mg_P_L) + "\n";
    dataString += "NO3- (mV): " + String(nitrate_mV) + "\n";
    dataString += "Specific Conductivity: " + String(specificConductivity_mS_P_cm) + "\n";
    dataString += "Salinity: " + String(salinity_psu) + "\n";
    dataString += "Total Dissolved Solids: " + String(totalDissolvedSolids_g_P_L) + "\n";
    dataString += "Raw Conductivity: " + String(rawConductivity_uS_P_cm) + "\n";
    dataString += "pH (units): " + String(pH_units) + "\n";
    dataString += "pH (mV): " + String(pH_mV) + "\n";
    dataString += "Reference (mV): " + String(referece_mV) + "\n";

    return dataString;
}

String Data::toJson() {
    String payload = "";
    payload += "{";
    payload += "\"temperature_kelvin\":" + String(temperature_kelvin);
    payload += ",\"nitrate_mg_P_L\":" + String(nitrate_mg_P_L);
    payload += ",\"nitrate_mV\":" + String(nitrate_mV);
    payload += ",\"specificConductivity_mS_P_cm\":" + String(specificConductivity_mS_P_cm);
    payload += ",\"salinity_psu\":" + String(salinity_psu);
    payload += ",\"totalDissolvedSolids_g_P_L\":" + String(totalDissolvedSolids_g_P_L);
    payload += ",\"rawConductivity_uS_P_cm\":" + String(rawConductivity_uS_P_cm);
    payload += ",\"pH_units\":" + String(pH_units);
    payload += ",\"pH_mV\":" + String(pH_mV);
    payload += ",\"referece_mV\":" + String(referece_mV);
    payload += "}";

    return payload;
}
