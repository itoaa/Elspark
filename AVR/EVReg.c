// ****************************************
// 
// EVReg.c
// $Rev$
// Ola Adersson
//
// ****************************************

#ifndef F_CPU
  #define F_CPU 8000000					//define cpu clock speed if not defined
#endif

const int BAUDRATE = 9600;				//set desired baud rate

char	IntFired=0;						// Setts when int is fired.	

const int CodeRev = 0x02;

#include <avr/io.h>
// #include "gpio.h"
#include <avr/interrupt.h>
#include "EVReg.h"
#include "m_serial.h"
#include "m_measure.h"
#include "EVReg_misc.h"

#define EEPROM_SECTION  __attribute__ ((section (".eeprom")))

// The Constand eeprom_page1 is defined and stored in EEPROM, with all user data
const page1data eeprom_page1 EEPROM_SECTION = {
{1,2,3,4,5,6,7,8},				// TimeAmpTable
400,							// Motor MaxAmp1Sec
350,							// MaxAmp5Sec
300,							// MaxAmp1Min
250,							// MaxAmp10Min
200,							// MaxAmpConst
90,								// MaxTemp
{1,15,60,77,104,131,167,186,203,217,228,236},		// mosfetTempADCTable
{22,40,60,65,75,85,100,110,120,130,140,150},		// mosfetTempTable, note that real temp is value-40
10								// accDelay
};

// Copy of values in ram
page1data pg1_var;
page1data *pg1_ptr = &pg1_var;


// Define RPage, including all realtime data sent on the serialport
RPageVars RPage;

// Refine a pointer til the realtimedata variable.
// char *RPagePointer = &RPage;
struct RPageVarsSruct *RPagePointer = &RPage;
char RTPS = 14;							// RealTimePageSize

// PWM var def.
unsigned int Next_PWM=0, Last_PWM=0, Max_Amp=5, Acceleration_Smothing_Factor=20, OverCurrent_retard=28;

// Subrutine
// Writes all data in the variable to EEPROM (Save user changes to EEPROM)
void	WriteStructureToEeprom()
		{
   			eeprom_write_block((const void*)&pg1_var, (void*)&eeprom_page1, sizeof(page1data));
		} 


// Subrutine
// Read all data in the EEPROM to variable (Att startup, get all parameters from EEPROM)
void 	ReadStructureFromEeprom()
		{
   			eeprom_read_block((void*)&pg1_var, (const void*)&eeprom_page1, sizeof(page1data));
		} 


// This interupt is fired when somting is reseved on the serial line. 
// The only thing that is done is to set ResevedSerialByte to 1.
// All handling for the byte is done outeside the ISR.
ISR(USART_RX_vect)
{
	ResevedSerialByte = 1;
}


// Initiation of the PWM
// Need fixing, is it possible to go active high??
void initPWM(){
    DDRD = (1 << 6);					// Set PD6 to output (for PWM) 
    OCR0A = 10;							// Set Plulse with 10
 //   TCNT0 = 0;
    TIMSK0 = (1 << OCIE0A);				// Enable Output compare match interrupt
    TCCR0A = (1 << COM0A1) | (1 << WGM01) | (1 <<  WGM00); // 1, Clear pin on Compare mach. 2-3, Fast PWM Mode
//   TCCR0A = (1 << COM0A1) |(1 << COM0A0) | (1 << WGM01) | (1 <<  WGM00); // 1-2, Set pin on Compare mach. 3-4, Fast PWM Mode
	
	//    TCCR0B = (1 << CS01);				// Use internal clock, (/8 prescale)
    TCCR0B = (1 << CS00);				// Use internal clock, (no prescale)
										// This meens 20KHz ??
    sei();								// Global interrupt enable
}


// Initiate the ADC
void initADC(){
//	PRR |= (1<<PRADC);  						// Supply current to ADC
//	ADCSRA = (1<<ADPS1)|(1<<ADPS2);				//  Prescale / 64
	ADCSRA = (1<<ADPS1)|(1<<ADPS0);				//  Prescale / 8  (convertion time??)
	ADMUX = (0<<REFS1)|(1<<REFS0);				//  Select AVCC ref, Voltage
	DIDR0 = (1<<ADC1D); 						//  Digital Input Disable (??)
}

// Init the Serialconnection
void initUART(void) {

  	UBRR0=UBRRVAL;								// Set correct baud rate, calculated from cpu-clock.
	
												// Set data frame format: 
	UCSR0C=(0<<UMSEL00)|(0<<UPM01)|(0<<UPM00)|	// Asynchronous mode,no parity, 1 stop bit, 8 bit size
		(0<<USBS0)|(0<<UCSZ02)|(1<<UCSZ01)|(1<<UCSZ00);	
	
	UCSR0B=(1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0);	// Enable Transmitter and Receiver 
												// and Interrupt on receive complete
	sei();										// Global interrupt enable
}

// If a serial byte is resived, deside what to do whit it.
void HandleSerialByte(){

	uint8_t SerialByteTemp;								// Defien temp value for storing received byte
	
	SerialByteTemp=UDR0;								// Store resived data to temp
	ResevedSerialByte = 0;								// Reset ResevedSerialByte to 0. Data is stored, and ready for next byte

	switch(SerialByteTemp)								// Evaluate resived data
	{
		case 'A': 
			Send_Realtimedata(RPagePointer);		// If "A" is resived, send Realtimedata
			break;
		case 'C': 
			Send_Sec(RPage.seconds);					// If "C" is resived, send Seconds (comm test)
			break;
//		case 'V': 
//			Send_Realtimedata(RPagePointer,RTPS);		// If "V" is resived, send Realtimedata.
//			break;
		case 'Q': 
			Send_Rev(CodeRev);							// If "Q" is resived, send code revision.
			break;
		case 'B': 
			WriteStructureToEeprom();					// If "B" is resived, Write pg1_var to EEPROM.
			break;
		case 'V': 
			Send_EpromVar(pg1_ptr);						// If "V" is resived, send pg1_var.
			break;
		case 'W': 
			Get_EpromVar(pg1_ptr,sizeof(pg1_var));					// If "W" is resived, get pg1_var.
			break;

		default: break;
	}
}

// Main loop.
int main()
{
	initPWM();											// Init PWM
	initADC();											// Init ADC
    initUART();											// Init Serial comunication
	
	int loop1;											// Define loop variable
	unsigned char temp44;
	unsigned int PulsWith;								// Define PW storage variable
	ReadStructureFromEeprom(pg1_var);					// Read initial values from EEPROM
	while(1)											// Loop forever
	{
		for (loop1=0;loop1 < 8000;loop1++)
		{
			if (ResevedSerialByte == 1) HandleSerialByte();			
			if (loop1 == 100)  
			{	
				Next_PWM = PotMeasure();
				
				if (RPage.AmpADC > Max_Amp*7)
					Next_PWM = Last_PWM - OverCurrent_retard;
				if (Last_PWM < (Next_PWM - Acceleration_Smothing_Factor))
					Next_PWM = Last_PWM + Acceleration_Smothing_Factor;
				Last_PWM = Next_PWM;
				RPage.TPSPos = Next_PWM;
				PulsWith=255-Next_PWM;			// Set PulsWith to Next_PWM.
		//		Next_PWM = 0;

				if (PulsWith < 5) PulsWith = 5;
				if (PulsWith > 250) PulsWith = 250;

				OCR0A=PulsWith;					// Set PWM pulwith to filtered value.
			}
			if (loop1 == 2100) RPage.AmpADC = AmpMeasure(RPage.TPSPos);
			if (loop1 == 4100) RPage.BattVoltage = B_VoltMeasure();
			if (loop1 == 6100) 
			{
				RPage.MosfetTempADC = MOS_TempMeasure();
				RPage.MosfetTemp = GetMosfetTemp(RPage.MosfetTempADC,&pg1_var.mosfetTempTable,&pg1_var.mosfetTempADCTable);
			}
		}

	}
}


ISR(TIMER0_COMPA_vect)
{
//	static int laststate = 0;

//	if ((laststate = 0) and 
	

//    static uint8_t dir = 0;
//    static uint8_t pwm = 0;
//    if (dir == 1)
//    {
//        if(--pwm == 0)
//       {
//			dir = 0;
//			AmpMeasure();
//			PotMeasure();
//
//    	}
//	}	
//    else
//    {
//        if (++pwm == 0xFF)
//		{
//            dir = 1;
//    	}

//	}
 //   OCR0A = 0xF0;
}

//***************************************************************************
//**
//** SCI Communications
//**
//** Communications is established when the PC communications program sends
//** a command character - the particular character sets the mode:
//**
//** "A" = send all of the realtime variables via txport.
//** "V" = send the VE table and constants via txport (128 bytes)
//** "W"+<offset>+<newbyte> = receive new VE or constant byte value and
//**  store in offset location
//** "B" = jump to flash burner routine and burn VE/constant values in RAM into flash
//** "C" = Test communications - echo back SECL
//** "Q" = Send over Embedded Code Revision Number (divide number by 10 - i.e. $21T is rev 2.1)
//**
//***************************************************************************



