/*
 Web Server

 A simple web server that shows the value of the analog input pins.
 using an Arduino WIZnet Ethernet shield.

 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13
 * Analog inputs attached to pins A0 through A5 (optional)

 created 18 Dec 2009
 by David A. Mellis
 modified 9 Apr 2012
 by Tom Igoe
 modified 02 Sept 2015
 by Arturo Guadalupi
 
 */

#include <SPI.h>
#include <Ethernet.h>

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(192, 168, 1, 177);

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(80);

void setup() {
  // You can use Ethernet.init(pin) to configure the CS pin
  Ethernet.init(10);  // Most Arduino shields
  //Ethernet.init(5);   // MKR ETH Shield
  //Ethernet.init(0);   // Teensy 2.0
  //Ethernet.init(20);  // Teensy++ 2.0
  //Ethernet.init(15);  // ESP8266 with Adafruit FeatherWing Ethernet
  //Ethernet.init(33);  // ESP32 with Adafruit FeatherWing Ethernet

  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("Ethernet WebServer Example");

  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
/*
  // Check for Ethernet hardware present
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    while (true) {
      delay(1); // do nothing, no point running without Ethernet hardware
    }
  }
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Ethernet cable is not connected.");
  }
*/
  // start the server
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
  DDRB  |= 1<<DDB2;                 // SS Pin has to be an output!!!!
  PORTB |= 1<<PB2;                  // SS Pin set to HIGH so that a connected Pin would deselected
  
  
  SPCR  |= (1<<MSTR);               // Enable Master role
  SPCR  |= (1<<SPE);                // Enable SPI
  
  // To change the clock speed. See datasheet. I had to do this, with full speed I received a lot of wrong data.
  // Can be slowed down and expedited everywhere in the code. So the communication to slave A could be slow at 16 MHz and the 
  // communication to slave B fast with 2 MHz:
  //SPCR  |= ((1<<SPR1) | (1<<SPR0));     

  // Define SCK, MOSI and custom CS as output
  DDRB |= ((1<SS) | (1<<DDB3) | (1<<DDB5));

  pinMode(A0, OUTPUT);
 pinMode(A1, OUTPUT);
}
#define CS PC0 
void sendInt(const char reqID, byte b, byte pin){
  SPCR  |= ((1<<SPR1) | (1<<SPR0));    
  //PORTC &= ~(1<<CS); 
  digitalWrite(pin, LOW);

  delayMicroseconds(20);            // Give the slave some time to digest

  
  // Send out the reqID
  SPDR = reqID;
  /* Wait for transmission complete */
  while(!(SPSR & (1<<SPIF)))
  delayMicroseconds(20);           // Give the slave some time to digest

  // Receive 1st byte. The '11' is not considered from the slave, however this step is necessary to initiate the transmission.
  SPDR = b;                      
  /* Wait for transmission complete */
  while(!(SPSR & (1<<SPIF)))
  //i = SPDR;                 // put the byte index 0 of the 'byte array' = float
  delayMicroseconds(20);
 //Serial.println(b);
 // PORTC |= (1<<CS);             // Set chip select high and stop communication
digitalWrite(pin, HIGH);
   


}
byte a1=0;
byte a2=0;
unsigned long last=0;

void loop() {
  if ( millis() - last > 10 ) {
  sendInt('L',a1, A0);
  sendInt('L',a2, A1);
  a1++;
  a2++;
  a2++;
  last=millis();
  }
  //delay(10);
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    //Serial.println("new client");
    // an HTTP request ends with a blank line
    bool currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        //Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the HTTP request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard HTTP response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
         // client.println("Refresh: 5");  // refresh the page automatically every 5 sec
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          // output the value of each analog input pin
          for (int analogChannel = 0; analogChannel < 6; analogChannel++) {
            int sensorReading = analogRead(analogChannel);
            client.print("analog input ");
            client.print(analogChannel);
            client.print(" is ");
            client.print(sensorReading);
            client.println("<br />");
          }
          client.println("</html>");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data*/
    
    // close the connection:
    client.stop();
    //Serial.println("client disconnected");
  }
}
