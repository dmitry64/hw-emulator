#include <SPI.h>

volatile byte rxByte;
volatile byte state;

volatile byte selectedRegister;
volatile byte selectedLength;
volatile byte currentByte;

void ss_falling ()
{
  if(state==2)
	state = 0;
}

void setup (void)
{
  Serial.begin (115200);   // debugging

  // have to send on master in, *slave out*
  pinMode(MISO, OUTPUT);
  pinMode(9, INPUT);
 
  /*DDRB  |= 0b00010000; 
   //The Port B 
  PORTB |= 0b00000100*/
  
//SPI Control Register
  SPCR  |= 0b11000000;
//SPI status register
  SPSR  |= 0b00000000;
  SPDR = 0x99;
  
  //SPDR = 0x00;
  currentByte = 0;
  selectedLength = 0;
  selectedRegister = 0;
  state = 0;
  rxByte = 0;
  attachInterrupt (0, ss_falling, FALLING);
  //SPI.setDataMode(SPI_MODE0);

 
  sei();

}  // end of setup

void sendSelectedByte(){
	switch(selectedRegister) {
		case 0x00:
		SPDR = 0x08;
		break;
		case 0x01:
		SPDR = 0xFF;
		break;
		case 0x7c:
		SPDR = 0x7c;
		break;
		case 0x7d:
		SPDR = 0x7d;
		break;
		default:
		SPDR = 0x87;
		break;
	}
	//if(selectedLength != 0xff) {
		//currentByte++;
	//}
}


// SPI interrupt routine
ISR (SPI_STC_vect)
{
  //cli();
  rxByte = SPDR;
  //SPDR = 0x09;
  switch(state) {
	case 0:
	selectedRegister = rxByte;
	state = 1;
	SPDR = 0x90;
	break;
	case 1:
	selectedLength = rxByte;
	/*if(selectedLength == 255) {
		state = 3;
	} else {*/
	state = 2;
	//}
	currentByte = 1;
	//SPDR = 0x91;
	sendSelectedByte();
	break;
	case 2:
	
	
	
	if(currentByte<selectedLength){
		if(currentByte+1 >= selectedLength) {
			SPDR = 0xDE;
		}
		else {
			sendSelectedByte();
		}
		//SPDR = 0x96;
		currentByte++;
	} else {
		SPDR = 0xAC;
		selectedLength = 0;
		selectedRegister = 0;
		currentByte = 0;
		state = 0;
	}
	
		//sendSelectedByte();
		//state = 0;
	//}
	break;
	/*case 3:
	if(rxByte == 0x00) {
		sendSelectedByte();
	} else {
		selectedRegister = rxByte;
		state = 1;
		currentByte = 0;
		SPDR = 0x07;
	}
	//SPDR = 0x66;
	break;*/
	
	
	default:
	state = 0;
	SPDR = 0x99;
	break;
  }
  //delay(1);
  //while(!(SPSR & (1 << SPIF)));
  
  //while SS Low
  /*while(!(PINB & 0b00000100)){
	//dataEcho = SPDR;
	buf[index] = SPDR;
	index++;
	
	//Serial.println(dataEcho);
	//delay(1);
    //SPDR = dataEcho;
    
    //wait SPI transfer complete
  while(!(SPSR & (1 << SPIF)));
    //delay(1);
    
    //  //send back in next round
  }*/
  //sei();
}  

void loop (void)
{
	//while(true);
	/*if(digitalRead(10) == HIGH) {
		if(state == 2) {
			state = 0;
		}
	}*/
}
