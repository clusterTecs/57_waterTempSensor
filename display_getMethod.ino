#include <SoftwareSerial.h>
#include <WiFiS3.h>
#include <ArduinoHttpClient.h>

// Wi-Fi credentials
const char* ssid = "Fonseca";
const char* password = "FonElz5516";

// Server configuration
const char* server = "apicontainer.57concrete.net";
const int port = 443; // HTTPS
const String pathBase = "/v1/container?number=";

// Display connection (to RA8875 Arduino or serial screen)
SoftwareSerial displaySerial(4, 5); // TX, RX

WiFiSSLClient wifiClient; // Use WiFiClient for HTTP
HttpClient httpClient = HttpClient(wifiClient, server, port);

void setup() {
  Serial.begin(115200);
  displaySerial.begin(9600);

  Serial.println("Display Arduino Starting...");
  displaySerial.println("Initializing...");

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi!");
  displaySerial.println("Connected to WiFi!");
}

void loop() {
  for (int i = 1; i <= 5; i++) {
    String sensorNum = (i < 10) ? "0" + String(i) : String(i); // 01, 02...
    String fullPath = pathBase + sensorNum;

    httpClient.get(fullPath);
    int statusCode = httpClient.responseStatusCode();
    String response = httpClient.responseBody();

    Serial.print("Sensor "); Serial.print(sensorNum);
    Serial.print(" - Status: "); Serial.println(statusCode);
    Serial.println("Response: " + response);

    if (statusCode == 200 && response.length() > 0) {
      // Basic JSON parsing
      float temperature = extractTemperature(response);
      displaySerial.print("ID"); displaySerial.print(sensorNum);
      displaySerial.print(": ");
      displaySerial.print(temperature, 2);
      displaySerial.println(" F");
    } else {
      displaySerial.print("ID"); displaySerial.print(sensorNum);
      displaySerial.println(": Error");
    }

    delay(2000); // Small delay between sensors
  }

  delay(8000); // Wait before refreshing again
}

// Naive JSON parser to extract "temperature" value
float extractTemperature(String json) {
  int index = json.indexOf("\"temperature\":");
  if (index == -1) return -1;

  int start = index + 14;
  int end = json.indexOf(",", start);
  if (end == -1) end = json.indexOf("}", start);
  if (end == -1) return -1;

  String tempStr = json.substring(start, end);
  return tempStr.toFloat();
}
