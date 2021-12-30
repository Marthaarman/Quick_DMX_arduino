const char* wifi_ssid = "wifi_ssid";
const char* wifi_password = "wifi_pass";
const char* wifi_hostname = "device_one";

const char* quickDMX_host = "192.168.x.x";
uint16_t quickDMX_port = 7348;
const char* quickDMX_pass = "";


//  buttons: 
const uint8_t nr_switch_pins = 3;                                       //set to the amount of pins used
const uint8_t nr_switch_buttons = 2;
uint8_t switch_pin[nr_switch_pins] = {D5, D6, D7};                      //which are they connected to
uint8_t switch_status[] = {false, false, false};                        //default status, false off, true means act right away
String switch_QuickDMX_button[nr_switch_pins][nr_switch_buttons] = {{"button1_0","button1_1"}, {"button2_0","button2_1"}, {"button3_0","button3_1"}};       //names of the buttons they trigger in QuickDMX
