#include <SPI.h>

volatile byte rxByte;
volatile byte state;

volatile byte selectedRegister;
volatile byte selectedLength;
volatile byte selectedMode;
volatile byte currentByte;
volatile unsigned int currentLongByte;
volatile byte currentWriteByte;

volatile byte ascanCounter;


struct tact{
	volatile byte CR;
	volatile byte TR1;
	volatile byte PULSER1;
	volatile byte TR2;
	volatile byte PULSER2;
	volatile byte RES;
};

// Registers:
volatile byte ForceZero;
volatile byte ForceOne;
volatile byte USM_ID;
volatile byte USM_SR;
volatile byte USM_CR;
volatile byte TRG_CR;
volatile byte TRG_DS;
volatile byte TRG_TS;
volatile byte PWR_CR;
volatile byte ODO_CR;

// Chanels table:
volatile tact CH_TAB[8];

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

volatile byte testfunc[255] = { 0x3f, 0x3f, 0x40, 0x40, 0x41, 0x41, 0x42, 0x42, 0x43, 0x43, 0x43, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x43, 0x43, 0x43, 0x42, 0x42, 0x41, 0x40, 0x40, 0x3f, 0x3f, 0x3e, 0x3d, 0x3d, 0x3c, 0x3c, 0x3b, 0x3b, 0x3a, 0x3a, 0x3a, 0x3a, 0x3a, 0x3a, 0x3a, 0x3a, 0x3b, 0x3b, 0x3b, 0x3c, 0x3d, 0x3d, 0x3e, 0x3f, 0x40, 0x41, 0x42, 0x42, 0x43, 0x44, 0x45, 0x45, 0x46, 0x47, 0x47, 0x47, 0x48, 0x48, 0x48, 0x47, 0x47, 0x47, 0x46, 0x45, 0x45, 0x44, 0x43, 0x42, 0x40, 0x3f, 0x3e, 0x3d, 0x3b, 0x3a, 0x39, 0x37, 0x36, 0x35, 0x34, 0x33, 0x33, 0x32, 0x32, 0x32, 0x32, 0x32, 0x33, 0x33, 0x35, 0x36, 0x37, 0x39, 0x3b, 0x3d, 0x40, 0x42, 0x45, 0x48, 0x4b, 0x4f, 0x52, 0x55, 0x59, 0x5c, 0x60, 0x63, 0x67, 0x6a, 0x6d, 0x70, 0x72, 0x75, 0x77, 0x79, 0x7b, 0x7c, 0x7e, 0x7e, 0x7f, 0x7f, 0x7f, 0x7e, 0x7e, 0x7c, 0x7b, 0x79, 0x77, 0x75, 0x72, 0x70, 0x6d, 0x6a, 0x67, 0x63, 0x60, 0x5c, 0x59, 0x55, 0x52, 0x4f, 0x4b, 0x48, 0x45, 0x42, 0x40, 0x3d, 0x3b, 0x39, 0x37, 0x36, 0x35, 0x33, 0x33, 0x32, 0x32, 0x32, 0x32, 0x32, 0x33, 0x33, 0x34, 0x35, 0x36, 0x37, 0x39, 0x3a, 0x3b, 0x3d, 0x3e, 0x3f, 0x40, 0x42, 0x43, 0x44, 0x45, 0x45, 0x46, 0x47, 0x47, 0x47, 0x48, 0x48, 0x48, 0x47, 0x47, 0x47, 0x46, 0x45, 0x45, 0x44, 0x43, 0x42, 0x42, 0x41, 0x40, 0x3f, 0x3e, 0x3d, 0x3d, 0x3c, 0x3b, 0x3b, 0x3b, 0x3a, 0x3a, 0x3a, 0x3a, 0x3a, 0x3a, 0x3a, 0x3a, 0x3b, 0x3b, 0x3c, 0x3c, 0x3d, 0x3d, 0x3e, 0x3f, 0x3f, 0x40, 0x40, 0x41, 0x42, 0x42, 0x43, 0x43, 0x43, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x43, 0x43, 0x43, 0x42, 0x42, 0x41, 0x41, 0x40, 0x40, 0x3f, 0x3f };

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
  ascanCounter = 0;
  state = S_HEAD_COM;
  rxByte = 0;
  
  ForceZero = 0x00;
  ForceOne = 0xFF;
  USM_ID = 0xAE;
  USM_SR = 0b00000001;
  USM_CR = 0b00000000;
  TRG_CR = 0b00000000;
  TRG_DS = 0b00000000;
  TRG_TS = 0b00000000;
  PWR_CR = 0b00000000;
  ODO_CR = 0b00000000;
  
  for(unsigned short i=0; i<8; i++) {
	CH_TAB[i].CR = 0b00000000;
	CH_TAB[i].TR1 = 0b00000000;
	CH_TAB[i].PULSER1 = 0b00000000;
	CH_TAB[i].TR2 = 0b00000000;
	CH_TAB[i].PULSER2 = 0b00000000;
	CH_TAB[i].RES = 0b00000000;
  }
  
  SPCR |= _BV(SPE);
  SPCR |= _BV(SPIE);

  attachInterrupt (1, ss_rising, RISING);
}  // end of setup

inline byte getAscanHeader(){
	return 0xCC;
}

inline byte getAscanBody(){
	
	ascanCounter++;
	if(ascanCounter == 255) {
		ascanCounter = 0;
	}
	return testfunc[ascanCounter];
}

inline byte getAscanByte(){
	if(currentLongByte < 11) {
		return getAscanHeader();
	} else {
		return getAscanBody();
	}

}

inline void sendSelectedByte(){
	switch(selectedRegister) {
		case 0x00:
		SPDR = ForceZero;
		break;
		case 0x01:
		SPDR = ForceOne;
		break;
		case 0x02:
		SPDR = USM_ID;
		break;
		case 0x03:
		SPDR = USM_SR;
		break;
		case 0x04:
		SPDR = USM_CR;
		break;
		case 0x05:
		SPDR = TRG_CR;
		break;
		case 0x06:
		SPDR = TRG_DS;
		break;
		case 0x07:
		SPDR = TRG_TS;
		break;
		case 0x08:
		SPDR = PWR_CR;
		break;
		case 0x09:
		SPDR = ODO_CR;
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
		if(selectedRegister >= 0x10 && selectedRegister <= 0x3f) {
			byte addr = selectedRegister - 0x10;
			volatile tact * ptr = &(CH_TAB[addr / 6]);
			switch(addr % 6) {
				case 0x00:
				SPDR = ptr->CR ;
				break;
				case 0x01:
				SPDR = ptr->TR1;
				break;
				case 0x02:
				SPDR = ptr->PULSER1;
				break;
				case 0x03:
				SPDR = ptr->TR2;
				break;
				case 0x04:
				SPDR = ptr->PULSER2;
				break;
				case 0x05:
				SPDR = ptr->RES;
				break;
			}
		} else {
		SPDR = 0xEE;
		}
		break;
	}
}

inline void writeByteToMemory(){
	switch(selectedRegister) {
		case 0x05:
		TRG_CR = rxByte & 0b00011101;
		break;
		case 0x06:
		TRG_DS = rxByte;
		break;
		case 0x07:
		TRG_TS = rxByte;
		break;
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
		default:
		if(selectedRegister >= 0x10 && selectedRegister <= 0x3f) {
			byte addr = selectedRegister - 0x10;
			volatile tact * ptr = &(CH_TAB[addr / 6]);
			switch(addr % 6) {
				case 0x00:
				ptr->CR = rxByte;
				break;
				case 0x01:
				ptr->TR1 = rxByte;
				break;
				case 0x02:
				ptr->PULSER1 = rxByte;
				break;
				case 0x03:
				ptr->TR2 = rxByte;
				break;
				case 0x04:
				ptr->PULSER2 = rxByte;
				break;
				case 0x05:
				ptr->RES = rxByte;
				break;
			}
		}
		
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
