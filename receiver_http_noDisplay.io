#include <SoftwareSerial.h>
#include <WiFiS3.h>
#include <ArduinoHttpClient.h>

// Wi-Fi credentials
const char* ssid = "Fonseca";
const char* password = "FonElz5516";

// --- Beeceptor TEST setup ---
const char* server = "tempcluster.free.beeceptor.com"; // Beeceptor subdomain
const int port = 443;                                  // HTTPS
const char* postPath = "/";                            // Beeceptor root path

// --- PRODUCTION server ---
// const char* server = "apicontainer.57concrete.net";   // Your live API domain
// const int port = 443;                                 // HTTPS
// const char* postPath = "/v1/container";               // Your API endpoint path

// SoftwareSerial for XBee (RX, TX)
SoftwareSerial xbeeSerial(2, 3);  // Change if needed for your XBee shield

WiFiSSLClient wifiClient;         // Use WiFiClient for HTTP (port 80)
HttpClient httpClient = HttpClient(wifiClient, server, port);

void setup() {
  Serial.begin(115200);
  xbeeSerial.begin(9600);

  Serial.println("Receiver Starting...");
  Serial.print("Connecting to WiFi");

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected to WiFi!");
  Serial.println("Receiver Ready");
}

void loop() {
  if (xbeeSerial.available()) {
    String incoming = xbeeSerial.readStringUntil('\n');
    incoming.trim();

    Serial.println("Received: " + incoming);
    sendToServer(incoming);
  }
}

void sendToServer(const String& payload) {
  int sepIndex = payload.indexOf(':');
  if (sepIndex == -1 || sepIndex < 3) {
    Serial.println("Invalid format");
    return;
  }

  String fullID = payload.substring(0, sepIndex);       // e.g. "ID01"
  String shortID = fullID.substring(2);                 // e.g. "01"
  String temperatureStr = payload.substring(sepIndex + 1); // e.g. "70.3"

  // Construct JSON
  String json = "{\"temperature\":" + temperatureStr + ",\"ID\":\"" + shortID + "\"}";
  Serial.println("Sending JSON: " + json);

  httpClient.beginRequest();
  httpClient.post(postPath);
  httpClient.sendHeader("Content-Type", "application/json");
  httpClient.sendHeader("Content-Length", json.length());
  httpClient.beginBody();
  httpClient.print(json);
  httpClient.endRequest();

  int statusCode = httpClient.responseStatusCode();
  String response = httpClient.responseBody();

  Serial.print("Status code: ");
  Serial.println(statusCode);
  Serial.print("Response: ");
  Serial.println(response);

  httpClient.stop();
}
