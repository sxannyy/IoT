String ssidAP = "pomogi_pzh_s_domashkoy"; // имя контроллера и точки доступа
String passwordAP = "pomogi_really"; // пароль точки доступа

//char* ssidCLI = "YOUR_SSID"; // имя контроллера и точки доступа
//char* passwordCLI = "YOUR_PASS"; // пароль точки доступа

// char* ssidCLI = "Razdayu";
// char* passwordCLI = "12345678903";

char* mqtt_broker = "broker.emqx.io";

const int mqtt_port = 1883;
const int http_port = 80;

const int LED_PIN = 2;
const int SHORT_BLINK = 500;
const int LONG_BLINK = 2000;
const int DELAY = 2000;
const int EEPROM_NAME_SIZE = 30;
const int STATE_IDLE = 0;
const int STATE_LONG_BLINK = 1;
const int STATE_SHORT_DELAY = 2;
const int STATE_SHORT_BLINK = 3;
const int STATE_MQTT_BLINK = 4;

char* net_name = new char[EEPROM_NAME_SIZE];
char* net_pass = new char[EEPROM_NAME_SIZE];

int blinks = 0;
long int blink_start = 0;
int blink_interval = LONG_BLINK;
bool blink_delay = false;
bool led_state = LOW;
int state = STATE_IDLE;
bool hotspot_started = false;
const int time_connection_delay = 30000;

