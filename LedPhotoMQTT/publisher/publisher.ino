#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>

#include "Config.h"
#include "WIFI.h"
#include "MQTT.h"

#define PHOTO_SENSOR_PIN A0

size_t len_nums = 3;
const int interval = 800;

String mainURL = "http://192.168.101.15:8080/get_server_topic";
String response;

long int start_loop = 0;

void setup(void) {
  Serial.begin(9600);
  WIFI_init(false);
  MQTT_init();
  Serial.print("Status of mqtt client connection: ");
  Serial.println(mqtt_cli.connected());
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("Connected, IP address: ");
    Serial.println(WiFi.localIP());

    HTTPClient http;

    http.begin(wifiClient, mainURL);

    for (int attempt = 0; attempt < 5; attempt++) {  
      int httpCode = http.GET();
      if (httpCode == HTTP_CODE_OK) {
        Serial.print("HTTP response code: ");
        Serial.println(httpCode);

        response = http.getString();
        response = response.substring(1, response.length() - 1);
        Serial.println("Server topic: " + response);
        break;
      } else {
        Serial.print("HTTP request failed with code: ");
        Serial.println(httpCode);
      }
      delay(500);
    }

    http.end();
  } else {
    Serial.println("WiFi not connected. Check credentials and try again.");
  }
}

void loop(void) {
  if (start_loop == 0)
    start_loop = millis();

  if (millis() > start_loop + interval) {
    if(! mqtt_cli.connected()) {
      Serial.println("Я переподключился :(");
      MQTT_init();
    }
    Serial.print("Status of mqtt client connection: ");
    Serial.println(mqtt_cli.connected());
    int sensorValue = analogRead(PHOTO_SENSOR_PIN);
    String st = String(sensorValue / 4);

    while (st.length() < len_nums) 
      st = '0' + st;

    if(!mqtt_cli.publish(response.c_str(), st.c_str()))
      Serial.println("Сообщение не ушло");  

    Serial.print("MQTT topic: ");
    Serial.println(response.c_str());
    Serial.println("Sensor value: " + st);

    start_loop = millis();
  }
}
