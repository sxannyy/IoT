#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WiFiMulti.h>

ESP8266WiFiMulti wifiMulti;    
WiFiClient wifiClient;

String ip = "(IP unset)"; 

String id(){
  uint8_t mac[WL_MAC_ADDR_LENGTH]; // WL_MAC_ADDR_LENGTH - константа из библиотеки
  WiFi.softAPmacAddress(mac);
  String macID = String(mac[WL_MAC_ADDR_LENGTH - 2], HEX) + String(mac[WL_MAC_ADDR_LENGTH - 1], HEX);
  macID.toUpperCase();
  return macID;
}

bool StartAPMode() {
  IPAddress apIP(192, 168, 4, 1);
  WiFi.disconnect();
  WiFi.mode(WIFI_AP); // WIFI_AP - константа из библиотеки, указывающая режим работы контроллера (тут - точка доступа)
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP((ssidAP + " " + id()).c_str(), passwordAP.c_str()); // Переменные из конфига
  Serial.println("");
  Serial.println("WiFi up in AP mode with name: "+ ssidAP + " " + id());
  return true;
}

void StartCLIMode() {
  wifiMulti.addAP(ssidCLI, passwordCLI); // тут вариант с клиентским режимом: указываем куда подключаемся, пароль и ждём пока подключится
  //it is possible to add more networks to connect
  while(wifiMulti.run() != WL_CONNECTED) {
       
  }
  // return true;
}

void WIFI_init(bool mode_ap) {
  String unique_id = "";
    if (mode_ap) {
      StartAPMode();
      ip = WiFi.softAPIP().toString();
    } else {
      StartCLIMode();
      ip = WiFi.localIP().toString(); // Роутер даёт IP-адрес, заранее мы его не знаем
    }
    Serial.println("IP address: ");
    Serial.println(ip);
}
