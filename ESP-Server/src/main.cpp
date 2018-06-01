#include <Arduino.h>
#include "WiFi.h"
#include <WiFiMulti.h>

WiFiMulti WiFiMulti;
WiFiClient client;

const uint16_t port = 5204;
const char * host = "10.31.1.100"; // ip or dns

//Server variable
WiFiServer server(80);

void setup() {
    Serial.begin(115200);
    delay(10);
    // We start by connecting to a WiFi network
    WiFiMulti.addAP("Human-A1-721-2G_EXT", "bsys12bsys34");
    Serial.println();
    Serial.println();
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
}


void loop() {
    Serial.print("connecting to ");
    Serial.println(host);
    if (!client.connect(host, port)) {
            Serial.println("connection failed");
            Serial.println("wait 5 sec...");
            delay(5000);
            return;
    }
    // This will send the request to the server
    client.print("ESP32 connected to server");
    //read back one line from server
    String line = client.readStringUntil('\r');
    client.println(line);
    delay(2000);
}
