#ifndef NETWORK_H
#define NETWORK_H

void setup_wifi(const char* ssid, const char* pw, LiquidCrystal_I2C& lcd);
void mqttReconnect(PubSubClient& client, LiquidCrystal_I2C& lcd, char clientId[]);
void printSerialKelistrikan(PZEM004Tv30& pzem);

#endif