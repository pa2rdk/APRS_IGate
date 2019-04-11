#ifdef __IN_ECLIPSE__
//This is a automatic generated file
//Please do not modify this file
//If you touch this file your change will be overwritten during the next build
//This file has been generated on 2019-04-11 20:49:08

#include "Arduino.h"
#include <Arduino.h>
#include <SPI.h>
#include <WiFi.h>
#include <Wire.h>
#include "SSD1306.h"
#include "config.h"

void setup() ;
void loop() ;
bool convertPacket(int bufLen,int bufPos) ;
boolean check_connection() ;
void receive_data() ;
void send_packet() ;
void display_packet() ;
void display(char *msg) ;
void InitConnection() ;
void WlanReset() ;
int WlanStatus() ;
void setDra(byte rxFreq, byte txFreq, byte rxTone, byte txTone) ;

#include "APRS_IGate.ino"


#endif
