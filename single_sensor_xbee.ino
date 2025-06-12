#include <SoftwareSerial.h>

SoftwareSerial xbeeSerial(2, 3); // RX, TX for XBee
int sensorPin = A0;              // Replace with actual sensor pin

const char* sensorID = "ID1";    // Change to "ID2" or "ID3" for other senders

void setup() {
  Serial.begin(9600);
  xbeeSerial.begin(9600);
}

void loop() {
  int raw = analogRead(sensorPin);
  float voltage = raw * (5.0 / 1023.0);
  float temperature = voltage * 100.0; // Assuming TMP36 or similar sensor

  // Format: ID1:25.6
  xbeeSerial.print(sensorID);
  xbeeSerial.print(":");
  xbeeSerial.println(temperature, 2);

  Serial.print("Sent: ");
  Serial.print(sensorID);
  Serial.print(":");
  Serial.println(temperature, 2);

  delay(2000); // Send every 2 seconds
}
