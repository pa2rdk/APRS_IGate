#ifdef __IN_ECLIPSE__
//This is a automatic generated file
//Please do not modify this file
//If you touch this file your change will be overwritten during the next build
//This file has been generated on 2019-04-17 20:38:40

#include "Arduino.h"
#include <Arduino.h>
#include <EEPROM.h>
#include <SPI.h>
#include <WiFi.h>
#include <Wire.h>
#include "SSD1306.h"
#include <TinyLoRa.h>

void IRAM_ATTR resetModule() ;
void setup() ;
void loop() ;
bool convertPacket(int bufLen,int bufPos) ;
boolean check_connection() ;
void receive_data() ;
void send_packet() ;
void send_LoRaPacket() ;
void display_packet() ;
void display(char *msg) ;
void InitConnection() ;
void updateGatewayonAPRS();
void WlanReset() ;
int WlanStatus() ;
void setDra(byte rxFreq, byte txFreq, byte rxTone, byte txTone) ;
void setSettings(bool doSet) ;
void getStringValue(int length) ;
byte getCharValue() ;
uint32_t get32NumericValue() ;
uint16_t get16NumericValue() ;
byte getNumericValue() ;
void saveConfig() ;
void loadConfig() ;
void printConfig() ;
void SerialFlush() ;

#include "APRS_IGate.ino"


#endif
