#include "wifi_settings.h"
#include <WebSocketsClient.h>
#include <ESP8266WiFi.h>

const char* quickDMX_host = "192.168.1.123";
uint16_t quickDMX_port = 7351;

const char* message0 = "HELLO|Live_Mobile_3|";

WebSocketsClient webSocket;

bool clientConnected = false;

bool allow_sending = false;
bool registered = false;
bool sent = false;
bool accepted = false;
String myRandWebSocket = String(rand() * 10000 + 10000);
WiFiClient client;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.print("WiFi: ");
  Serial.print(wifi_ssid);
  Serial.print(" As: ");
  Serial.print(wifi_hostname);
  Serial.println("");
  Serial.println("Connecting...");

  WiFi.hostname(wifi_hostname);
  WiFi.begin(wifi_ssid, wifi_password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");

  Serial.println("connected to server");
  // Make a HTTP request:
  if (client.connect("192.168.1.123", 7351)) {
    client.println("GET / HTTP/1.1");
    client.println("Host: 192.168.1.123");
    client.println("Upgrade: websocket");
    client.println("Connection: Upgrade");
    client.println("Sec-WebSocket-Key: " + String(myRandWebSocket) + String(myRandWebSocket));
    client.println("Sec-WebSocket-Version: 13");
    client.println();
  }
  webSocket.begin(quickDMX_host, quickDMX_port);
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(3000);
  webSocket.enableHeartbeat(15000, 3000, 2);


}

void loop() {
  if(!accepted) {
    while (client.available()) {
      String line = client.readStringUntil('\n');
      if(line.substring(0,21) == "Sec-WebSocket-Accept:") {
        accepted = true;
      }
    }
    return;
  }
  Serial.println("send msg");
  webSocket.sendTXT(message0);
  webSocket.loop();
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.println("[WSc] Disconnected!");
      allow_sending = false;
      registered = false;
      break;
    case WStype_CONNECTED:
      Serial.printf("[WSc] Connected to url: %s\n", payload);
      allow_sending = true;
      break;
    case WStype_TEXT:
      Serial.printf("[WSc] get text: %s\n", payload);
      break;
  }

}
