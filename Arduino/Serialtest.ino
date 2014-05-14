#include <EVReg_serial.h>

/*
  Test att to make Arduino comunicatr with Mergatune tha same way that EVFartreg 1.0
  Atmel version do.
 
 Author Ola Andersson
 
 Created 2012-07-20
 
 */

// Define some structures

// Define user typed input stored in EEPROM.
// It is only one large strukt of data
struct Page1DataStruct {
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
};
// The structure of the realtime data sent on the Serial port to megatune
struct RPageVarsStruct {
    unsigned int seconds,pw1,pw2,rpm;	// pw in usec
    unsigned char	TPSPos;			// TPS Position
    unsigned char	AmpADC;	  		// Peek MosFET Amps.
    unsigned char	BattVoltage;		// Batterypack Voltage
    unsigned char	MosfetDriverVoltage;    // Mosfet driver Voltage.
    unsigned char	MosfetTempADC;		// MosFet Teperatur ADC value.
    unsigned char   MosfetTemp;			// Real Mosfet temp value, looked up in table
};

// These constants won't change.  They're used to give names
// to the pins used:
const int MT_BaudRate = 9600;             // BaudRate for Megatune comunication, def. in bps
const int CodeRev = 0x02;                // Code Revition to send to Megatune.

char IntFired = 0;                      // Has and int fired??
RPageVarsStruct RPageVars;              // Creata RPageVars from RPageVarStruct;
Page1DataStruct pg1_var; 
int RTPS;                    // Real time data page size
int P1DS;                  // Page 1 data size (EEPROM)
int ResevedSerialByte;      // Flag to set if serial byte receved.

struct RPageVarsSruct *RPagePointer = &RPageVars;


void setup() {

  // initialize serial communications at defined bps:
  Serial.begin(MT_BaudRate);   
}
EVRegSerial EVS;
void HandleSerialByte(int SerialByte){

	ResevedSerialByte = 0;								// Reset ResevedSerialByte to 0. Data is stored, and ready for next byte

	switch(SerialByte)								// Evaluate resived data
	{
		case 'A': 
			EVS.Send_Realtimedata();		// If "A" is resived, send Realtimedata
			break;
		case 'C': 
			EVS.Send_Sec(RPageVars.seconds);					// If "C" is resived, send Seconds (comm test)
			break;
		case 'Q': 
			EVS.Send_Rev(CodeRev);							// If "Q" is resived, send code revision.
			break;
//		case 'B': 
//			WriteStructureToEeprom(pg1_ptr);					// If "B" is resived, Write pg1_var to EEPROM.
//			break;
		case 'V': 
			EVS.Send_EpromVar();						// If "V" is resived, send pg1_var.
			break;
//		case 'W': 
//			Get_EpromVar(pg1_ptr,sizeof(pg1_var));					// If "W" is resived, get pg1_var.
//			break;

		default: break;
	}
}
void loop() {

  // read the analog in value:
//  sensorValue = analogRead(analogInPin);            
  // map it to the range of the analog out:
//  outputValue = map(sensorValue, 0, 1023, 0, 255);  
  // change the analog out value:
//  analogWrite(analogOutPin, outputValue);           

  // print the results to the serial monitor:
//  Serial.print("sensor = " );                       
//  Serial.print(sensorValue);      
//  Serial.print("\t output = ");      
//  Serial.println(outputValue);   

  // wait 2 milliseconds before the next loop
  // for the analog-to-digital converter to settle
  // after the last reading:
 // delay(2);                     
}


