#ifndef _EVReg_h
#define _EVReg_h

#include <avr/eeprom.h>

#define EEPROM_SECTION  __attribute__ ((section (".eeprom")))

// Define user typed input stored in EEPROM.
// It is only one large strukt of data
// page1struct variable is created
typedef struct{

	int TimeAmpTabel[8];
	int mMaxAmp1Sec;
	int mMaxAmp5Sec;
	int mMaxAmp1Min;
	int mMaxAmp10Min;
	int mMaxAmpConst;
	int mMaxTemp;
	unsigned char mosfetTempADCTable[12];
	unsigned char mosfetTempTable[12];
	int	accDelay;

} page1data;

// page1data is declared in mail file
extern const page1data eeprom_page1 EEPROM_SECTION;
extern page1data *pg1_ptr;

// Copy of values in ram
extern page1data pg1_var;


// The structure of the realtime data sent on the Cerial port to megatune
// Variable RPage is created.
typedef struct RPageVarsSruct {
unsigned 	int seconds,pw1,pw2,rpm;	// pw in usec
			unsigned char	TPSPos;				// TPS Position
			unsigned char	AmpADC;	  			// Peek MosFET Amps.
			unsigned char	BattVoltage;		// Batterypack Voltage
			unsigned char	MosfetDriverVoltage;// Mosfet driver Voltage.
			unsigned char	MosfetTempADC;		// MosFet Teperatur ADC value.
			unsigned char   MosfetTemp;			// Real Mosfet temp value, looked up in table


} RPageVars;


extern char RTPS;							// RealTimePageSize
// char *RPagePointer = &RPage;
extern RPageVars RPage;
extern struct RPageVarsSruct *RPagePointer;

extern const int BAUDRATE;				//set desired baud rate

#define UBRRVAL F_CPU/16/BAUDRATE-1 	//calculate UBRR value

extern const int CodeRev;				// Set CodeRev to match in Megatune

#define TBUFSIZE	32					// Sätt Serieportens sändningsbuffert till 32
#define RBUFSIZE	32					// Sätt Serieportens mottagarbuffert till 32

#define TMASK		(TBUFSIZE-1)
#define RMASK		(RBUFSIZE-1)


// rs232 Outputs to pc

volatile unsigned char tbuf[TBUFSIZE];	// reservera Sändbuffert
volatile unsigned char rbuf[RBUFSIZE];	// reservera mottagarbuffert

volatile unsigned char t_in;
volatile unsigned char t_out;

volatile unsigned char r_in;
volatile unsigned char r_out;

int ResevedSerialByte;


#endif
