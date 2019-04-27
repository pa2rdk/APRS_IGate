// Configure your AirGate in this file.

// Set your WiFi info
//#define mySSID "PI4RAZ"
//#define PASS "PI4RAZ_Zoetermeer"

//#define mySSID "iPhone van Robert"
//#define PASS "0919932003"

#define mySSID "MARODEKExtender"
#define PASS "0919932003"

// Set your callsign-SID:
#define CALLSIGN "PA2RDK-11"

// Set TNC frequency:
#define MODEMCHANNEL 64 //12.5 KHz steps, 64 = 144.800

// Set the display timeout in seconds
#define OLEDTIMEOUT 5

// Set your APRS passcode code:
// Contact local APRS hams to find out your passcode, or ask on the book forum site.
#define PASSCODE "21946"

// Define the latitude and longitude of your AirGate:
#define LATITUDE "5204.44N"
#define LONGITUDE "00430.24E"

// PHG gives information about the station capabilities.
// PHG01000 describes an RX-only beacon 20ft AGL with a 0db gain "discone" antenna.
// PHG01600 is the same with a 6dB gain (J-Pole) antenna
// PGO02600 is the J-Pole up 30ft.
// Calculate yours at http://www.aprsfl.net/phgr.php
#define PHG "PHG01000"

// IP address of the APRS-IS backbone gateway in your country:
#define APRSIP "rotate.aprs.net"    //sjc.aprs2.net

#define aprsport 14580

// Software version information, for login and position packet.
#define VERSION "Arduino_RAZ_IGATE_TCP"
#define INFO "Arduino RAZ IGATE"

// APRS destination, which indicates the APRS device being used:
// This one identifies Argent Data products.
#define DESTINATION "APRAZ1"


