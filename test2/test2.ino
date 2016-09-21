/*WORK print the IP addres on the serial monitor.

read from http://www.instructables.com/id/Arduino-Ethernet-Shield-T
 
 Name:		test2.ino
 Created:	6/24/2016 11:03:55 AM
 Author:	nasko
 
// the setup function runs once when you press reset or power the board
*/
/*
DHCP-based IP printer

This sketch uses the DHCP extensions to the Ethernet library
to get an IP address via DHCP and print the address obtained.
using an Arduino Wiznet Ethernet shield.

Circuit:
Ethernet shield attached to pins 10, 11, 12, 13

created 12 April 2011
modified 9 Apr 2012
by Tom Igoe
modified 02 Sept 2015
by Arturo Guadalupi

*/

#include <EthernetUdp.h>
#include <EthernetServer.h>
#include <EthernetClient.h>
#include <Ethernet.h>
#include <Dns.h>
#include <Dhcp.h>
#include <SPI.h>
// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = {
	0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02// this is default address
};

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
EthernetClient client;
void setup() {
	// Open serial communications and wait for port to open:
	Serial.begin(9600);
	// this check is only needed on the Leonardo:
	while (!Serial) {
		; // wait for serial port to connect. Needed for native USB port only
	}
	// start the Ethernet connection:
	if (Ethernet.begin(mac) == 0) {
		Serial.println("Failed to configure Ethernet using DHCP");
		// no point in carrying on, so do nothing forevermore:
		for (;;)
			;
	}
	// print your local IP address:
	printIPAddress();
}

// the loop function runs over and over again until power down or reset
void loop() {
	switch (Ethernet.maintain())
	{
	case 1:
		//renewed fail
		Serial.println("Error: renewed fail");
		break;

	case 2:
		//renewed success
		Serial.println("Renewed success");

		//print your local IP address:
		printIPAddress();
		break;

	case 3:
		//rebind fail
		Serial.println("Error: rebind fail");
		break;

	case 4:
		//rebind success
		Serial.println("Rebind success");

		//print your local IP address:
		printIPAddress();
		break;

	default:
		//Serial.println("No case! This is defaul"); // if this row work after print IP address every second print "No case..."
		//nothing happened
		break;

	}
  
}
void printIPAddress()
{
	Serial.print("My IP address: ");
	for (byte thisByte = 0; thisByte < 4; thisByte++) {
		// print the value of each byte of the IP address:
		Serial.print(Ethernet.localIP()[thisByte], DEC);
		Serial.print(".");
	}
	Serial.println();
}
