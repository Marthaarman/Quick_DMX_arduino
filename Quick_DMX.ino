#include "settings.h"
#include <ESP8266WiFi.h>
#include <TinyXML.h>

//settings:
String message0 = "HELLO|Live_Mobile|\r\n";

String message1 = "BUTTON_LIST\r\n";
String message2_1 = "BUTTON_RELEASE|";
String message2_2 = "\r\n";

String message3_1 = "BUTTON_PRESS|";
String message3_2 = "\r\n";



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



}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.disconnect();
    WiFi.reconnect();
    Serial.println("Connecting...");
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("\nWiFi connected!");
    return;
  }

  WiFiClient client;
  if (!client.connect(quickDMX_host, quickDMX_port)) {
    Serial.println("connection failed");
    return;
  }

  client_send_and_receive(client, message0);
  client_receive(client);

  String button_list = client_send_and_receive(client, message1);
  int button_ID = get_button_value(button_list, button_text, "ID");
  if (button_ID >= 0) {
    int pressed = get_button_value(button_list, button_text, "pressed");
    if (pressed >= 0) {
      delay(10);
      if (pressed > 0) {
        client_send_and_receive(client, message2_1 + button_ID + message2_2);
      } else {
        client_send_and_receive(client, message3_1 + button_ID + message3_2);
      }
    }else {
      Serial.println("Status not found");
    }
  } else {
    Serial.println("Button not found");
  }
  delay(4000);
  client.stop();
}

String client_receive(WiFiClient client) {
  String response_text = "";
  if (client.connected()) {
    Serial.print("Waiting for response");
    bool response = false;
    while (!response) {
      Serial.print(".");
      while (client.available()) {
        Serial.println("");
        Serial.println("Response:");
        response = true;
        String line = client.readStringUntil('\n');
        response_text += line;
        Serial.println(line);
      }
      delay(1);
    }
  }
  return response_text;
}

String client_send_and_receive(WiFiClient client, String message) {
  String response_text = "";
  if (client.connected()) {
    Serial.print("Sending message: ");
    client.println(message);
    Serial.print("Waiting for response");
    bool response = false;

    while (!response) {
      Serial.print(".");
      while (client.available()) {
        Serial.println("");
        Serial.println("Response:");
        response = true;
        String line = client.readStringUntil('\n');
        response_text += line;
        Serial.println(line);
      }
    }
  }
  return response_text;
}

int get_button_value(String button_list, String button_name, String parameter) {
  int needle_location = button_list.indexOf(">"+button_name+"</button");
  if (needle_location > 0) {
    uint8_t button_begin = needle_location - 75;
    uint8_t button_end = needle_location;
    String button_substring = button_list.substring(button_begin, button_end);

    uint8_t index_begin = button_substring.indexOf(parameter) + parameter.length() + 2;
    String index_substring = button_substring.substring(index_begin, index_begin + 4);
    uint8_t index_end = index_substring.indexOf('"') + index_begin;
    String ID = button_substring.substring(index_begin, index_end);
    return uint8_t(ID.toInt());
  } else {
    return -1;
  }
}
