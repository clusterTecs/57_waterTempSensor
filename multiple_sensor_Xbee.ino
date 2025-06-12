//Multi water sensor temperature reader and sending using Xbee

#include <SoftwareSerial.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// XBee communication
SoftwareSerial xbeeSerial(2, 3); // RX, TX

// Define pins for each DS18B20 sensor
#define SENSOR_PIN_1 13
#define SENSOR_PIN_2 12
#define SENSOR_PIN_3 11
#define SENSOR_PIN_4 10

// OneWire instances for each sensor
OneWire oneWire1(SENSOR_PIN_1);
OneWire oneWire2(SENSOR_PIN_2);
OneWire oneWire3(SENSOR_PIN_3);
OneWire oneWire4(SENSOR_PIN_4);

// DallasTemperature objects for each sensor
DallasTemperature sensor1(&oneWire1);
DallasTemperature sensor2(&oneWire2);
DallasTemperature sensor3(&oneWire3);
DallasTemperature sensor4(&oneWire4);

// Sensor IDs
const char* sensorIDs[4] = {"ID4-S1", "ID4-S2", "ID4-S3", "ID4-S4"};

void setup() {
  Serial.begin(9600);
  xbeeSerial.begin(9600);

  sensor1.begin();
  sensor2.begin();
  sensor3.begin();
  sensor4.begin();
}

void loop() {
  // Request temperature readings
  sensor1.requestTemperatures();
  sensor2.requestTemperatures();
  sensor3.requestTemperatures();
  sensor4.requestTemperatures();

  // Read and send each one
  sendTemperature(sensor1, sensorIDs[0]);
  sendTemperature(sensor2, sensorIDs[1]);
  sendTemperature(sensor3, sensorIDs[2]);
  sendTemperature(sensor4, sensorIDs[3]);

  delay(2000); // Wait before next cycle
}

void sendTemperature(DallasTemperature& sensor, const char* id) {
  float celsius = sensor.getTempCByIndex(0);
  float fahrenheit = (celsius * 9.0 / 5.0) + 32.0;

  // Send via XBee
  xbeeSerial.print(id);
  xbeeSerial.print(":");
  xbeeSerial.println(fahrenheit, 2);

  // Debug
  Serial.print("Sent → ");
  Serial.print(id);
  Serial.print(":");
  Serial.print(fahrenheit, 2);
  Serial.println(" °F");

  delay(500); // Small delay between messages
}
