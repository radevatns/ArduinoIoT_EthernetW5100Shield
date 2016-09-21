/*work but temp is refresh while bush the button for led ON or OFF
 Name:		readTempAndControlLedWithWEB.ino
 Created:	6/27/2016 5:40:32 PM
 Author:	nasko

LM35DZ is connected to A0 output
Led is connected to D2 -> Long foot = D2, short - R330 OM - GND
*/


#include <SPI.h>
#include <EthernetUdp.h>
#include <EthernetServer.h>
#include <EthernetClient.h>
#include <Ethernet.h>
#include <Dns.h>
#include <Dhcp.h>
byte mac[] = { 0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02 };
IPAddress ip(192, 168, 15, 101); // IP address, may need to change depending on network
byte gateway[] = { 192, 168, 15, 1 };// the router's gateway address: * not a obligatory
byte subnet[] = { 255, 255, 255, 0 };// the subnet: * not a obligatory
 EthernetServer server(80);  // create a server at port 80
float powervoltage = 5;
String HTTP_req;          // stores the HTTP request
boolean LED_status = 0;   // state of LED, off by default

void setup()
{
	//Ethernet.begin(mac, ip);  // initialize Ethernet device* this is obligatory
	Ethernet.begin(mac, ip, gateway, subnet); //this is not obligatory
	server.begin();           // start to listen for clients
	Serial.begin(9600);       // for diagnostics
	pinMode(2, OUTPUT);

	analogReference(INTERNAL);//this is for tempReading
	delay(500);
	while (!Serial) {
		;
	}// wait for serial port to connect. Needed for native USB port only// LED on pin 2
	Serial.print("server is at ");
	Serial.println(Ethernet.localIP());
}


void loop()
{
	float temperature; // start temp measurement and print to serial monitor
					   // read the input on analog pin 0:
	float sensorValue = analogRead(A2);
	// print out the value you read:
	temperature = (analogRead(A2)) / 10.26;
	//temperature = (1.078*sensorValue / 1023)*96.33;// by default *100, with campare Fluke *95
	Serial.print("The room temperature degree is:");
	Serial.print(temperature, 1);
	Serial.println(" C");
	delay(100);

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
					client.println("<title>Arduino LED Control and Temp Monitoring</title>");
					client.println("</head>");
					client.println("<body>");
					client.println("<h1>LED control</h1>");
					client.println("<p>Click to switch LED on and off.</p>");
					client.println("<form method=\"get\">");
					ProcessCheckbox(client);// this is metod
					//add the temp information
					client.println("<br />");
					client.println("<h1>Temeprature monitoring</h1>");
					client.print("The temperature is ");
					client.print(temperature, 1);
					client.println(" C");
					client.println("<br />");
					client.println("</html>");
					client.print("The row data from LM35DZ is ");
					client.print(sensorValue);
					//end the temp information

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
