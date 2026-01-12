
// GPIO defines -------------------------------------------
#define SPEED       A0    // Timer frequency setting input (analog)
#define POWER       A1    // Timer frequency setting input (analog)

#define ANALOG  7
#define BUTTON  8


volatile byte state = HIGH; 

 

// SETUP --------------------------------------------------
void setup() {

	 
	pinMode(ANALOG, OUTPUT);
	
	pinMode(BUTTON, INPUT);
	attachInterrupt(digitalPinToInterrupt(BUTTON), toggle, CHANGE);
  
	pinMode(SPEED, INPUT);  
	pinMode(POWER, INPUT);  

	start();
	
}
volatile uint16_t ocr=1535;
volatile byte power=255;

void start() {
	cli(); // stop interrupts
	TCCR1A = 0; // set entire TCCR1A register to 0
	TCCR1B = 0; // same for TCCR1B
	TCNT1  = 0; // initialize counter value to 0
	// set compare match register for 500 Hz increments
	TIMSK1=0;
	
  	
	//OCR1A = 65535/((1000/  1000   )+1); // 20 ms
	//OCR1A = 65535/((1000/  30   )+1); // 20 ms
	OCR1A = ocr;
 // OCR1A = 1;
	// turn on CTC mode
	TCCR1B = _BV(WGM12) | _BV(CS11);// | _BV(CS10); //8 = 1,000,000/s
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
  
  //TCNT1  = 0; 
	if ( active ) {
		if ( state ) {
     // TCNT0=0; 
      analogWrite(ANALOG, power);
		

      //sei();
		} else {
    	//  digitalWrite(ANALOG,0);
			analogWrite(ANALOG, 0);
		}
		state=!state;
	} else {
		analogWrite(ANALOG, 0);
	}


}



ISR(TIMER1_COMPA_vect) {
  OCR1A=ocr;
 
  setLED();
	
  
	
  
}

 

// LOOP ---------------------------------------------------
void loop() {
	 
	//CR1A = map(s,0,1023,0,65535);
	 int s= analogRead(SPEED);
	 double ss=s/1023.0;
	 double p=pow(ss,2); 
     // int d=map(s,0,1023,1,50);
      ocr=map(floor(p*1023),20,1000,120,10200);
    //  ocr=10;
    
     int ar=analogRead(POWER);
     if ( ar > 800 ) {
       power=255;
     } else if ( ar < 100 ) {
        power=0;
     } else {
	      power=map(ar,20,1000,0,255);
     
     }

     // int d=map(s,0,1023,1,50);
	//power=map(analogRead(POWER),20,1000,0,255);
      
	//OCR1A = 65535/((1000/  30   )+1); // 20 ms
	//10200 = 65535/(( 1000 / x))
  //0.15= 1000/x
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