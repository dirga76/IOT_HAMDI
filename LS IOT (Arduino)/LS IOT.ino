#ifdef ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#elif defined(ESP32)
#include <WiFi.h>
#include <ESPmDNS.h>
#else
#error "Board not found"
#endif


//Library WifiManager
#include<WiFiManager.h>  

#include "EmonLib.h"             // Include Emon Library

//Library Websocket and ArduinoJson
#include <WebSocketsServer.h>
#include <Ticker.h>
#include <ArduinoJson.h>

//Library On The Air
#include <AsyncElegantOTA.h>

//Page HTML
#include "index.h"
#include "style.h"
#include "script.h"
#include "NotFound.h"



//Pin GPIO
#define VOLT_CAL 138.7
#define Kontak 5

// Replace with your network credentials
const char* ssid = "LS_IOT";
const char* password = "bismillah ";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
WebSocketsServer websockets(81);

void send_sensor();
EnergyMonitor emon1;             // Create an instance
Ticker timer;

//Page not found
void notFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/html", notfoundpage);
}

//Websocket Event Kontak
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {

  switch (type) 
  {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED: {
        IPAddress ip = websockets.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);

        // send message to client
        websockets.sendTXT(num, "Connected from server");
      }
      break;
    case WStype_TEXT:
      Serial.printf("[%u] get Text: %s\n", num, payload);
      String message = String((char*)( payload));
      Serial.println(message);

      
     DynamicJsonDocument doc(200);
    // deserialize the data
    DeserializationError error = deserializeJson(doc, message);
    // parse the parameters we expect to receive (TO-DO: error handling)
      // Test if parsing succeeds.
  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }

  //Check status contact with parsing JSON
  int emon1_status = doc["emon1"];
  int Kontak_status = doc["Kontak"];

  //Contact
  digitalWrite(emon1,emon1_status);
  digitalWrite(Kontak,Kontak_status);
  }
}

void setup()
{  
   // Initialize the output variables as outputs
  Serial.begin(9600);
  emon1.voltage(25, VOLT_CAL, 1.7);  // Voltage: input pin, calibration, phase_shift
  pinMode (Kontak, OUTPUT);
  Serial.begin(115200);
 //WifiManager  
  WiFiManager wifiManager;  

  //Access Point  
  wifiManager.autoConnect("ConnectHavo","password");  
  Serial.println("Connected.....");  
    
  if (MDNS.begin("havo")) { //havo.local/
    Serial.println("MDNS responder started");
  }
  
  //Dashboard Page
  server.on("/", [](AsyncWebServerRequest * request)
  { 

  request->send_P(200, "text/html", webpage);
  });

  server.onNotFound(notFound);
  AsyncElegantOTA.begin(&server); // Start ElegantOTA
  server.begin();  // it will start webserver
  websockets.begin();
  websockets.onEvent(webSocketEvent);
  timer.attach(2,send_sensor);

emon1.calcVI(25,1000);         // Calculate all. No.of half wavelengths (crossings), time-out
  
  float supplyVoltage   = emon1.Vrms;             //extract Vrms into Variable
  if(supplyVoltage >= 100)
  {
    Serial.print("Power ON: ");  
    Serial.println(supplyVoltage);
    digitalWrite(Kontak, HIGH);
  }
  else
 {
   Serial.println("Power OFF: ");
   Serial.println(supplyVoltage);
   digitalWrite(Kontak, LOW);   
  }
}


void loop()
{
  // Websocket
 websockets.loop();
}

//Sensor Realtime
void send_sensor()
{
   // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  String title = "Hello word";
  int counter = 0;
  
  for(int i = 0; i <= 100; i++){
    counter = i;
    // JSON_Data = {"title":title,"counter":counter}
    String JSON_Data = "{\"title\":";
           JSON_Data += "\"";
           JSON_Data += title;
           JSON_Data += "\"";
           JSON_Data += ",\"counter\":";
           JSON_Data += counter;
           JSON_Data += "}";
    Serial.println(JSON_Data);     
    websockets.broadcastTXT(JSON_Data);
    delay(1000);
  }
}
