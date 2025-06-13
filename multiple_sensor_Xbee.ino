#include <OneWire.h>
#include <DallasTemperature.h>
#include <SoftwareSerial.h>

// Define sensor pins
#define ONE_WIRE_BUS_1 13
#define ONE_WIRE_BUS_2 12
#define ONE_WIRE_BUS_3 11
#define ONE_WIRE_BUS_4 10

// XBee communication (TX=2, RX=3)
SoftwareSerial xbeeSerial(2, 3);

// Create OneWire instances
OneWire oneWire1(ONE_WIRE_BUS_1);
OneWire oneWire2(ONE_WIRE_BUS_2);
OneWire oneWire3(ONE_WIRE_BUS_3);
OneWire oneWire4(ONE_WIRE_BUS_4);

// Create DallasTemperature instances
DallasTemperature sensor1(&oneWire1);
DallasTemperature sensor2(&oneWire2);
DallasTemperature sensor3(&oneWire3);
DallasTemperature sensor4(&oneWire4);

void setup() {
  Serial.begin(115200);
  xbeeSerial.begin(9600);

  sensor1.begin();
  sensor2.begin();
  sensor3.begin();
  sensor4.begin();

  Serial.println("Sender with 4 sensors started");
}

void loop() {
  float tempC1 = readTemperature(sensor1);
  float tempC2 = readTemperature(sensor2);
  float tempC3 = readTemperature(sensor3);
  float tempC4 = readTemperature(sensor4);

  float tempF1 = cToF(tempC1);
  float tempF2 = cToF(tempC2);
  float tempF3 = cToF(tempC3);
  float tempF4 = cToF(tempC4);

  // Send each sensor's data as a separate line
  sendTemperature("ID03", tempF1);
  sendTemperature("ID04", tempF2);
  sendTemperature("ID05", tempF3);
  sendTemperature("ID06", tempF4);

  delay(5000); // Wait 5 seconds between each cycle
}

float readTemperature(DallasTemperature& sensor) {
  sensor.requestTemperatures();
  return sensor.getTempCByIndex(0);
}

float cToF(float celsius) {
  return celsius * 9.0 / 5.0 + 32.0;
}

void sendTemperature(const String& sensorID, float tempF) {
  String message = sensorID + ":" + String(tempF, 2);
  xbeeSerial.println(message);
  Serial.println("Sent: " + message);
}

