#include "Config.h"
#include "WIFI.h"
#include "MQTT.h"
#include "Server.h"
#include <ESP8266HTTPClient.h>

unsigned long int time_connection;
bool is_connected = false;
String topic = "lab/esp8266/laba_lamp";

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  EEPROM.begin(EEPROM_NAME_SIZE * 2);

  EEPROM.get(0, net_name);
  EEPROM.get(EEPROM_NAME_SIZE, net_pass);
  if (net_name[0] == '\0' or net_pass[0] == '\0') {
    WIFI_init(true);
    Serial.println("ESP in hotspot mode");
    hotspot_started = true;
    server_init();
    blink_interval = LONG_BLINK;
    state = STATE_LONG_BLINK;
    blink_delay = false;
  } 
  else {
    WIFI_init(false);
    
    if(checkHTTPConnection())
      Serial.println("Нет подключения к интернету");
    else
      is_connected = true;

    hotspot_started = false;
    state = STATE_SHORT_DELAY;
    blink_interval = SHORT_BLINK;
    blink_delay = true;
    time_connection = millis();
    if (is_connected) {
      time_connection = millis();
      MQTT_init();
      state = STATE_SHORT_BLINK;
      blink_delay = false;
      mqtt_cli.subscribe(topic.c_str());
    }
  }
}

void loop() {
  if (state == STATE_LONG_BLINK or state == STATE_SHORT_DELAY){
    // Serial.println("i'm here1");
    server.handleClient();
    blink();
  }
  else if (state == STATE_SHORT_BLINK or state == STATE_MQTT_BLINK){
    if(!mqtt_cli.connected()) {
     Serial.println("Я переподключился :(");
     MQTT_init();
     mqtt_cli.subscribe(topic.c_str()); 
   }
    mqtt_cli.loop();
    // Serial.println("i'm here2");
    if (state == STATE_SHORT_BLINK)
      blink();
    
  }
  
  if (!is_connected){
    // Serial.println("i'm here3");
    if (state == STATE_LONG_BLINK){
      return;
    }
    if (WiFi.status() != WL_CONNECTED){
      if (!hotspot_started){
        WIFI_init(true);
        hotspot_started = true;
        server_init();
        blink_interval = LONG_BLINK;
        state = STATE_LONG_BLINK;
        blink_delay = false;
      }
    }
    else{
      blink_interval = SHORT_BLINK;
      state = STATE_SHORT_DELAY;
      blink_delay = true;
    }
  }
  else if (is_connected){
    // Serial.println("i'm here4");
    if (state == STATE_SHORT_DELAY){
      MQTT_init();
      blink_interval = SHORT_BLINK;
      state = STATE_SHORT_BLINK;
      blink_delay = false;
      mqtt_cli.subscribe(topic.c_str());
    }
  }
  if (state != STATE_LONG_BLINK and millis() > time_connection + time_connection_delay){
    if(!checkHTTPConnection())
      Serial.println("Нет подключения к интернету");
    else
      is_connected = true;
    
    time_connection = millis();
    Serial.println(state);
  }
}

void blink(){
  if (millis() >= blink_start + blink_interval and millis() < blink_start + 2 * blink_interval){
    if (led_state == LOW){
      digitalWrite(LED_PIN, LOW);
      led_state = HIGH;
    }
  }
  if (millis() > blink_start + 2 * blink_interval){
    if (led_state == HIGH){
      digitalWrite(LED_PIN, HIGH);
      led_state = LOW;
    }
    if (blink_delay){
      blinks++;
      blink_start = millis();
      if (blinks == 2){
        blinks = 0;
        blink_start = millis() + DELAY;
      }
    }
    else{
      blink_start = millis();
    }
  }
}

bool checkHTTPConnection() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("wifi не подключен.");
    return false;
  }

  HTTPClient http;
  const char* test_url = "http://yandex.ru";
  http.begin(wifiClient, test_url); 

  int httpCode = http.GET();

  if (httpCode > 0) {
    Serial.print("HTTP код ответа: ");
    Serial.println(httpCode);

    if (httpCode == HTTP_CODE_OK or httpCode == 301 or httpCode == 302) {
      http.end();
      return true;
    }
  } else {
    Serial.print("Ошибка соединения: ");
    Serial.println(http.errorToString(httpCode).c_str());
  }

  http.end();
  return false;
}