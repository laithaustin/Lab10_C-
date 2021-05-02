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
	SYSCTL_RCGCGPIO_R |= 0x00000020; // activate clock for port F
	while((SYSCTL_PRGPIO_R & 0x00000020) == 0){};
	GPIO_PORTF_DIR_R  &= ~0x10;     //  make PF4 in (built-in button)
	GPIO_PORTF_DEN_R  |=  0x10;     //  enable digital I/O on PF4   
	GPIO_PORTF_PUR_R  |=  0x10;     //  enable weak pull-up on PF4
	GPIO_PORTF_IS_R   &= ~0x10;     //  PF4 is edge-sensitive
	GPIO_PORTF_IBE_R  |=  0x10;     //  PF4 is both edges
	GPIO_PORTF_ICR_R = 0x10;    // clear flag4
	GPIO_PORTF_IM_R |= 0x10;    // arm interrupt on PF4  
	NVIC_PRI7_R = (NVIC_PRI7_R& 0xFF00FFFF)| 0x00A00000;  // Set Priority Level
	NVIC_EN0_R = 0x40000000;    // enable interrupt 30 in NVIC  
}
