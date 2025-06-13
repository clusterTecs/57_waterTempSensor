#include <SoftwareSerial.h>
#include <WiFiS3.h>
#include <ArduinoHttpClient.h>

// WiFi credentials
const char* ssid = "Fonseca";
const char* password = "FonElz5516";

// --- Test server (Beeceptor) ---
//const char* testServer = "tempcluster.free.beeceptor.com";
//const int testPort = 443;
//const char* testPath = "/";

// --- Production server (commented for now) ---
const char* prodServer = "apicontainer.57concrete.net";
const int prodPort = 443;
const char* prodPath = "/v1/container";

// Choose server to use
//const char* server = testServer;
//const int port = testPort;
//const char* postPath = testPath;

const char* server = prodServer;
const int port = prodPort;
const char* postPath = prodPath;


// SoftwareSerial for XBee (RX, TX)
SoftwareSerial xbeeSerial(2, 3);

WiFiSSLClient wifiClient;
HttpClient httpClient(wifiClient, server, port);

void setup() {
  Serial.begin(115200);
  xbeeSerial.begin(9600);
  delay(1000);
  Serial.println("Receiver Starting...");

  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
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
  if (sepIndex == -1) {
    Serial.println("Invalid data format");
    return;
  }

  String sensorID = payload.substring(2, sepIndex);   // "01" from "ID01"
  String tempValue = payload.substring(sepIndex + 1); // temperature

  String json = "{\"temperature\":" + tempValue + ",\"number\":\"" + sensorID + "\"}";

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

  Serial.print("HTTP Status: ");
  Serial.println(statusCode);
  Serial.print("Response: ");
  Serial.println(response);

  httpClient.stop();
}

