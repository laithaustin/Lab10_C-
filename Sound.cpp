// Sound.cpp

// Sound assets based off the original Space Invaders 
// These arrays create classic sounds!
// Jonathan Valvano
// 1/16/2021=
#include "../inc/tm4c123gh6pm.h"
#include <stdint.h>
#include "Sound.h"
#include "DAC.h"
#include "Timer2.h"
// these are sampled at 8 bits 11kHz
// If your DAC is less than 8 bits you will need to scale the data

void TimerISR(void);
// initialize a 11kHz timer, flags, pointers, counters,and the DAC
void Sound_Init(void){
	DAC_Init();
	Timer2_Init(&TimerISR,7273);
  TIMER2_CTL_R = 0x00000000;    // disable TIMER1A
};

//******* Sound_Start ************
// This function does not output to the DAC. 
// Rather, it sets a pointer and counter, and then enables the timer interrupt.
// It starts the sound, and the timer ISR does the output
// feel free to change the parameters
// Input: pt is a pointer to an array of DAC outputs
//        count is the length of the array
// Output: none
// special cases: as you wish to implement
const uint8_t *sounds;
uint32_t counter = 0;
uint32_t max;

void Sound_Start(const uint8_t *pt, uint32_t count){
	counter = 0;
	max = count;
	sounds = pt;
  TIMER2_CTL_R = 0x00000001;
};
// 11 kHz timer ISR
void TimerISR(void) {
	if (sounds == shoot) DAC_Out(sounds[counter]>>2);
	else DAC_Out(sounds[counter]);
	counter++;
	if (counter >= max) TIMER2_CTL_R = 0x0;
}
// 
// timer ISR sends one point to DAC each interrupt
// Because TExas sends data to TExasDisplay at 10kHz, you will not be able to toggle a profile pin
// However, you can activate TExaS in analog scope mode and connect PD2 to the DAC output to visualize the sound wave
