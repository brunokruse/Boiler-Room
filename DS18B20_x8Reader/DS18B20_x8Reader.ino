/*
  Multiple DS18B20 Temperature Sensors
*/

#include <OneWire.h>
#include <DallasTemperature.h>

// one wire + dallas temperature
#define ONE_WIRE_BUS_PIN 4
OneWire oneWire(ONE_WIRE_BUS_PIN);
DallasTemperature sensors(&oneWire);

// sensor array
DeviceAddress temp1 = { 0x28, 0x34, 0xB5, 0x5F, 0x07, 0x00, 0x00, 0x8A }; // ok
DeviceAddress temp2 = { 0x28, 0x3C, 0x63, 0x60, 0x07, 0x00, 0x00, 0x3B }; // ok
DeviceAddress temp3 = { 0x28, 0x2E, 0x81, 0x5F, 0x07, 0x00, 0x00, 0x85 }; // ok
DeviceAddress temp4 = { 0x28, 0x05, 0xC2, 0x5F, 0x07, 0x00, 0x00, 0x6C }; // ok
DeviceAddress temp5 = { 0x28, 0x9E, 0x45, 0x5F, 0x07, 0x00, 0x00, 0xBF }; // ok
DeviceAddress temp6 = { 0x28, 0x49, 0x93, 0x5F, 0x07, 0x00, 0x00, 0x3C }; // ok
DeviceAddress temp7 = { 0x28, 0x4B, 0x18, 0x60, 0x07, 0x00, 0x00, 0x3B }; // ok
DeviceAddress temp8 = { 0x28, 0x1F, 0xED, 0x5F, 0x07, 0x00, 0x00, 0x6F }; // ok

// 0x28, 0x34, 0xB5, 0x5F, 0x07, 0x00, 0x00, 0x8A
// 0x28, 0x3C, 0x63, 0x60, 0x07, 0x00, 0x00, 0x3B
// 0x28, 0x2E, 0x81, 0x5F, 0x07, 0x00, 0x00, 0x85
// 0x28, 0x05, 0xC2, 0x5F, 0x07, 0x00, 0x00, 0x6C
// 0x28, 0x9E, 0x45, 0x5F, 0x07, 0x00, 0x00, 0xBF
// 0x28, 0x49, 0x93, 0x5F, 0x07, 0x00, 0x00, 0x3C
// 0x28, 0x4B, 0x18, 0x60, 0x07, 0x00, 0x00, 0x3B
// 0x28, 0x1F, 0xED, 0x5F, 0x07, 0x00, 0x00, 0x6F

void setup() {

  // start serial port to show results
  Serial.begin(9600);

  while (!Serial); // arduino yun hack

  Serial.print("Initializing Temperature Control Library Version ");
  Serial.println(DALLASTEMPLIBVERSION);

  // Initialize the Temperature measurement library
  sensors.begin();

  // set the resolution to 10 bit (Can be 9 to 12 bits .. lower is faster
  sensors.setResolution(temp1, 10);
  sensors.setResolution(temp2, 10);
  sensors.setResolution(temp3, 10);
  sensors.setResolution(temp4, 10);
  sensors.setResolution(temp5, 10);
  sensors.setResolution(temp6, 10);
  sensors.setResolution(temp7, 10);
  sensors.setResolution(temp8, 10);

  Serial.println("Sensors have been set.");

}

void loop() {

  delay(1000);

  Serial.println();
  Serial.print("Number of Devices found on bus = ");
  Serial.println(sensors.getDeviceCount());
  Serial.print("Getting temperatures... ");
  Serial.println();

  // Command all devices on bus to read temperature
  sensors.requestTemperatures();

  Serial.print("Probe 01 temperature is:   ");
  printTemperature(temp1);
  Serial.println();

  Serial.print("Probe 02 temperature is:   ");
  printTemperature(temp2);
  Serial.println();

  Serial.print("Probe 03 temperature is:   ");
  printTemperature(temp3);
  Serial.println();

  Serial.print("Probe 04 temperature is:   ");
  printTemperature(temp4);
  Serial.println();

  Serial.print("Probe 05 temperature is:   ");
  printTemperature(temp5);
  Serial.println();

  Serial.print("Probe 06 temperature is:   ");
  printTemperature(temp6);
  Serial.println();

  Serial.print("Probe 07 temperature is:   ");
  printTemperature(temp7);
  Serial.println();

  Serial.print("Probe 08 temperature is:   ");
  printTemperature(temp8);
  Serial.println();
}

void printTemperature(DeviceAddress deviceAddress) {

  float tempC = sensors.getTempC(deviceAddress);

  if (tempC == -127.00) {

    Serial.print("Error getting temperature  ");

  } else {

    Serial.print("C: ");
    Serial.print(tempC);
    Serial.print(" F: ");
    Serial.print(DallasTemperature::toFahrenheit(tempC));

  }
}
