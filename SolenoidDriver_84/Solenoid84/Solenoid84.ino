
// GPIO defines -------------------------------------------
#define SPEED       A3    // Timer frequency setting input (analog)
//#define CLKOUT      6     // Timer output (TIMER1, OC1A)
#define LED      0     // Timer output (TIMER1, OC1A)
#define ANALOG  7
volatile byte state = HIGH; 
#define BUTTON  8
// Timer frequency function -------------------------------
void timerFreq() {
  OCR1A = 0;//255;//252 - 0;//(analogRead(SPEED)/6); // Yields approx. 16kHz to 48kHz freq range
}

// SETUP --------------------------------------------------
void setup() {
//	pinMode(CLKOUT, OUTPUT);  // Set pin as output
	pinMode(LED,OUTPUT);
	pinMode(ANALOG,OUTPUT);
	digitalWrite(LED, LOW); 
	
	pinMode(BUTTON, INPUT);
	attachInterrupt(digitalPinToInterrupt(BUTTON), toggle, CHANGE);
  
	//TCCR1A = _BV(COM1A0) | _BV(WGM11) | _BV(WGM10);
	//TCCR1B = _BV(CS10) | _BV(CS12) | _BV(WGM12) | _BV(WGM13);
	//OCR1A = 254;
	//TIMSK1 |= (1 << OCIE1A);
	//TCCR1A = 0;
	//TCCR1B = 0;
	//TCNT1  = 0;
	
	//1500 = 43hz = 23.25
	//1350 = 47hz = 21.28
	//1200 = 53hz = 18.87
	//1050 = 61hz = 16.39
	
	//TCCR1B |= (1 << WGM12);
	////TCCR1B |= (1 << CS12) | (1 << CS10);
	//TCCR1A =   _BV(WGM10);
	//TCCR1B =    _BV(CS11) |  _BV(WGM12);
	//OCR1A = 65535;//0;//255-(16*0);
	
	//TCCR1A =   _BV(WGM11) | _BV(WGM10);
	//TCCR1B = _BV(WGM12) |_BV(WGM13) | _BV(CS12);
	//OCR1A = 1025; //16000000hz/256hz/60hz  ≈ 1025
	//OCR1B = 512;
   
	
	 
	//TIMSK1 |= _BV(OCIE1A);
	

	start();
	
}

void start() {
	cli(); // stop interrupts
	TCCR1A = 0; // set entire TCCR1A register to 0
	TCCR1B = 0; // same for TCCR1B
	TCNT1  = 0; // initialize counter value to 0
	// set compare match register for 500 Hz increments
	TIMSK1=0;
	
	//OCR1A = 65535/((1000/  1000   )+1); // 20 ms
	OCR1A = 65535/((1000/  35   )+1); // 20 ms
	
 // OCR1A = 1;
	// turn on CTC mode
	TCCR1B = _BV(WGM12) | _BV(CS11);// | _BV(CS10);
	// Set CS12, CS11 and CS10 bits for 1 prescaler
	
	//TCCR1B |= _BV(CS10);
	// enable timer compare interrupt
	//TIMSK1 |= (1 << OCIE1A);
	TIMSK1 = (1 << OCIE1A);
	sei(); // allow interrupts
}

bool active=true;

void toggle() {
//	if ( debounce() ) {
	
		active=digitalRead(BUTTON);
    if ( active ) {
        unsigned long t = TCNT1;
      	start();
        TCNT1=0;
        state=HIGH;
        setLED();
        // TCNT1=0;
      	 
    } else {
      state=LOW;
      setLED();

    }
	//}
	
}

void setLED() {
  if ( active ) {
		 	if ( state ) {
			analogWrite(ANALOG,255);
		} else {
    	  digitalWrite(ANALOG,0);
			analogWrite(ANALOG,0);
		}
	 state=!state;
	} else {
		digitalWrite(ANALOG,0);
	}


}
ISR(TIMER1_COMPA_vect) {
  setLED();
	
  
	
  
}

 

// LOOP ---------------------------------------------------
void loop() {
 // timerFreq();
}
 

bool debounce() {
  static uint16_t btnState = 0;
  btnState = (btnState<<1) | (!digitalRead(BUTTON));
  return (btnState == 0xFFF0);
}

ISR(TIMER1_OVF_vect) {
 //digitalWrite(LED, !digitalRead(LED)); 

}