#include <avr/io.h>
#include "gpio.h"
#include <avr/interrupt.h>
#include "EVReg.h"

// char	sendbyte;
extern int ResevedSerialByte;

int SendSerialByte(int l_sendbyte)
{
while ( !( UCSR0A & (1<<UDRE0)) );		// Vänta tills det inte finns någonting i bufferten
UDR0 = l_sendbyte;						// Skicka data
return(0);
}

int Send_Sec(int l_sec)
{
	SendSerialByte(l_sec);
//	RPage.seconds++;
	return(l_sec++);
}
int Send_Rev(int l_CodeRev)
{
	SendSerialByte(l_CodeRev);
	return(0);
}
int Send_Realtimedata(char *l_ptr)
{
	int i;
	char *temppointer;
	for (i=0; i < RTPS; i++)					// i = 0 till RealTimePageSize
	{
		temppointer = l_ptr + i;			// temppointer pekar på nästa position i RPagePointer
		SendSerialByte(*temppointer);			// Skicka värdet på den positionen
//		SendSerialByte( *(char*)( RPagePointer + i) ); // Skicka värdet i (char pointer casted) RPagePointer +1.
	}
return(0);
}
int Send_EpromVar(char *l_ptr)
{
	int i;
	char *temppointer;
	for (i=0; i < sizeof(pg1_var); i++)			// i = 0 till page1size
	{
		temppointer = l_ptr + i;				// temppointer pekar på nästa position i pg1pointer
		SendSerialByte(*temppointer);			// Skicka värdet på den positionen
	}
return(0);
}
int Get_EpromVar(char *l_ptr)
{
	int i,j;
	char *temppointer;
	for (i=0; i < 6000; i++)					// Wait for next byte, this byte is the offset in eeprom (timeout = ?? 64000 tic ??)
	if (ResevedSerialByte == 1)					// ResevedSerialByte is sett when we have somting in the reseve buffer
	{
		temppointer = l_ptr + UDR0;				// Point temppointer to correct plase in page1_var (Variable pointer + offsett)
		ResevedSerialByte = 0;					// Mark that we have got the byte.
		for (j=0; j < 6000; j++)				// Wait for next byte, this byte is the value for the just reseved offset (timeout = ?? 64000 tic ??)
		if (ResevedSerialByte == 1)				// ResevedSerialByte is sett when we have somting in the reseve buffer
		{
			*temppointer = UDR0;					// Put the value in the position that temppointer points to.
			ResevedSerialByte = 0;				// Mark that we have got the byte.
			j = 6001;							// breake inner loop if byte is resived before timeout
		}
		i = 6001;								// breake loop if byte is resived before timeout
	}
return(0);
}
