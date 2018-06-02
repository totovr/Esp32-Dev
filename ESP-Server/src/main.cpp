#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>

WiFiMulti WiFiMulti;
WiFiClient client;

// Functions
void server_raspberry();
void watch();

// Client variables
const uint16_t port = 5204;
const char * host = "10.31.1.100"; // ip or dns

// Server variables
WiFiServer server(80);
const char* ssid     = "Human-A1-721-2G_EXT";
const char* password = "bsys12bsys34";

void setup() {
    Serial.begin(115200);
    delay(10);
    // Client constructor
    WiFiMulti.addAP(ssid, password);
    // Server constructor
    WiFi.begin(ssid, password);
    // Connecting to the network
    Serial.print("Wait for WiFi... ");
    while(WiFiMulti.run() != WL_CONNECTED) {
            Serial.print(".");
            delay(500);
    }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    delay(500);
    // This is to start the Server
    server.begin();
}

void loop() {
    // listen for incoming clients
    WiFiClient client = server.available();
    // This function connect the watch
    watch();
    // This function connect to the Raspberry
    server_raspberry();
    delay(500);
}

void server_raspberry() {
  // Client connecting to raspberry
  Serial.print("connecting to ");
  Serial.println(host);
  if (!client.connect(host, port)) {
          Serial.println("connection failed");
          Serial.println("wait 5 sec...");
          delay(1000);
          return;
  }
  // This will send the request to the server
  client.print("ESP32 connected to server");
  delay(500);
}

void watch() {
  if (client) {
    // if client connected
    Serial.println("New Client.");
    // make a String to hold incoming data from the client
    String currentLine = "";
    //Serial.print('\n');
    while (client.connected()) {
      // loop while the client's connected
      if (client.available()) {
          // if there's bytes to read from the client
          char c = client.read();
          // read a byte
          Serial.println(c);
          // print it out the serial monitor
          Serial.print('\n');
        }
      }
    }
}
