#include <SoftwareSerial.h>
#include <OneWire.h>
#include <WiFiS3.h>
#include <ArduinoHttpClient.h>
#include <WiFiClient.h>
#include <HttpClient.h>

// Receiver <-> Display connection
SoftwareSerial displaySerial(4, 5); // TX, RX — change as needed

// Wi-Fi credentials
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";


// Beeceptor settings
const char* server = "tempsensor-test.free.beeceptor.com";
const int port = 443;
String endpoint = "/";

// Server settings
const char* serverAddress = "yourserver.com"; // e.g., "example.com"
int port = 443; // HTTPS
String endpoint = "/your-endpoint-path"; // e.g., "/api/data"


WiFiSSLClient wifiClient;
HttpClient client = HttpClient(wifiClient, serverAddress, port);


// RX, TX for XBee
SoftwareSerial xbeeSerial(2, 3); 

void setup() {
  Serial.begin(9600);
  xbeeSerial.begin(9600);
  displaySerial.begin(9600);  // Connects to display Arduino
  Serial.println("Receiver Ready");

  // Connect to Wi-Fi
  Serial.print("Connecting to WiFi");
  while (WiFi.begin(ssid, password) != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi!");

}

void loop() {
  if (xbeeSerial.available()) {
    String incomingData = xbeeSerial.readStringUntil('\n');
    incomingData.trim();

    Serial.println("Received from XBee: " + incomingData);

    // Send to HTTP server
    sendToServer(incomingData);

    // Send raw data to display Arduino
    displaySerial.print(sensorID);
    displaySerial.print(":");
    displaySerial.println(valueStr); 
  }
}

void sendToServer(String payload) {
  String contentType = "application/json";
  String json = "{\"data\":\"" + payload + "\"}";

  Serial.println("Sending to server: " + json);
  
  client.post(postPath, contentType, json);

  int statusCode = client.responseStatusCode();
  String response = client.responseBody();

  Serial.print("Status Code: ");
  Serial.println(statusCode);
  Serial.print("Response: ");
  Serial.println(response);

  client.stop();
}



