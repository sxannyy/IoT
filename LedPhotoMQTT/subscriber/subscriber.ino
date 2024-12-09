
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#include "Config.h"
#include "WIFI.h"
#include "MQTT.h"

//#define LED_PIN 1

String mainURL = "http://192.168.101.15:8080/get_server_topic";
String response;
bool flag = true;

void setup(void){
  Serial.begin(9600);
  WIFI_init(false);
  MQTT_init();
  // Для отправки
  HTTPClient http;
   
  if (WiFi.status() == WL_CONNECTED) {  // Ensure WiFi is connected
    http.begin(wifiClient, mainURL);     // Initialize HTTP client with WiFi client

    while (true) {
      int httpCode = http.GET();         // Make the HTTP request
      if(httpCode == HTTP_CODE_OK) {     // Check for successful response
        Serial.print("HTTP response code ");
        Serial.println(httpCode);
        
        response = http.getString();     // Get response string from server
        response = response.substring(1, response.length() - 1);
        Serial.println("Server topic: " + response);
        break;
      } else {
        Serial.print("HTTP request failed with code: ");
        Serial.println(httpCode);        // Print error code if request failed
      }
      delay(500);  // Add delay to prevent request spamming if server is unreachable
    }
    
    http.end();  // Close HTTP client connection
  } else {
    Serial.println("WiFi not connected. Check credentials and try again.");
  }

  while (!mqtt_cli.connected()) {
    Serial.print("Attempting MQTT connection...");
    String client_id = "esp8266-" + String(WiFi.macAddress());
    if (mqtt_cli.connect(client_id.c_str())) {
      Serial.println("connected");
      mqtt_cli.subscribe(response.c_str());  // Подписываемся на тему
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqtt_cli.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

//void reconnect() {
//  // Попытка переподключения к MQTT
//  while (!mqtt_cli.connected()) {
//    Serial.print("Attempting MQTT connection...");
//    String client_id = "esp8266-" + String(WiFi.macAddress());
//    if (mqtt_cli.connect(client_id.c_str())) {
//      Serial.println("connected");
//      mqtt_cli.subscribe(response.c_str());  // Подписываемся на тему
//    } else {
//      Serial.print("failed, rc=");
//      Serial.print(mqtt_cli.state());
//      Serial.println(" try again in 5 seconds");
//      delay(5000);
//    }
//  }
//}

void loop(void){   
//  if (!mqtt_cli.connected()) {
//    MQTT_init();  // Переподключение к MQTT, если связь потеряна
//    mqtt_cli.subscribe(response.c_str());
//  }
//  if(!mqtt_cli.connected()) {
//      Serial.println("Я переподключился :(");
//      MQTT_init();
//      mqtt_cli.subscribe(response.c_str()); 
//    }
    mqtt_cli.loop();
//    Serial.print("Status of mqtt client connection: ");
//    Serial.println(mqtt_cli.connected());
//
//    Serial.print("MQTT topic: ");
//    Serial.println(response.c_str());
    // Обработка входящих сообщений MQTT
    
   
    
}
