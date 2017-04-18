//Blynk Terminal V1
//Blynk Button   V2
//Blynk Button2 V10
//Blynk TempF    V5

#include <BlynkSimpleEsp8266.h>
#include "settings.h"

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>

#include <ArduinoOTA.h>

#include <DHT.h>

// #define BLYNK_PRINT Serial
// #define BLYNK_NO_BUILTIN

#define soil_pin             0 //analog
#define DHT_pin             14
#define SEC                  55 //sleep time(s)

float tempF = 0;

WidgetTerminal terminal(V1);
BlynkTimer timer;
DHT dht(DHT_pin, DHT11);

void goToSleep(){
    Serial.println("Going to sleep...");
    yield();
    ESP.deepSleep(SEC*1000000);// zero for external reset
}


void vWriteData(){
    //Read Data
    int soil = analogRead(soil_pin);
    float h = dht.readHumidity();
    float tf = dht.readTemperature(true);
 
    // DISPLAY DATA
    Serial.print("Temp: ");
    Serial.print(tf);
    Serial.print(",\t");
    Serial.print("Hum: ");
    Serial.print(h);
    Serial.print("Soil: ");
    Serial.println(soil);

    terminal.print("Temp: ");
    terminal.print(tf);
    terminal.print(", Hum: ");
    terminal.print(h);
    terminal.print(", Soil: ");
    terminal.println(soil);
    terminal.flush();

    Blynk.virtualWrite(V5, soil);
    Blynk.virtualWrite(V6, h);
    Blynk.virtualWrite(V7, tf); 

    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
    digitalWrite(LED_BUILTIN, HIGH);
}

void setup(){

    //Debug console
    Serial.begin(115200);
    Serial.println("Booting...");

    dht.begin();
    Blynk.begin(auth, ssid, passwd); 

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

    //OTA Setup
    ArduinoOTA.onStart([]() {
        Serial.println("Start");
    });
    ArduinoOTA.onEnd([]() {
        Serial.println("\nEnd");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
        else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
        else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
        else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

    ArduinoOTA.begin();

    if(!Blynk.connected()){
        Serial.println("Wifi failed to connect. Try changing ssid/pw settings.");
        terminal.println("Failed to connect. Change wifi settings in OTA mode.");
        terminal.flush();
    }
    terminal.println("Hello!");
    terminal.flush();

    terminal.print("IP address: ");
    terminal.println(WiFi.localIP());
    terminal.flush();

    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    vWriteData();
    timer.setInterval(5000, goToSleep);
}

void loop(){
    Blynk.run();
    timer.run();   
    ArduinoOTA.handle();
}
