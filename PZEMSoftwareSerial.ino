#include <PZEM004Tv30.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "Network.h"

/* Use software serial for the PZEM
 * Pin 12 D6 Rx (Connects to the Tx pin on the PZEM) //TX
 * Pin 13 D7 Tx (Connects to the Rx pin on the PZEM) //RX
*/
#define relay 2

PZEM004Tv30 pzem(12, 13);
LiquidCrystal_I2C lcd(0x27,16,2);
WiFiClient espClient;
PubSubClient client(espClient);

const char *ssid = "Hinet";
const char *pw = "123123123";
const char* mqttServer = "broker.hivemq.com";
int port = 1883;
char clientId[50];
unsigned long waktuSebelumnya = 0;
boolean relayAktif = false;
float energiTotal = 0.0; 

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.clear();
  setup_wifi(ssid, pw, lcd);
  lcd.setCursor(0, 0);
  pinMode(relay, OUTPUT);
  digitalWrite(relay, HIGH);
  client.setServer(mqttServer, port);
  client.setCallback(callbackMqtt);
}

void loop() {
    float voltage = pzem.voltage();
    float current = pzem.current();
    float power = pzem.power();
    float energy = pzem.energy();
    float frequency = pzem.frequency();
    float pf = pzem.pf();

    if (digitalRead(relay) == LOW) {
      relayAktif = true;
      
    } else {
      relayAktif = false;
    }
    
    if (digitalRead(relay) == LOW) {
      relayAktif = true;
    } else {
      relayAktif = false;
    }
    unsigned long waktuSekarang = millis();
    float waktuDelta = (waktuSekarang - waktuSebelumnya) / 1000.0;
    if (relayAktif) {
      energiTotal += ((isnan(power)? 0 : power) * waktuDelta) / 3600000.0;  // Konversi daya dari Watt ke kWh
      Serial.print("custom kwh : ");
      Serial.println(energiTotal,3);
      float harga = isnan(2466 * energiTotal) ? 0 : (2466 * energiTotal);
      lcd.clear();
      lcd.print(power);
      lcd.print("W ");
      lcd.print(energiTotal);
      lcd.print("kWh");
      lcd.setCursor(0, 1);
      lcd.print("Rp ");
      lcd.print(harga);

      if (!client.connected()) {
        mqttReconnect(client, lcd, clientId);
      }else{
        // String payloadMsg = String(voltage, 2) + "V\n";
        // payloadMsg += String(current, 2) + "A\n";
        // payloadMsg += String(power, 2) + "W\n";
        // payloadMsg += String(energy, 3) + "kWh\n";
        // payloadMsg += String(frequency, 2) + "Hz\n";
        // payloadMsg += String(pf, 2) + "PF\n";
        String payloadMsg = "[" + String(waktuDelta) + "," + String(energiTotal, 3) + "," + String(harga, 3) + "]";
        client.publish("kwhMeter42", payloadMsg.c_str());
      }
      client.loop();
      printSerialKelistrikan(pzem);
    }else{
      lcd.clear();
      lcd.print("Ready To Use");
      energiTotal = 0;
      if (!client.connected()) {
        mqttReconnect(client, lcd, clientId);
      }else{
        // String payloadMsg = String(voltage, 2) + "V\n";
        // payloadMsg += String(current, 2) + "A\n";
        // payloadMsg += String(power, 2) + "W\n";
        // payloadMsg += String(energy, 3) + "kWh\n";
        // payloadMsg += String(frequency, 2) + "Hz\n";
        // payloadMsg += String(pf, 2) + "PF\n";
        String payloadMsg = "Not Used";
        client.publish("kwhMeter42", payloadMsg.c_str());
      }
      client.loop();
    }
    waktuSebelumnya = waktuSekarang;
    delay(2000);
}

void callbackMqtt(char* topic, byte* message, unsigned int length) {
  String stMessage;
  for (int i = 0; i < length; i++) {
    stMessage += (char)message[i];
  }
  if (String(topic) == "kwhMeter43") {
    Serial.print("Changing output to ");
    if(stMessage == "on"){
      Serial.println("on");
      digitalWrite(relay, LOW);
    }
    else if(stMessage == "off"){
      Serial.println("off");
      digitalWrite(relay, HIGH);
    }
  }
}
