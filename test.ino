#include <SPI.h>

volatile byte rxByte;
volatile byte state;

volatile byte selectedRegister;
volatile byte selectedLength;
volatile byte selectedMode;
volatile byte currentByte;
volatile unsigned short currentLongByte;
volatile byte currentWriteByte;

volatile byte tvg_c1[150];
volatile byte tvg_c2[150];
volatile byte tvg_c3[150];
volatile byte tvg_c4[150];
volatile byte tvg_c5[150];
volatile byte tvg_c6[150];
volatile byte tvg_c7[150];
volatile byte tvg_c8[150];

#define S_HEAD_COM 0
#define S_HEAD_LEN 1
#define S_BODY_R_S 2
#define S_BODY_R_L 3
#define S_BODY_W_S 4
#define S_BODY_W_L 5

void ss_rising ()
{
	cli();	
	if(state == S_BODY_R_L || state == S_BODY_W_L)
	if( currentLongByte > 255 ) {
		state = S_HEAD_COM;
		currentLongByte = 0;
	}
	sei();
}

void setup (void)
{
  pinMode(MISO, OUTPUT);
   
  SPDR = 0x00;
  currentByte = 0;
  currentLongByte = 0;
  currentWriteByte = 0;
  selectedLength = 0;
  selectedRegister = 0;
  selectedMode = 0;
  state = S_HEAD_COM;
  rxByte = 0;
  
  SPCR |= _BV(SPE);
  SPCR |= _BV(SPIE);

  attachInterrupt (1, ss_rising, RISING);
}  // end of setup

inline byte getAscanByte(){
	if(currentLongByte == 798)
		return 0xDE;
	else if (currentLongByte == 799)
		return 0xAC;
	else
		return 0xA5;
}

inline void sendSelectedByte(){
	switch(selectedRegister) {
		case 0x00:
		SPDR = 0x00;
		break;
		case 0x01:
		SPDR = 0xFF;
		break;
		case 0x7c:
		SPDR = getAscanByte();
		break;
		case 0x7d:
		SPDR = getAscanByte();
		break;
		case 0x40:
		SPDR = tvg_c1[currentByte];
		break;
		case 0x41:
		SPDR = tvg_c2[currentByte];
		break;
		case 0x42:
		SPDR = tvg_c3[currentByte];
		break;
		case 0x43:
		SPDR = tvg_c4[currentByte];
		break;
		case 0x44:
		SPDR = tvg_c5[currentByte];
		break;
		case 0x45:
		SPDR = tvg_c6[currentByte];
		break;
		case 0x46:
		SPDR = tvg_c7[currentByte];
		break;
		case 0x47:
		SPDR = tvg_c8[currentByte];
		break;
		default:
		SPDR = 0x87;
		break;
	}
}

inline void writeByteToMemory(){
	switch(selectedRegister) {
		case 0x40:
		tvg_c1[currentWriteByte] = rxByte;
		break;
		case 0x41:
		tvg_c2[currentWriteByte] = rxByte;
		break;
		case 0x42:
		tvg_c3[currentWriteByte] = rxByte;
		break;
		case 0x43:
		tvg_c4[currentWriteByte] = rxByte;
		break;
		case 0x44:
		tvg_c5[currentWriteByte] = rxByte;
		break;
		case 0x45:
		tvg_c6[currentWriteByte] = rxByte;
		break;
		case 0x46:
		tvg_c7[currentWriteByte] = rxByte;
		break;
		case 0x47:
		tvg_c8[currentWriteByte] = rxByte;
		break;
	}
}

ISR (SPI_STC_vect)
{
  cli();
  rxByte = SPDR;

  switch(state) {
	case S_HEAD_COM:
	selectedRegister = rxByte & 0b01111111;
	selectedMode = (rxByte & 0b10000000) != 0;
	state = S_HEAD_LEN;
	SPDR = 0x90;
	break;
	case S_HEAD_LEN:
	selectedLength = rxByte;
	if(selectedLength == 0xff) {
		state = S_BODY_R_L;
		sendSelectedByte();
	} else {
		if(selectedMode == 1) {
			state = S_BODY_W_S;
			SPDR = 0xDD;
			//writeByteToMemory();
			currentWriteByte = 0;
		} else {
			state = S_BODY_R_S;
			sendSelectedByte();
			currentByte = 1;
		}
	}
	break;
	case S_BODY_R_S:	
	if(currentByte<selectedLength){
		if(currentByte+1 >= selectedLength) {
			//SPDR = 0xDE;
			sendSelectedByte();
		}
		else {
			sendSelectedByte();
		}
		currentByte++;
	} else {
		SPDR = 0xAC;
		currentByte = 0;
		state = S_HEAD_COM;
	}
	break;
	case S_BODY_R_L:
		sendSelectedByte();
		currentLongByte++;
	break;
	case S_BODY_W_S:
		if(currentWriteByte < selectedLength -1) {
			writeByteToMemory();
			SPDR = 0xDC;
			currentWriteByte++;
		} else {
			writeByteToMemory();
			currentWriteByte = 0;
			SPDR = 0xAC;
			state = S_HEAD_COM;
		}
	break;
	
	
	
	default:
		SPDR = 0xCC;
	break;
  }
  sei();
}  

void loop (void)
{

}
