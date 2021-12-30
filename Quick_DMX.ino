#include "settings.h"
#include <ESP8266WiFi.h>
#include <TinyXML.h>



//messages:
String msg_init = "HELLO|Live_Mobile|";
String msg_request_buttons = "BUTTON_LIST";
String msg_button_release = "BUTTON_RELEASE|";
String msg_button_press = "BUTTON_PRESS|";


bool debug = false;

//  settings:
uint16_t button_hold_time_ms = 300;

WiFiClient client;
void setup() {
  delay(1000);
  Serial.begin(9600);
  delay(100);


  //  initialize switch pins

  for (int i = 0; i < 3; i++) {
    pinMode(switch_pin[i], INPUT);
  }

  //  do not change
  if (debug) {
    Serial.print("WiFi: ");
    Serial.print(wifi_ssid);
    Serial.print(" As: ");
    Serial.print(wifi_hostname);
    Serial.println("");
    Serial.println("Connecting...");
  }
  WiFi.hostname(wifi_hostname);
  connect_wifi();
}

void connect_wifi() {
  WiFi.begin(wifi_ssid, wifi_password);
  int connect_iterations = 0;
  bool wifi_connected = true;
  while (WiFi.status() != WL_CONNECTED) {
    connect_iterations++;
    delay(500);
    if (debug) {
      Serial.print(".");
    }
    if (connect_iterations > 20) {
      wifi_connected = false;
      break;
    }
  }

  if (!wifi_connected) {
    WiFi.disconnect();
    if (debug) {
      Serial.println("Try again");
    }
    connect_wifi();
  } else {
    if (debug) {
      Serial.println("Connected");
    }
    delay(2000);
  }
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connect_wifi();
    return;
  }

  for (uint8_t i = 0; i < nr_switch_pins; i++) {
    if (digitalRead(switch_pin[i]) != switch_status[i]) {
      delay(button_hold_time_ms);
      if (digitalRead(switch_pin[i]) != switch_status[i]) {
        if (debug) {
          Serial.print("pin ");
          Serial.print(switch_pin[i]);
          Serial.println(" triggered");
        }
        switch_status[i] = digitalRead(switch_pin[i]);
        int random_number = rand() % nr_switch_buttons;
        toggle_QuickDMX(switch_QuickDMX_button[i][0]);
      }
    }
  }

}

void toggle_QuickDMX(String QuickDMX_button) {
  WiFiClient client;
  if (debug) {
    Serial.print("Connecting to QuickDMX host ");
    Serial.print(quickDMX_host);
    Serial.print(":");
    Serial.println(quickDMX_port);
  }

  if (!client.connect(quickDMX_host, quickDMX_port)) {
    if (debug) {
      Serial.println("connection failed");
    }
    return;
  }
  if (quickDMX_pass == "") {
    client_send_and_receive(client, msg_init);
  } else {
    client_send_and_receive(client, msg_init + quickDMX_pass);
  }

  client_receive(client);
  String button_list = client_send_and_receive(client, msg_request_buttons);
  int button_ID = get_button_value(button_list, QuickDMX_button, "index");
  if (button_ID >= 0) {
    int pressed = get_button_value(button_list, QuickDMX_button, "pressed");
    if (pressed >= 0) {
      delay(10);
      if (pressed > 0) {
        client_send_and_receive(client, msg_button_release + button_ID);
      } else {
        client_send_and_receive(client, msg_button_press + button_ID);
      }
    } else {
      if (debug) {
        Serial.println("Status not found");
      }
    }
  } else {
    if (debug) {
      Serial.println("Button not found");
    }
  }

  client.stop();
}

String client_receive(WiFiClient client) {
  String response_text = "";
  if (client.connected()) {
    if (debug) {
      Serial.print("Waiting for response");
    }
    uint8_t iteration = 0;
    bool response = false;
    while (!response) {
      iteration++;
      if (debug) {
        Serial.print(".");
      }
      while (client.available()) {
        response = true;
        if (debug) {
          Serial.println("Received a response");
        }
        yield();
        String line = client.readStringUntil('\n');
        response_text += line;
        if (debug) {
          //Serial.println("");
          //Serial.println("Response:");
          //Serial.println(line);
        }
      }
      delay(1);
      if (iteration > 50) {
        break;
      }
    }
  }
  return response_text;
}

String client_send_and_receive(WiFiClient client, String message) {
  String response_text = "";
  if (client.connected()) {
    if (debug) {
      Serial.print("Sending message: ");
      Serial.println(message);
    }
    client.println(message + "\r\n");
    return client_receive(client);

  }
  return response_text;
}

int get_button_value(String button_list, String button_name, String parameter) {
  int needle_location = button_list.indexOf(">" + button_name + "</button");
  if (needle_location > 0) {
    uint16_t button_begin = needle_location - 75;
    uint16_t button_end = needle_location;
    String button_substring = button_list.substring(button_begin, button_end);
    uint16_t index_begin = button_substring.indexOf(parameter);
    if (index_begin > 0) {
      index_begin += parameter.length() + 2;
      String index_substring = button_substring.substring(index_begin, index_begin + 4);
      uint16_t index_end = index_substring.indexOf('"') + index_begin;
      String ID = button_substring.substring(index_begin, index_end);
      return uint8_t(ID.toInt());
    } else {
      return -1;
    }
  } else {
    return -1;
  }
}
