// Switch.cpp
// This software to input from switches or buttons
// Runs on TM4C123
// Program written by: put your names here
// Date Created: 3/6/17 
// Last Modified: 1/14/21
// Lab number: 10
// Hardware connections
// TO STUDENTS "REMOVE THIS LINE AND SPECIFY YOUR HARDWARE********

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
// Code files contain the actual implemenation for public functions
// this file also contains an private functions and private data


void Switch_Init(void){
	SYSCTL_RCGCGPIO_R |= 0x00000001; // activate clock for port A
	while((SYSCTL_PRGPIO_R & 0x00000001) == 0){};
	GPIO_PORTA_DIR_R  &= ~0x04;     //  make PA2 in (built-in button)
	GPIO_PORTA_DEN_R  |=  0x04;     //  enable digital I/O on PA2   
	GPIO_PORTA_IS_R   &= ~0x04;     //  PA2 is edge-sensitive
	GPIO_PORTA_IBE_R  |=  0x04;     //  PA2 is both edges
	GPIO_PORTA_ICR_R = 0x04;    // clear flag4
	GPIO_PORTA_IM_R |= 0x04;    // arm interrupt on PA2  
	NVIC_PRI0_R = (NVIC_PRI0_R& 0xFFFFFF1F)| 0x00000060;  // Set Priority Level 3
	NVIC_EN0_R = 0x00000001;    // enable interrupt 30 in NVIC  
}
