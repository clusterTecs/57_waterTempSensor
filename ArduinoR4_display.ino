#include <WiFiS3.h>
#include <WiFiSSLClient.h>
#include <Arduino_JSON.h>
#include <Adafruit_GFX.h>
#include <Adafruit_RA8875.h>
#include <SPI.h>

// WiFi credentials
const char* ssid = "Fonseca";
const char* password = "FonElz5516";

// HTTPS endpoint
const char* host = "apicontainer.57concrete.net";
const int httpsPort = 443;
const char* url = "/v1/container";

WiFiSSLClient client;

// RA8875 display pins
#define RA8875_CS 10
#define RA8875_RESET 9
Adafruit_RA8875 tft = Adafruit_RA8875(RA8875_CS, RA8875_RESET);

// Max number of different sensors expected
const int MAX_SENSORS = 10;

// Arrays to store unique sensors
String sensorNumbers[MAX_SENSORS];
double sensorTemps[MAX_SENSORS];
String updatedAts[MAX_SENSORS];
int sensorCount = 0;

void connectWiFi() {
  Serial.print("Connecting to WiFi...");
  while (WiFi.begin(ssid, password) != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println(" connected!");
}

void displayMessage(String msg, uint16_t foreColor, uint16_t backColor, int y) {
  tft.textMode();
  tft.textEnlarge(3); // Scale text 2x
  tft.textColor(foreColor, backColor);
  tft.textSetCursor(20, y);
  tft.print(msg);
}

void requestAndDisplayData() {
  Serial.print("Connecting to ");
  Serial.println(host);

  if (!client.connect(host, httpsPort)) {
    Serial.println("Connection failed!");
    return;
  }

  // HTTPS GET request
  client.println(String("GET ") + url + " HTTP/1.1");
  client.println(String("Host: ") + host);
  client.println("Connection: close");
  client.println();

  // Wait for response
  while (client.connected() && !client.available()) delay(10);

  String response = "";
  bool bodyStarted = false;
  while (client.available()) {
    String line = client.readStringUntil('\n');
    if (!bodyStarted && line == "\r") {
      bodyStarted = true;
    } else if (bodyStarted) {
      response += line;
    }
  }
  client.stop();

  Serial.println("Server Response:");
  Serial.println(response);

  JSONVar json = JSON.parse(response);

  if (JSON.typeof(json) != "array") {
    Serial.println("Failed to parse JSON");
    return;
  }

  sensorCount = 0;

  for (int i = 0; i < json.length(); i++) {
    String number = (const char*)json[i]["number"];
    double temp = (double)json[i]["temperature"];
    String updatedAt = (const char*)json[i]["updatedAt"];

    bool found = false;
    for (int j = 0; j < sensorCount; j++) {
      if (sensorNumbers[j] == number) {
        if (updatedAt > updatedAts[j]) {
          sensorTemps[j] = temp;
          updatedAts[j] = updatedAt;
        }
        found = true;
        break;
      }
    }

    if (!found && sensorCount < MAX_SENSORS) {
      sensorNumbers[sensorCount] = number;
      sensorTemps[sensorCount] = temp;
      updatedAts[sensorCount] = updatedAt;
      sensorCount++;
    }
  }

  tft.fillScreen(RA8875_BLACK);
  displayMessage("Sensor Temps", RA8875_YELLOW, RA8875_BLACK, 10);

  int y = 70;
  for (int i = 0; i < sensorCount; i++) {
    String msg = "Sensor " + sensorNumbers[i] + ": " + String(sensorTemps[i], 2) + " F";
    Serial.println(msg);
    displayMessage(msg, RA8875_CYAN, RA8875_BLACK, y);
    y += 60; // Increased spacing for double-size text
  }
}

void setup() {
  Serial.begin(9600);
  delay(1000);

  connectWiFi();

  if (!tft.begin(RA8875_800x480)) {
    Serial.println("RA8875 Not Found!");
    while (1);
  }

  tft.displayOn(true);
  tft.GPIOX(true);
  tft.PWM1config(true, RA8875_PWM_CLK_DIV1024);
  tft.PWM1out(255);
  tft.fillScreen(RA8875_BLACK);

  requestAndDisplayData();
}

void loop() {
  requestAndDisplayData();
  delay(10000); // Refresh every 10 seconds
}

