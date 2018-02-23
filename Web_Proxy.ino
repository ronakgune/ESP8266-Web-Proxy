#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>

#define USE_SERIAL Serial

ESP8266WiFiMulti WiFiMulti;

const char* ssid = "";  //Enter WIFI name
const char* password = ""; // Enter Wifi Password

const char* apssid = ""; //Enter AP Name
const char* appassword = ""; // Enter AP password

ESP8266WebServer server(80);

void handleRoot() 
{
  server.send(200, "text/plain", "Enter the url");
}


void handleNotFound()
{ 
  const char* host;
  WiFiClient client;

//Get URI from browser
  Serial.print("Requesting URI");
  String requestUri = server.uri();
  Serial.println(requestUri);

  requestUri.remove(0,1);
  int i = requestUri.indexOf('/');
  String domain = requestUri.substring(0,i);
  host = domain.c_str();
  requestUri.remove(0,i);

  Serial.print("Host: ");
  Serial.println(host);
  Serial.println(requestUri);

//If connection is not successful
  while (!client.connect(host, 80)) 
  {
    Serial.println("connection failed, Trying again");
  }
  
  client.print(String("GET ") + requestUri);
  
  client.print(String(" HTTP/1.1\r\n") +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
               
while(!client.available())
  {
    yield();
  }


// Replace htttp and HTTP with WHAT-IS-THIS

  String line;
  while(client.available())
  {
    line = client.readStringUntil('\r');
    line.replace("HTTP", "WHAT-IS-THIS");
    line.replace("http", "WHAT-IS-THIS");
  }
  server.send(200, "text/html", line);

  client.stop();
}

void setup(void)
{
  Serial.begin(115200);  // Baud rate set to 115200
  
  for(uint8_t t = 10; t > 0; t--) 
  {
        USE_SERIAL.printf("[SETUP] LOADING WEBSITE %d...\n", t);
        USE_SERIAL.flush();
        delay(1000);
  }
  
  WiFi.softAP(apssid, appassword);

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS response");
  }

  server.on("/", handleRoot);

  server.on("/inline", []()
  {
    server.send(200, "text/plain", "It works");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("Starting HTTP Server");
  WiFiMulti.addAP(ssid, password);    
}

void loop(void)
{
  server.handleClient();
}