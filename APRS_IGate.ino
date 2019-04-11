#include <Arduino.h>
#include <SPI.h>
#include <WiFi.h>
#include <Wire.h>
#include "SSD1306.h"

// Change callsign, network, and all other configuration in the config.h file
#include "config.h"

#define BUFFERSIZE 260
#define Modem_RX 22
#define Modem_TX 23
#define OLED_SCL	15			// GPIO15
#define OLED_SDA	4			// GPIO4
#define OLED_RST	16
#define TX_LED 27
#define OLED_ADR	0x3C		// Default 0x3C for 0.9", for 1.3" it is 0x78

char recvBuf[BUFFERSIZE+1];
char buf[BUFFERSIZE+1];
int buflen = 0;
bool DEBUG_PRINT = 1;
uint32_t oledSleepTime = 0;

WiFiClient client;
HardwareSerial Modem(1);

SSD1306  lcd(OLED_ADR, OLED_SDA, OLED_SCL);// i2c ADDR & SDA, SCL on wemos

void setup() {
	pinMode(OLED_RST,OUTPUT);
	digitalWrite(OLED_RST, LOW); // low to reset OLED
	pinMode(TX_LED,OUTPUT);
	digitalWrite(TX_LED, LOW); // low to reset OLED
	delay(50);
	digitalWrite(OLED_RST, HIGH); // must be high to turn on OLED
	delay(50);
	Wire.begin(4,15);

	lcd.init();
	lcd.flipScreenVertically();
	lcd.setFont(ArialMT_Plain_10);
	lcd.setTextAlignment(TEXT_ALIGN_LEFT);
	lcd.drawString(0, 0, "APRS IGate");
	lcd.drawString(0, 8, "STARTING");
	lcd.display();
	lcd.setFont(ArialMT_Plain_10);
	delay(1000);

	Serial.begin(9600);
	Serial.println("AIRGATE - PA2RDK");
	Modem.begin(9600, SERIAL_8N1, Modem_RX, Modem_TX);
	Modem.setTimeout(2);
	delay(1000);
	setDra(MODEMCHANNEL, MODEMCHANNEL, 0, 0);
	Modem.println(F("AT+DMOSETVOLUME=8"));
	Modem.println(F("AT+DMOSETMIC=8,0"));
	Modem.println(F("AT+SETFILTER=1,1,1"));
	Serial.print(F("DRA Initialized on freq:"));
	Serial.println(float(144000+(MODEMCHANNEL*12.5))/1000);
	lcd.drawString(0, 16, "Modem set at "+String(float(144000+(MODEMCHANNEL*12.5))/1000)+ "MHz" );
	lcd.display();
	delay(1000);
}

void loop() {
	if (millis()-oledSleepTime>OLEDTIMEOUT*1000){
		oledSleepTime=millis();
		lcd.clear();
		lcd.display();
	}
	boolean connected = check_connection();
	byte doSwap = 1;
	int bufpos = 0;
	while (Modem.available()) {
		char ch = Modem.read();

		if (ch == 0xc0 && buflen>4){
			ch='\n';
		}
		if (ch==0x03){
			doSwap = 0;
			bufpos=buflen;
		}

		if (ch == '\n') {
			recvBuf[buflen] = 0;
			if (convertPacket(buflen,bufpos)){
				if (connected){
					digitalWrite(TX_LED, HIGH);
					send_packet();
					delay(100);
					digitalWrite(TX_LED, LOW);
					oledSleepTime=millis();
				}
			} else {
				Serial.println("Illegal packet");
				lcd.clear();
				lcd.drawString(0, 0, "Illegal packet");
				lcd.drawStringMaxWidth(0,16, 200, buf);
				lcd.display();
				oledSleepTime=millis();
			}
			buflen = 0;
		} else if (ch == 0xc0) {
			// Skip chars
		} else if ((ch > 31 || ch == 0x1c || ch == 0x1d || ch == 0x27 || ch == 0x1e) && buflen < BUFFERSIZE) {
			// Mic-E uses some non-printing characters
			if (doSwap==1) ch=ch>>1;
			recvBuf[buflen++] = ch;
		}
	}

	//	If connected to APRS-IS, read any response from APRS-IS and display it.
	//	Buffer 80 characters at a time in case printing a character at a time is slow.
	if (connected) {
		receive_data();
	}
}

bool convertPacket(int bufLen,int bufPos) {
	bool retVal = 0;
	if (bufPos>0){
		memcpy(buf,&recvBuf[8],6);
		int newPos=0;
		for (int i=7;i<13;i++){
			if (recvBuf[i]!=' ') buf[newPos++]=recvBuf[i];
		}
		buf[newPos++]='-';
		buf[newPos++]=recvBuf[13];
		buf[newPos++]='>';

		for (int i=0;i<6;i++){
			if (recvBuf[i]!=' ') buf[newPos++]=recvBuf[i];
		}
		buf[newPos++]=':';
		for (int i=bufPos+1;i<bufLen;i++){
			buf[newPos++]=recvBuf[i];
		}
		buf[newPos++]=0;
		retVal = 1;
	} else {
		strcpy (buf,recvBuf);
	}
	return retVal;
}

// See http://www.aprs-is.net/Connecting.aspx
boolean check_connection() {
	if (WiFi.status() !=WL_CONNECTED || !client.connected()) {
		InitConnection();
	}
	return client.connected();
	oledSleepTime=millis();
}

void receive_data() {
	if (client.available()) {
		char rbuf[81];
		int i = 0;
		while (i < 80 && client.available()) {
			char c = client.read();
			rbuf[i++] = c;
			if (c == '\n') break;
		}
		rbuf[i] = 0;
		display(rbuf);
	}
}

void send_packet() {
	display("Sending packet");
	display(buf);
	lcd.clear();
	lcd.drawString(0, 0, "Send packet");
	lcd.drawStringMaxWidth(0,16, 200, buf);
	lcd.display();
	client.println(buf);
	//client.println("PA2RDK-9" ">" DESTINATION ":!" LATITUDE "I" LONGITUDE "&" PHG "/" "DATATEST");
	client.println();
}

void display_packet() {
	Serial.println(buf);
}

void display(char *msg) {
	// Put a space before each serial output line, to avoid sending commands to the RadioShield.
	Serial.println(msg);
}

void InitConnection() {
	display("++++++++++++++");
	display("Initialize connection");
	lcd.clear();
	lcd.drawString(0, 0, "Connecting to WiFi" );
	lcd.display();

	WlanReset();
	int agains=1;
	while (((WiFi.status()) != WL_CONNECTED) && (agains < 10)){
		display("++++++++++++++");
		display("Connecting to WiFi");
		WlanReset();
		WiFi.begin(mySSID,PASS);
		delay(5000);
		agains++;
	}
	WlanStatus();
	display("Connected to the WiFi network");
	lcd.drawString(0, 8, "Connected to:");
	lcd.drawString(70,8, mySSID);
	lcd.display();

	if (WlanStatus()==WL_CONNECTED){
		display("++++++++++++++");
		display("WiFi connected");
		display("IP address: ");
		Serial.println(WiFi.localIP());

		if (!client.connected()) {
			display("Connecting...");
			if (client.connect(APRSIP, aprsport)) {
				// Log in
				client.println("user " CALLSIGN " pass " PASSCODE " vers " VERSION);
				client.println(CALLSIGN">APRS,TCPIP*:@072239z"LATITUDE"/"LONGITUDE"I/A=000012 "INFO);
				display("Connected");
				lcd.drawString(0, 16, "Con. APRS-IS:");
				lcd.drawString(70,16, CALLSIGN);
				lcd.display();
			} else {
				display("Failed");
				lcd.drawString(0, 16, "Conn. to APRS-IS Failed" );
				lcd.display();
				// if still not connected, delay to prevent constant attempts.
				delay(1000);
			}
		}
	}
}

void WlanReset() {
	WiFi.persistent(false);
	WiFi.disconnect();
	WiFi.mode(WIFI_OFF);
	WiFi.mode(WIFI_STA);
	delay(1000);
}

int WlanStatus() {
	switch (WiFi.status()) {
	case WL_CONNECTED:
		Serial.print(F("WlanCStatus:: CONNECTED to:"));				// 3
		Serial.println(WiFi.SSID());
		WiFi.setAutoReconnect(true);				// Reconenct to this AP if DISCONNECTED
		return(3);
		break;

		// In case we get disconnected from the AP we loose the IP address.
		// The ESP is configured to reconnect to the last router in memory.
	case WL_DISCONNECTED:
		Serial.print(F("WlanStatus:: DISCONNECTED, IP="));			// 6
		Serial.println(WiFi.localIP());
		return(6);
		break;

		// When still pocessing
	case WL_IDLE_STATUS:
		Serial.println(F("WlanStatus:: IDLE"));					// 0
		return(0);
		break;

		// This code is generated as soonas the AP is out of range
		// Whene detected, the program will search for a better AP in range
	case WL_NO_SSID_AVAIL:
		Serial.println(F("WlanStatus:: NO SSID"));					// 1
		return(1);
		break;

	case WL_CONNECT_FAILED:
		Serial.println(F("WlanStatus:: FAILED"));					// 4
		return(4);
		break;

		// Never seen this code
	case WL_SCAN_COMPLETED:
		Serial.println(F("WlanStatus:: SCAN COMPLETE"));			// 2
		return(2);
		break;

		// Never seen this code
	case WL_CONNECTION_LOST:
		Serial.println(F("WlanStatus:: LOST"));					// 5
		return(5);
		break;

		// This code is generated for example when WiFi.begin() has not been called
		// before accessing WiFi functions
	case WL_NO_SHIELD:
		Serial.println(F("WlanStatus:: WL_NO_SHIELD"));				//255
		return(255);
		break;

	default:
		break;
	}
	return(-1);
}

void setDra(byte rxFreq, byte txFreq, byte rxTone, byte txTone) {
	char buff[50];
	int txPart, rxPart;
	if(txFreq>79) txPart = txFreq-80; else txPart=txFreq;
	if(rxFreq>79) rxPart = rxFreq-80; else rxPart=rxFreq;

	sprintf(buff,"AT+DMOSETGROUP=0,14%01d.%04d,14%01d.%04d,%04d,1,%04d",int(txFreq/80)+4,txPart*125,int(rxFreq/80)+4,rxPart*125,txTone,rxTone);
	Serial.println();
	Serial.println(buff);
	Modem.println(buff);
}
