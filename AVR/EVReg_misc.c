// Copyed from MS_extar
// Locks up and returns real value for a adc-value in the temp_tables.
// Fixed 12 point chare tables are used.
// param1, adc_value 		  = adc-value to lookup real value-temp.
// param2, temp_table		= Table to lock up the temp value.
// param3, adc_table		= Table to lock up the ADC value.
//
// Returns real temp value for adc_value


// temp_table = pg1_ptr. + 40;
// adc_table = pg1_ptr + 28;

#include "EVReg.h"

char GetMosfetTemp(int adc_value, unsigned char *temp_table, unsigned char *adc_table)  {
  int ix;
  int NO_TEMPS;
  long interp, interp3;
  // returns values for various cold warmup temp_table interpolations
  // bound input arguments
  NO_TEMPS = 12;
//  if(adc_value > adc_table[NO_TEMPS-1])  {
//    return(temp_table[NO_TEMPS -1]);
//  }
//  if(adc_value < adc_table[0])  {
//    return(temp_table[0]);
//  }
  for(ix = NO_TEMPS - 2; ix > -1; ix--)  {
  	if(adc_value > adc_table[ix])  {
   		break;
  	}
  }
  if(ix < 0)ix = 0;

  interp = adc_table[ix + 1] - adc_table[ix];
  if(interp != 0)  {
    interp3 = (adc_value - adc_table[ix]);
    interp3 = (100 * interp3);
    interp = interp3 / interp;
  }
  return((temp_table[ix] + interp * (temp_table[ix+1] - temp_table[ix])/ 100));
}
