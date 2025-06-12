#include <SPI.h>
#include "Adafruit_GFX.h"
#include "Adafruit_RA8875.h"
#include <SoftwareSerial.h>

// RA8875 pins
#define RA8875_CS   10
#define RA8875_RST  9
Adafruit_RA8875 tft = Adafruit_RA8875(RA8875_CS, RA8875_RST);

// Communication from receiver Arduino
SoftwareSerial inputSerial(2, 3); // RX, TX

// Sensor data storage
String sensorData[5] = {
  "ID1-S1: --.- °F",
  "ID2-S1: --.- °F",
  "ID3-S1: --.- °F",
  "ID4-S1: --.- °F",
  "ID4-S2: --.- °F"
};

void setup() {
  Serial.begin(9600);
  inputSerial.begin(9600);

  if (!tft.begin(RA8875_800x480)) {
    Serial.println("RA8875 not found");
    while (1);
  }

  tft.displayOn(true);
  tft.GPIOX(true);    
  tft.PWM1config(true, RA8875_PWM_CLK_DIV1024);
  tft.PWM1out(255);

  tft.fillScreen(RA8875_BLACK);
  tft.textMode();
  tft.textEnlarge(1);

  drawSensorList();
}

void loop() {
  if (inputSerial.available()) {
    String msg = inputSerial.readStringUntil('\n');
    msg.trim();  // Example: "ID4-S2:77.52"

    // Match and store data
    if (msg.startsWith("ID1-S1:")) sensorData[0] = msg + " °F";
    else if (msg.startsWith("ID2-S1:")) sensorData[1] = msg + " °F";
    else if (msg.startsWith("ID3-S1:")) sensorData[2] = msg + " °F";
    else if (msg.startsWith("ID4-S1:")) sensorData[3] = msg + " °F";
    else if (msg.startsWith("ID4-S2:")) sensorData[4] = msg + " °F";

    drawSensorList();
  }
}

void drawSensorList() {
  tft.fillRect(0, 0, 800, 250, RA8875_BLACK);  // Clear area

  tft.textColor(RA8875_WHITE, RA8875_BLACK);

  for (int i = 0; i < 5; i++) {
    tft.textSetCursor(10, 10 + i * 40);
    tft.print("Sensor ");
    tft.print(i + 1);
    tft.print(" → ");
    tft.print(sensorData[i]);
  }
}

