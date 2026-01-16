/*  SPI Master Demo written by Robert Mollik, 2016
 *  
 *  This demo uses the SPI protocol on a Arduino Uno R3 / ATmega8 controller
 *  
 *  The demo sends out a request ID depending, whether a temperature or humidity signal shall be obtained from the slave. Both signals 
 *  are datatype float, however get received as 4 bytes and need to be re-assembled.
 */


#define CS PC0                      // Chip select pin, any vacant portpin can be used

//float temp,hum;                     // Variables for the temperature and humidity, which will be recieved.
byte temp,hum;                     // Variables for the temperature and humidity, which will be recieved.

void initSPI(){

  // initilize the SPI functionalities on the ATmega
  
  DDRB  |= 1<<DDB2;                 // SS Pin has to be an output!!!!
  PORTB |= 1<<PB2;                  // SS Pin set to HIGH so that a connected Pin would deselected
  
  
  SPCR  |= (1<<MSTR);               // Enable Master role
  SPCR  |= (1<<SPE);                // Enable SPI
  
  // To change the clock speed. See datasheet. I had to do this, with full speed I received a lot of wrong data.
  // Can be slowed down and expedited everywhere in the code. So the communication to slave A could be slow at 16 MHz and the 
  // communication to slave B fast with 2 MHz:
  SPCR  |= ((1<<SPR1) | (1<<SPR0));     

  // Define SCK, MOSI and custom CS as output
  DDRB |= ((1<SS) | (1<<DDB3) | (1<<DDB5));

}
byte receiveInt(const char reqID){
  byte i=0;
  PORTC &= ~(1<<CS); 
  delayMicroseconds(20);            // Give the slave some time to digest

  
  // Send out the reqID
  SPDR = reqID;
  /* Wait for transmission complete */
  //while(!(SPSR & (1<<SPIF)))
  delayMicroseconds(20);           // Give the slave some time to digest

  // Receive 1st byte. The '11' is not considered from the slave, however this step is necessary to initiate the transmission.
  SPDR = 11;                      
  /* Wait for transmission complete */
 // while(!(SPSR & (1<<SPIF)))
  //i = SPDR;                 // put the byte index 0 of the 'byte array' = float
  delayMicroseconds(20);

  SPDR = 11;                      
  /* Wait for transmission complete */
 // while(!(SPSR & (1<<SPIF)))
 // i = SPDR;                 // put the byte index 0 of the 'byte array' = float
  delayMicroseconds(20);

  PORTC |= (1<<CS);             // Set chip select high and stop communication

  return i;


}
bool sendInt(const char reqID, byte b){
   
  PORTC &= ~(1<<CS); 
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
 Serial.println(b);
  PORTC |= (1<<CS);             // Set chip select high and stop communication

   


}

float receiveFloat(const char reqID){

  float ans=0;                      // target variable for requested signal
  byte* ansPTR = (byte*)&ans;       // byte pointer on the target variable

  // Pull chip select pin down to tell slave that the communications starts
  PORTC &= ~(1<<CS); 
  delayMicroseconds(20);            // Give the slave some time to digest

  
  // Send out the reqID
  SPDR = reqID;
  /* Wait for transmission complete */
  while(!(SPSR & (1<<SPIF)))
  delayMicroseconds(20);           // Give the slave some time to digest

  // Receive 1st byte. The '11' is not considered from the slave, however this step is necessary to initiate the transmission.
  SPDR = 11;                      
  /* Wait for transmission complete */
  while(!(SPSR & (1<<SPIF)))
  ansPTR[0] = SPDR;                 // put the byte index 0 of the 'byte array' = float
  delayMicroseconds(20);

  
  // Receive 2nd byte
  SPDR = 11;
  /* Wait for transmission complete */
  while(!(SPSR & (1<<SPIF)))
  ansPTR[1] = SPDR; 
  delayMicroseconds(20);  

  // Receive 3rd byte
  SPDR = 11;
  /* Wait for transmission complete */
  while(!(SPSR & (1<<SPIF)))
  ansPTR[2] = SPDR; 
  delayMicroseconds(20);  

  // Receive 4th byte
  SPDR = 128;
  /* Wait for transmission complete */
  while(!(SPSR & (1<<SPIF)))
  ansPTR[3] = SPDR; 
  delayMicroseconds(20);    
  

  PORTC |= (1<<CS);             // Set chip select high and stop communication

  return ans;
}

void setup (void)
{
  Serial.begin(115200);         // Serial transmission for demonstration puroses
  
  initSPI();
  DDRC |= (1<<DDC0);            // datadrection for chip select is not in initSPI()
}  
byte a=120;

void loop (void)
{
  sendInt('L',a);
  a++;

  //temp = receiveInt('T');     // See function definition
  //delay(100);
  //hum = receiveInt('H');
  ///delay(100);

  // Print in serial monitor for demonstration
  //Serial.print("T = ");
 // Serial.print(temp, DEC);
  //Serial.println();

  //Serial.print("H = ");
 // Serial.print(hum, DEC);
  //Serial.println(); 

  delay(1);
  
}  // end of loop