#include <Arduino.h>
#include <PZEM004Tv30.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

void setup_wifi(const char* ssid, const char* pw, LiquidCrystal_I2C& lcd){
  delay(10);
  lcd.print("Connecting");
  Serial.print("Connecting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pw);
  int i=0;
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    if(i==3){
      lcd.clear();
      lcd.print("Connecting");
    }
    lcd.print(".");
    Serial.print(".");
    i++;
  }
  lcd.clear();
  lcd.print("Wifi Connected");
  Serial.print("Wifi Connected");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());
  Serial.print("Connected to : ");
  Serial.print(ssid);
  Serial.print(" IP ");
  Serial.print(WiFi.localIP());
  delay(2000);
}
void printSerialKelistrikan(PZEM004Tv30& pzem){
    float voltage = pzem.voltage();
    float current = pzem.current();
    float power = pzem.power();
    float energy = pzem.energy();
    float frequency = pzem.frequency();
    float pf = pzem.pf();

    if( !isnan(energy) ){
        Serial.print("Voltage: "); Serial.print(voltage); Serial.println("V");
    } else {
        Serial.println("Error reading voltage");
    }

    if( !isnan(current) ){
        Serial.print("Current: "); Serial.print(current); Serial.println("A");
    } else {
        Serial.println("Error reading current");
    }

    if( !isnan(power) ){
        Serial.print("Power: "); Serial.print(power); Serial.println("W");
    } else {
        Serial.println("Error reading power");
    }

    if( !isnan(energy) ){
        Serial.print("Energy: "); Serial.print(energy,3); Serial.println("kWh");
    } else {
        Serial.println("Error reading energy");
    }

    if( !isnan(frequency) ){
        Serial.print("Frequency: "); Serial.print(frequency, 1); Serial.println("Hz");
    } else {
        Serial.println("Error reading frequency");
    }

    if( !isnan(pf) ){
        Serial.print("PF: "); Serial.println(pf);
    } else {
        Serial.println("Error reading power factor");
    }

    Serial.println();
}
void mqttReconnect(PubSubClient& client, LiquidCrystal_I2C& lcd, char clientId[]) {
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("reconnecting");
    lcd.setCursor(0,1);
    lcd.print("to mqtt...");
    long r = random(1000);
    sprintf(clientId, "clientId-%ld", r);
    if (client.connect(clientId)) {
      Serial.print(clientId);
      Serial.println(" connected");
      client.subscribe("kwhMeter43");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}