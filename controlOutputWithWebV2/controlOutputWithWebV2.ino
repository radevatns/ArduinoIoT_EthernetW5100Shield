/* tested and work !
from: http://startingelectronics.org/tutorials/arduino/ethernet-shield-web-server-tutorial/web-server-LED-control/
 Name:		controlOutputWithWebV2.ino
 Created:	6/27/2016 3:00:59 PM
 Author:	nasko
*/

/*--------------------------------------------------------------
Program:      eth_websrv_LED

Description:  Arduino web server that serves up a web page
allowing the user to control an LED

Hardware:     - Arduino Uno and official Arduino Ethernet shield. Should work with other 
Arduinos and compatible Ethernet shields. - LED and resistor in series connected between
Arduino pin 2 and GND

Software:     Developed using Arduino 1.0.3 software
Should be compatible with Arduino 1.0 +

References:   - WebServer example by David A. Mellis and
modified by Tom Igoe
- Ethernet library documentation:
http://arduino.cc/en/Reference/Ethernet

Date:         11 January 2013

Author:       W.A. Smith, http://startingelectronics.org
--------------------------------------------------------------*/



// MAC address from Ethernet shield sticker under board
#include <SPI.h>
#include <EthernetUdp.h>
#include <EthernetServer.h>
#include <EthernetClient.h>
#include <Ethernet.h>
#include <Dns.h>
#include <Dhcp.h>
byte mac[] = { 0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02 };
IPAddress ip(192, 168, 15, 101); // IP address, may need to change depending on network
EthernetServer server(80);  // create a server at port 80

String HTTP_req;          // stores the HTTP request
boolean LED_status = 0;   // state of LED, off by default

void setup()
{
	Ethernet.begin(mac, ip);  // initialize Ethernet device
	server.begin();           // start to listen for clients
	Serial.begin(9600);       // for diagnostics
	pinMode(2, OUTPUT);       // LED on pin 2
}

void loop()
{
	EthernetClient client = server.available();  // try to get client

	if (client) {  // got client?
		boolean currentLineIsBlank = true;
		while (client.connected()) {
			if (client.available()) {   // client data available to read
				char c = client.read(); // read 1 byte (character) from client
				HTTP_req += c;  // save the HTTP request 1 char at a time
								// last line of client request is blank and ends with \n
								// respond to client only after last line received
				
								//add this for google chrome. Because when click the led on,I cannot off it
				if (HTTP_req.indexOf("favicon.ico") > -1) {

					HTTP_req = "";

					break;

				} // end of add for google chorme

				if (c == '\n' && currentLineIsBlank) {
					// send a standard http response header
					client.println("HTTP/1.1 200 OK");
					client.println("Content-Type: text/html");
					client.println("Connection: close");
					client.println();
					// send web page
					client.println("<!DOCTYPE html>");
					client.println("<html>");
					client.println("<head>");
					client.println("<title>Arduino LED Control</title>");
					client.println("</head>");
					client.println("<body>");
					client.println("<h1>LED</h1>");
					client.println("<p>Click to switch LED on and off.</p>");
					client.println("<form method=\"get\">");
					ProcessCheckbox(client);
					client.println("</form>");
					client.println("</body>");
					client.println("</html>");
					Serial.print(HTTP_req);
					HTTP_req = "";    // finished with request, empty string
					break;
				}
				// every line of text received from the client ends with \r\n
				if (c == '\n') {
					// last character on line of received text
					// starting new line with next character read
					currentLineIsBlank = true;
				}
				else if (c != '\r') {
					// a text character was received from client
					currentLineIsBlank = false;
				}
			} // end if (client.available())
		} // end while (client.connected())
		delay(1);      // give the web browser time to receive the data
		client.stop(); // close the connection
	} // end if (client)
}

// switch LED and send back HTML for LED checkbox
void ProcessCheckbox(EthernetClient cl)
{
	if (HTTP_req.indexOf("LED2=2") > -1) {  // see if checkbox was clicked
											// the checkbox was clicked, toggle the LED
		if (LED_status) {
			LED_status = 0;
		}
		else {
			LED_status = 1;
		}
	}

	if (LED_status) {    // switch LED on
		digitalWrite(2, HIGH);
		// checkbox is checked
		cl.println("<input type=\"checkbox\" name=\"LED2\" value=\"2\" \
        onclick=\"submit();\"checked>LED2");
	}
	else {              // switch LED off
		digitalWrite(2, LOW);
		// checkbox is unchecked
		cl.println("<input type=\"checkbox\" name=\"LED2\" value=\"2\" \
        onclick=\"submit();\">LED2");
	}
}
