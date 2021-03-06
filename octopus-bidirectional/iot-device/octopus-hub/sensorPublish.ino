/*
 * Bosch SI Example Code License Version 1.0, January 2016
 *
 * Copyright 2017 Bosch Software Innovations GmbH ("Bosch SI"). All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
 * following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following
 * disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
 * following disclaimer in the documentation and/or other materials provided with the distribution.
 *
 * BOSCH SI PROVIDES THE PROGRAM "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE ENTIRE RISK AS TO THE
 * QUALITY AND PERFORMANCE OF THE PROGRAM IS WITH YOU. SHOULD THE PROGRAM PROVE DEFECTIVE, YOU ASSUME THE COST OF ALL
 * NECESSARY SERVICING, REPAIR OR CORRECTION. THIS SHALL NOT APPLY TO MATERIAL DEFECTS AND DEFECTS OF TITLE WHICH BOSCH
 * SI HAS FRAUDULENTLY CONCEALED. APART FROM THE CASES STIPULATED ABOVE, BOSCH SI SHALL BE LIABLE WITHOUT LIMITATION FOR
 * INTENT OR GROSS NEGLIGENCE, FOR INJURIES TO LIFE, BODY OR HEALTH AND ACCORDING TO THE PROVISIONS OF THE GERMAN
 * PRODUCT LIABILITY ACT (PRODUKTHAFTUNGSGESETZ). THE SCOPE OF A GUARANTEE GRANTED BY BOSCH SI SHALL REMAIN UNAFFECTED
 * BY LIMITATIONS OF LIABILITY. IN ALL OTHER CASES, LIABILITY OF BOSCH SI IS EXCLUDED. THESE LIMITATIONS OF LIABILITY
 * ALSO APPLY IN REGARD TO THE FAULT OF VICARIOUS AGENTS OF BOSCH SI AND THE PERSONAL LIABILITY OF BOSCH SI'S EMPLOYEES,
 * REPRESENTATIVES AND ORGANS.
 */

float humidityMin = 1E+20;
float humidityMax = 1E-20;
float tempMin = 1E+20;
float tempMax = 1E-20;
float barometerMin = 1E+20;
float barometerMax = 1E-20;
float gasMin = 1E+20;
float gasMax = 1E-20;
float powerMin = 1E+20;
float powerMax = 1E-20;
float tempBnoMin = 1E+20;
float tempBnoMax = 1E-20;
float altitudeMin = 1E+20;
float altitudeMax = 1E-20;

String modifyFeaturePropertiesMsg(String featureName, const String& properties) {
  StaticJsonBuffer<600> jsonBuffer;
  JsonObject& dittoProtocolMsg = jsonBuffer.createObject();
  dittoProtocolMsg["topic"] = String(THINGS_NAMESPACE) + "/" + String(THING_NAME) + "/things/twin/commands/modify";
  JsonObject& headers = dittoProtocolMsg.createNestedObject("headers");
  headers["response-required"] = false;
  headers["content-type"] = "application/vnd.eclipse.ditto+json";
  dittoProtocolMsg["path"] = "/features/" + featureName + "/properties";
  dittoProtocolMsg["value"] = jsonBuffer.parseObject(properties);

  String output;
  dittoProtocolMsg.printTo(output);
  jsonBuffer.clear();
  return output;
}

String sensorMinMaxValue(float sensorValue, float minValue, float maxValue, const String& units) {
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& properties = jsonBuffer.createObject();
  JsonObject& statusProps = properties.createNestedObject("status");
  statusProps["sensorValue"] = sensorValue;
  statusProps["minMeasuredValue"] = minValue;
  statusProps["maxMeasuredValue"] = maxValue;
  statusProps["sensorUnits"] = units;
  
  String output;
  properties.printTo(output);
  jsonBuffer.clear();
  return output;
}

String sensor3dValue(float xValue, float yValue, float zValue, const String& units) {
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& properties = jsonBuffer.createObject();
  JsonObject& statusProps = properties.createNestedObject("status");
  statusProps["xValue"] = xValue;
  statusProps["yValue"] = yValue;
  statusProps["zValue"] = zValue;
  statusProps["sensorUnits"] = units;
  
  String output;
  properties.printTo(output);
  jsonBuffer.clear();
  return output;
}

void publishSensorData(float power, const Bme680Values& bme680Values, const Bno055Values& bno055Values) {

  updateMinMax(power, bme680Values, bno055Values);

  hub.publish(
    modifyFeaturePropertiesMsg("voltage", sensorMinMaxValue(power, powerMin, powerMax, "V")));
  hub.publish(
    modifyFeaturePropertiesMsg("humidity", sensorMinMaxValue(bme680Values.humidity, humidityMin, humidityMax, "%")));
  hub.publish(
    modifyFeaturePropertiesMsg("temperature", sensorMinMaxValue(bme680Values.temperature, tempMin, tempMax, "°C")));
  hub.publish(
    modifyFeaturePropertiesMsg("pressure", sensorMinMaxValue(bme680Values.pressure / 100.0, barometerMin, barometerMax, "hPa")));
  hub.publish(
    modifyFeaturePropertiesMsg("gas_resistance", sensorMinMaxValue(bme680Values.gas_resistance / 1000.0, gasMin, gasMax, "KOhms")));
  hub.publish(
    modifyFeaturePropertiesMsg("altitude", sensorMinMaxValue(bme680Values.altitude, altitudeMin, altitudeMax, "m")));
  hub.publish(
    modifyFeaturePropertiesMsg("ambient_temperature", sensorMinMaxValue(bno055Values.temperature, tempBnoMin, tempBnoMax, "°C")));

  hub.publish(
    modifyFeaturePropertiesMsg("acceleration", sensor3dValue(bno055Values.accelerationX, bno055Values.accelerationY, bno055Values.accelerationZ, "m/s^2")));
  hub.publish(
    modifyFeaturePropertiesMsg("orientation", sensor3dValue(bno055Values.orientationX, bno055Values.orientationY, bno055Values.orientationZ, "°")));
  hub.publish(
    modifyFeaturePropertiesMsg("gravity", sensor3dValue(bno055Values.gravityX, bno055Values.gravityY, bno055Values.gravityZ, "m/s^2")));
  hub.publish(
    modifyFeaturePropertiesMsg("angular_velocity", sensor3dValue(bno055Values.angularVelocityX, bno055Values.angularVelocityY, bno055Values.angularVelocityZ, "rad/s")));
  hub.publish(
    modifyFeaturePropertiesMsg("linear_acceleration", sensor3dValue(bno055Values.LinearAccelerationX, bno055Values.LinearAccelerationY, bno055Values.LinearAccelerationZ, "m/s^2")));
  hub.publish(
    modifyFeaturePropertiesMsg("magnetometer", sensor3dValue(bno055Values.magneticFieldStrengthX, bno055Values.magneticFieldStrengthY, bno055Values.magneticFieldStrengthZ, "uT")));
}

void updateMinMax(float power, const Bme680Values& bme680Values, const Bno055Values& bno055Values) {
  if (powerMin > power) {
    powerMin = power;
  }
  if (powerMax < power) {
    powerMax = power;
  }

  float humidity = bme680Values.humidity;
  if (humidityMin > humidity) {
    humidityMin = humidity;
  }
  if (humidityMax < humidity) {
    humidityMax = humidity;
  }

  float temp = bme680Values.temperature;
  if (tempMin > temp) {
    tempMin = temp;
  }
  if (tempMax < temp) {
    tempMax = temp;
  }

  float barometer = bme680Values.pressure / 100.0;
  if (barometerMin > barometer) {
    barometerMin = barometer;
  }
  if (barometerMax < barometer) {
    barometerMax = barometer;
  }

  float tempBno = bno055Values.temperature;
  if (tempBnoMin > tempBno) {
    tempBnoMin = tempBno;
  }
  if (tempBnoMax < tempBno) {
    tempBnoMax = tempBno;
  }

  float altitude = bme680Values.altitude;
  if (altitudeMin > altitude) {
    altitudeMin = altitude;
  }
  if (humidityMax < altitude) {
    altitudeMax = altitude;
  }

  float gas = bme680Values.gas_resistance / 1000.0;
  if (gasMin > gas) {
    gasMin = gas;
  }
  if (gasMax < gas) {
    gasMax = gas;
  }
}
