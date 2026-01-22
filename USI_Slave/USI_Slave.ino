/*  USI SPI Slave Demo written by Robert Mollik, 2016
 *  
 *  This demo uses the SPI protocol with USI interface. This is necessary with with the smaller AVR controller since they don't 
 *  a complete SPI functionality. 
 *  Demo is written for the pinout of an Arduino Trinket and ATtiny85. Pins need to be modified if a different controller is used.
 *  
 *  The demo takes receives a request ID from the Master at the beginning of every communication. According to the request ID,
 *  it sends back either a temperature or a humidity values. Since both values are present in float, the 4 bytes of the float need
 *  to be send separately.
 */

#define CSp    PA3//PA0 //PB3     // Chip select
#define LED   PA7 //PB4     // Test LED
#define DO    PA5     // MISO or Data Out
#define USCK  PA4     // Clock
#include <wiring_private.h>
// Variable use in Interrupt need to be volotile to tell the compiler, they may be altered in other functions than in Main loop.
// Otherwise the compiler could remove or replace them with a constant:

volatile char reqID = 0;              // This is for the first byte we receive, which is intended to be the request identifier
volatile uint8_t index = 0;           // this is to send back the right element in the array
volatile char ana = 0;  
// Test variables:

volatile byte temperature = 10;
volatile byte humidity = 20;

// Pointer on the test variables. Datatype is casted into byte to read out the single bytes of the float:

byte * tempPTR = (byte*) &temperature;
byte * humPTR  = (byte*) &humidity;

void setup (void)
{
  cli();                              // Deactivate Interrupts
 // pinMode(LED, OUTPUT);
  DDRA |= 1<<LED;                     // Set PB1 as output (LED on Trinket) / PB0 will be the Input for the Interrupt
  DDRA |= 1<<DO;                      // MISO Pin has to be an output

  USICR = ((1<<USIWM0)|(1<<USICS1));  // Activate 3- Wire Mode and use of external clock but NOT the interrupt at the Counter overflow (USIOIE)
  
  PORTA |= 1<<LED;                    // Turn PB1 off
  PORTA |= 1<<CSp;                     // Activate Pull-Up resistor on PB0
  
   GIMSK = (1 << PCIE0);
    PCMSK0 = (1 << CSp);
sei();

 delay(200);
  digitalWrite(7, HIGH);
   delay(500);
   digitalWrite(7, LOW);
   delay(500);
  digitalWrite(7, HIGH);
   delay(500);
  digitalWrite(7, LOW);
   delay(500);
      
 // PORTA^= 1<<LED;    
  //PORTA= 1<<LED;
  //pinMode(7,OUTPUT);    
  //analogWrite(7,150);    
   sei();                  
}  // end of setup


// Interrupt routine at the CS pin. Always executed when value on CS pin changes:

ISR(PCINT0_vect)
  {    
    //PORTA= 0<<LED;  
    if((PINA & (1<<CSp))== 0){
      
      // If edge is falling, the command and index variables shall be initialized
      // and the 4-bit overflow counter of the USI communication shall be activated:

      reqID = 0;
      index = 0;
      USICR |= (1<<USIOIE);
      USISR = 1<<USIOIF;      // Clear Overflow bit

      // XOR operation to turn PB3 (LED) on/off. (Not needed for USI)
      
      //PORTA^= 1<<LED;  
    } else{

      // If edge is rising, turn the 4-bit overflow interrupt off:
      
      USICR &= ~(1<<USIOIE);
    }
  }

 // USI interrupt routine. Always executed when 4-bit overflows (after 16 clock edges = 8 clock cycles):

ISR(USI_OVF_vect)
  {  
    
      switch(reqID){

        // Switch-Case to respond according to request from Master:

        case 0:               // If reqID value is zero (just initialized), then first message is the reqID.
          temperature++; 
          //PORTA^= 1<<LED; 
          reqID = USIDR;      // Read in from USIDR register
          USISR = 1<<USIOIF;  // Clear Overflow bit
          break;
        case 'L': {
            ana = USIDR;      // Read in from USIDR register
            // analogWrite(7,ana);
            sbi(TCCR0A, COM0B1); 
            OCR0B = ana; 
            //PORTA= 0<<LED;  
            USISR = 1<<USIOIF;  // Clear Overflow bit
            reqID=0;
            break;

        }
        
        case 'T':

          // Write value to send back into USIDR and clear the overflow bit:
          USIDR = temperature;//tempPTR[index];
          USISR = 1<<USIOIF;      
           
          index++;            // Increment index to transmit the folloing element next
          break;
          
        case 'H':
        
          // Write value to send back into USIDR and clear the overflow bit:
          USIDR = humidity;//humPTR[index]; 
          USISR = 1<<USIOIF; 
          //humidity++;
          index++;            // Increment index to transmit the folloing element next
          break;

        default:
          // Default option of Switch-Case. Send 'reqID' back for debugging.
          USIDR = reqID;
          USISR = 1<<USIOIF;
          
      }      
  }

void loop (void)
{
  // Nothing done here in this example
}  