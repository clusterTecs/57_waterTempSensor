#include <WiFiS3.h>
#include <WiFiClient.h>
#include <Arduino_JSON.h>
#include <Adafruit_GFX.h>
#include <Adafruit_RA8875.h>
#include <SPI.h>

// WiFi credentials
const char* ssid = "";
const char* password = "";

// HTTP server details
const char* host = "apicontainer.57concrete.net";
const int httpPort = 80;

// Sensor IDs to display
const int NUM_SENSORS = 4;
const char* sensorIDs[NUM_SENSORS] = {"01", "02", "03", "04"};

// RA8875 display pins
#define RA8875_CS 10
#define RA8875_RESET 9
Adafruit_RA8875 tft = Adafruit_RA8875(RA8875_CS, RA8875_RESET);

void connectWiFi() {
  Serial.print("Connecting to WiFi...");
  while (WiFi.begin(ssid, password) != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println(" connected!");
}

void displayMessage(String msg, uint16_t foreColor, uint16_t backColor, int y) {
  // Clear area first (x, y, width, height, background)
  tft.fillRect(30, y, 740, 40, backColor);  // Adjust width/height as needed

  tft.textMode();
  tft.textEnlarge(3);  // Bigger text!
  tft.textColor(foreColor, backColor);
  tft.textSetCursor(30, y);
  tft.print(msg);
}

void requestAndDisplaySensor(const char* id, int y) {
  String path = String("/v1/container/") + id;
  bool success = false;

  for (int attempt = 1; attempt <= 2 && !success; attempt++) {
    WiFiClient client;
    delay(100);

    Serial.println("Connecting to " + String(host) + path + " (attempt " + attempt + ")");

    if (!client.connect(host, httpPort)) {
      Serial.println("Connection failed!");
      delay(300);
      continue;
    }

    client.println("GET " + path + " HTTP/1.1");
    client.println("Host: " + String(host));
    client.println("Connection: close");
    client.println();

    unsigned long timeout = millis();
    while (client.connected() && !client.available()) {
      if (millis() - timeout > 5000) {
        Serial.println("Timeout waiting for server response");
        client.stop();
        break;
      }
      delay(10);
    }

    while (client.available()) {
      String line = client.readStringUntil('\n');
      if (line == "\r") break;
    }

    String response = "";
    while (client.available()) {
      response += client.readString();
    }
    client.stop();

    Serial.println("Response: " + response);

    JSONVar json = JSON.parse(response);
    if (JSON.typeof(json) == "object" && json.hasOwnProperty("temperature")) {
      double temp = (double)json["temperature"];
      String msg = "Container " + String(id) + ": " + String(temp, 2) + " F";
      Serial.println(msg);
      displayMessage(msg, RA8875_CYAN, RA8875_BLACK, y);
      delay(500);
      success = true;
    } else {
      Serial.println("JSON parse failed");
      delay(500);
    }
  }

  if (!success) {
    displayMessage("Container " + String(id) + ": FAIL", RA8875_RED, RA8875_BLACK, y);
  }
}

void setup() {
  Serial.begin(115200);
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

  displayMessage("Sensor Temps Mission", RA8875_YELLOW, RA8875_BLACK, 10);
}

void loop() {
  int y = 110;
  for (int i = 0; i < NUM_SENSORS; i++) {
    requestAndDisplaySensor(sensorIDs[i], y);
    y += 90;  // More spacing for bigger font
    delay(300);  // Slight pause between sensors
  }

  delay(15000);  // Refresh every 15 seconds
}
