// put implementations for functions, explain how it works
// put your names here, date
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"

#include "DAC.h"


// Port B bits 5-0 have the 6-bit DAC
#include <stdint.h>
#include "..//inc//tm4c123gh6pm.h"

#define DAC   (*((volatile unsigned long *)0x400050FC))

// **************DAC_Init*********************
// Initialize 6-bit DAC 
// Input: none
// Output: none
void DAC_Init(void){
  SYSCTL_RCGCGPIO_R |= 0x02;
	while ((SYSCTL_RCGCGPIO_R & 0x02) == 0){} //init clock and wait
	GPIO_PORTB_DIR_R |= 0x3F; //set outputs
	GPIO_PORTB_DEN_R |= 0x3F;
}


// **************DAC_Out*********************
// output to DAC
// Input: 6-bit data, 0 to 63 
// Output: none
void DAC_Out(uint8_t data){
	GPIO_PORTB_DATA_R = data;
}




