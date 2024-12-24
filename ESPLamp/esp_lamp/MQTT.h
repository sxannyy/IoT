#include <PubSubClient.h>
#include <cstring>
#include "Arduino.h"
// #define LED_PIN 1
extern int state;
// mainURL = "http://192.168.101.15:8080/get_server_topic";
PubSubClient mqtt_cli(wifiClient);

void callback(char *topic, byte *payload, unsigned int length) {
  String message = "";
  state = STATE_MQTT_BLINK;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println("Message: " + message);
  if (message == "led_off") {
    digitalWrite(LED_PIN, HIGH);
  } 
  else if (message == "led__on"){
    digitalWrite(LED_PIN, LOW);
  }

}


void MQTT_init(){
  mqtt_cli.setServer(mqtt_broker, mqtt_port);
  while (!mqtt_cli.connected()) {
      String client_id = "esp8266-" + String(WiFi.macAddress());
      Serial.print("The client " + client_id);
      Serial.println(" connects to the public mqtt broker\n");
      if (mqtt_cli.connect(client_id.c_str())){
          Serial.println("MQTT Connected");
      } else {
          Serial.print("failed with state ");
          Serial.println(mqtt_cli.state());
          delay(2000);
      }
  }  
  mqtt_cli.setCallback(callback);
}
