#include <avr/io.h>
#include "gpio.h"
#include <avr/interrupt.h>

#define    PWM_cfg   D,   6,   6		// Config för PWM port, PortD pin 6

/* Macro for testing of a single bit in an I/O location*/
#define CHECKBIT(ADDRESS,BIT) (ADDRESS & (1<<BIT))
#define SETBIT(ADDRESS,BIT) (ADDRESS |= (1<<BIT))
#define CLEARBIT(ADDRESS,BIT) (ADDRESS &= ~(1<<BIT))

unsigned char   temp;
unsigned char	AmpAverageNextPos;
unsigned char	AmpAverageBuffer[10]={0,0,0,0,0,0,0,0,0,0};

int AmpAverage()
	{
	int total=0;
	int i;
	for (i=0; i<10; i++)
		{
		total+=AmpAverageBuffer[i];
		}
	return(total/10);
	}
	
void AddAmpSamples(unsigned char sample)
	{
	AmpAverageBuffer[AmpAverageNextPos] = sample;
	if (++AmpAverageNextPos == 10)
		{
		AmpAverageNextPos = 0;
		}
	}
int PotMeasure()
{

  	while((ADCSRA & (1<<ADSC))!=0) ; 		// Wait until ADC is finiched
//	ADMUX = (1<<REFS1)|(1<<REFS0)|(1<<MUX2);//  Select ADC4, AVCC ref, Voltage (tmp, now pot)
	ADMUX = (1<<REFS0)|(1<<MUX2);//  Select ADC4, AVCC ref, Voltage (tmp, now pot)
	ADCSRA |= (1<<ADSC)|(1<<ADEN);		// Enable and start ADC-conversion
    while((ADCSRA & (1<<ADSC))!=0) ; 	// Wait until ADC is finiched
		temp = (ADC/4);					// Copier TPS pos till temp.
										// Dela med 4 för att convertera 10bit till 8.
	return(temp);
}

int AmpMeasure(int pw)
{	
    while((ADCSRA & (1<<ADSC))!=0) ; 		// Wait until ADC is finiched

	ADMUX = (1<<REFS0)|(1<<MUX0);			//  Select ADC1, AVCC ref, Amp
 	
//	if  (pw > 0)  {						// I PW is under 100%, wait for next cycle
//	while (!gpio_rd8( PIN, PWM_cfg ));		// Wait until Pin is lo PD6
//	while (gpio_rd8( PIN, PWM_cfg ));		// Wait until Pin is high PD6
//	}
//	if  (pw > 0) and (pw < 255) {			// I PW is less then 100% and more than 0%, wait for next cycle
//	while (!gpio_rd8( PIN, PWM_cfg ));		// Wait until Pin is lo PD6
//	while (gpio_rd8( PIN, PWM_cfg ));		// Wait until Pin is high PD6
//	}
//	Removed to stop hanging in the this rutine


	SETBIT(PORTD,PD7);					// Set PD7 high, to be able to measure ADC-time
	ADCSRA |= (1<<ADSC)|(1<<ADEN);		// Enable and start ADC-conversion
    while((ADCSRA & (1<<ADSC))!=0) ; 	// Wait until ADC is finiched
		// RPage.peekAmp = ADC/4;			// Uppdatera RealtimeData med peekAmp.
	CLEARBIT(PORTD,PD7);				// Set PD7 low, to be able to measure ADC-time
	temp = ADC/4;						// Uppdatera temp med peekAmp/4 
	return(temp);						// Returnera värdet istället för att uppdatera RealtimeData
}	

int B_VoltMeasure()
{
    while((ADCSRA & (1<<ADSC))!=0) ; 		// Wait until ADC is finiched
	ADMUX = (0<<REFS1)|(1<<REFS0);		// AVCC ref, Voltage (Battery Voltage)
	ADCSRA |= (1<<ADSC)|(1<<ADEN);		// Enable and start ADC-conversion
    while((ADCSRA & (1<<ADSC))!=0) ; 	// Wait until ADC is finiched
		temp = ADC/4;					// Copier TPS pos till temp.
	return(temp);
}

int MOS_TempMeasure()
{
	while((ADCSRA & (1<<ADSC))!=0) ; 		// Wait until ADC is finiched
	ADMUX = (0<<REFS1)|(1<<REFS0|(1<<MUX0)|(1<<MUX1));	//  Select ADC3, AVCC ref, Voltage (tmp)
	ADCSRA |= (1<<ADSC)|(1<<ADEN);		// Enable and start ADC-conversion
    while((ADCSRA & (1<<ADSC))!=0) ; 	// Wait until ADC is finiched
		temp = ADC/4;					// Copier TPS pos till temp.
		temp = 255 - temp;				// Makes higer value corespond to higer temp.
//		temp = 64; 					    // Untill tested, just copy "64" Will display 32 degrees in MT
    return(temp);
}
