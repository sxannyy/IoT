#include <ESP8266WebServer.h>
#include <EEPROM.h>

ESP8266WebServer server(80);    

void handleRoot() {                         
  server.send(200, 
              "text/html", 
              "<form action=\"/send\" method=\"POST\">Network name: <input type=\"text\" name=\"net_name\"><br>Password: <input type=\"password\" name=\"net_pass\"><br><input type=\"submit\" value=\"send\"></form>");
}

void handleSend(){
  net_name = strdup(server.arg("net_name").c_str());
  net_pass = strdup(server.arg("net_pass").c_str());
  // Serial.println("net_name: " + String(net_name));
  // Serial.println("net_pass: " + String(net_pass));
  EEPROM.put(0, net_name);
  EEPROM.put(EEPROM_NAME_SIZE, net_pass);
  EEPROM.commit();

  WIFI_init(false);
  hotspot_started = false;
  blink_interval = SHORT_BLINK;
  state = STATE_SHORT_DELAY;
  blink_delay = true;
  Serial.println("ESP in client mode");
}

void handleNotFound(){
  server.send(404, "text/plain", "404: Not found"); 
}

void server_init() {
  server.on("/", HTTP_GET, handleRoot);     
  server.on("/send", HTTP_POST, handleSend);
  server.onNotFound(handleNotFound);        

  server.begin();                          
  Serial.println("HTTP server started");    
}
