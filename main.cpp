// main.cpp
// Runs on TM4C123
// Jonathan Valvano and Daniel Valvano
// This is a starter project for the EE319K Lab 10 in C++

// Last Modified: 1/16/2021 
// http://www.spaceinvaders.de/
// sounds at http://www.classicgaming.cc/classics/spaceinvaders/sounds.php
// http://www.classicgaming.cc/classics/spaceinvaders/playguide.php
/* 
 Copyright 2021 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */
// ******* Possible Hardware I/O connections*******************
// Slide pot pin 1 connected to ground
// Slide pot pin 2 connected to PD2/AIN5
// Slide pot pin 3 connected to +3.3V 
// fire button connected to PE0
// special weapon fire button connected to PE1
// 47k  resistor DAC bit 0 on PB0 (least significant bit)
// 24k  resistor DAC bit 1 on PB1
// 12k  resistor DAC bit 2 on PB2
// 6k   resistor DAC bit 3 on PB3 
// 3k   resistor DAC bit 4 on PB4 
// 1.5k resistor DAC bit 5 on PB5 (most significant bit)

// VCC   3.3V power to OLED
// GND   ground
// SCL   PD0 I2C clock (add 1.5k resistor from SCL to 3.3V)
// SDA   PD1 I2C data

//************WARNING***********
// The LaunchPad has PB7 connected to PD1, PB6 connected to PD0
// Option 1) do not use PB7 and PB6
// Option 2) remove 0-ohm resistors R9 R10 on LaunchPad
//******************************

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "TExaS.h"
#include "SSD1306.h"
#include "Random.h"
#include "Switch.h"
#include "Images.h"
#include "SlidePot.h"
#include "Timer0.h"
#include "Timer1.h"
#include "Timer2.h"
#include "Sprite.h"
#include "powerups.h"
//********************************************************************************
// debuging profile, pick up to 7 unused bits and send to Logic Analyzer
#define PA54                  (*((volatile uint32_t *)0x400040C0)) // bits 5-4
#define PF321                 (*((volatile uint32_t *)0x40025038)) // bits 3-1
// use for debugging profile
#define PF1       (*((volatile uint32_t *)0x40025008))
#define PF2       (*((volatile uint32_t *)0x40025010))
#define PF3       (*((volatile uint32_t *)0x40025020))
#define PA5       (*((volatile uint32_t *)0x40004080)) 
#define PA4       (*((volatile uint32_t *)0x40004040)) 
// TExaSdisplay logic analyzer shows 7 bits 0,PA5,PA4,PF3,PF2,PF1,0 
void LogicAnalyzerTask(void){
  UART0_DR_R = 0x80|PF321|PA54; // sends at 10kHz
}
void ScopeTask(void){  // called 10k/sec
  UART0_DR_R = (ADC1_SSFIFO3_R>>4); // send ADC to TExaSdisplay
}
/*void Profile_Init(void){
  SYSCTL_RCGCGPIO_R |= 0x21;      // activate port A,F
  while((SYSCTL_PRGPIO_R&0x20) != 0x20){};
  GPIO_PORTF_DIR_R |=  0x0E;   // output on PF3,2,1 
  GPIO_PORTF_DEN_R |=  0x0E;   // enable digital I/O on PF3,2,1
  GPIO_PORTA_DIR_R |=  0x30;   // output on PA4 PA5
  GPIO_PORTA_DEN_R |=  0x30;   // enable on PA4 PA5  
}*/
void Profile_Init(void){
  SYSCTL_RCGCGPIO_R |= 0x21;      // activate port A,F
  while((SYSCTL_RCGCGPIO_R&0x21) != 0x21){}
  GPIO_PORTF_DIR_R |=  0x0E;   // output on PF3,2,1 
  GPIO_PORTF_DIR_R &= ~0x10;   // input on PF4
  GPIO_PORTF_AFSEL_R &= ~0x10; // disable alt funct
  GPIO_PORTF_DEN_R |=  0x1E;   // enable digital I/O on PF4,3,2,1
  GPIO_PORTF_PCTL_R &= ~0x000F0000;
  GPIO_PORTF_AMSEL_R &= ~0x10; // disable analog funct
  GPIO_PORTF_PUR_R |=     0x10;     // PUR on PF4
  GPIO_PORTF_IS_R &= ~0x10;      //PF0 not edge-sensitive
  GPIO_PORTF_IBE_R &= ~0x10;      //PF0 not both edges
  GPIO_PORTF_IEV_R &= ~0x10;      //PF0 falling edge event
  GPIO_PORTF_ICR_R = 0x10;          //clear flag4
  GPIO_PORTF_IM_R |= 0x10;         //arm interrupt on PF4
  NVIC_PRI7_R = (NVIC_PRI7_R&0xFF00FFFF)|0x00A00000; //priority 5
  NVIC_EN0_R = 0x40000000;          // enable interrupt 30 in NVIC
  GPIO_PORTA_DIR_R &=  ~0xC;   // input on PA2 PA3
  GPIO_PORTA_DEN_R |=  0xC;   // enable on PA2 PA3
	GPIO_PORTA_DEN_R |= 0x10; //enable on PA4
	GPIO_PORTA_DIR_R |= 0x10; //output on PA4
}

//********************************************************************************
 
extern "C" void GPIOF_Handler(void);
extern "C" void GPIOA_Handler(void);
extern "C" void DisableInterrupts(void);
extern "C" void EnableInterrupts(void);
extern "C" void SysTick_Handler(void);
void Delay100ms(uint32_t count); // time delay in 0.1 seconds

#define REFLECTORW 4
#define REFLECTORH 14
#define MAXYVELOCITY 6
#define BALLH 4
#define BALLSPEED -6 //1 pixel per 50 ms
#define AIYSPEED 1 //1 pixel per 50 ms
#define PRECISION 4096
#define BALLS 3

bool reverseMode = false; //dictates the annoyingness of the game
bool English = true;
bool noCollisionsMode = false;


Sprite lasers[25];
Sprite goodGuy(5, 31,REFLECTORH, reflector, false, false);
Sprite bouncyBall(63,31,REFLECTORH, ball, true, false);
Sprite enemyReflector(122-6,31,BALLH, reflector, false, true);
Sprite bouncyBalls[3];
powerups pUps(goodGuy, enemyReflector, bouncyBall, &reverseMode, &noCollisionsMode);

void GPIOF_Handler() {
		DisableInterrupts();
		while ((GPIO_PORTF_DATA_R&0x10) == 0x0) {}
		while ((GPIO_PORTF_DATA_R&0x10) == 0x10) {}
		while ((GPIO_PORTF_DATA_R&0x10) == 0x0) {}
		EnableInterrupts();
    GPIO_PORTF_ICR_R = 0x10;
}

void GPIOA_Handler() {
		DisableInterrupts();
		pUps.activatePower();
		GPIO_PORTA_DATA_R &= ~0x10; //clear LED
		GPIO_PORTA_ICR_R = 0x04;
		EnableInterrupts();
}

void SysTick_Init(unsigned long period){
	NVIC_ST_CTRL_R = 0;
	NVIC_ST_CURRENT_R = 0;
	NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x20000000;
	if (period == 0){
		NVIC_ST_CTRL_R = 0;
		return;
	}
	NVIC_ST_RELOAD_R = period - 1;
  NVIC_ST_CTRL_R = 7; //COUNTER, INTERRUPTS, CLOCK
}


int numLasers = 0;

void Draw(Sprite &p){
	bool drawLeaderboard = true;
	SSD1306_ClearBuffer();
	if(p.round == '5' || p.round == '9') {
		for (int i = 0; i < numLasers; i++) {
			if (lasers[i].life == alive) SSD1306_DrawBMP(lasers[i].x, lasers[i].y, Laser0, 0, SSD1306_WHITE);
			if (lasers[i].y >= 2 && lasers[i].y <= 14) drawLeaderboard = false;
		}
	}
	SSD1306_DrawBMP(p.x, p.y, p.image, 0, SSD1306_WHITE);
	if(bouncyBall.life == alive) SSD1306_DrawBMP(bouncyBall.x, bouncyBall.y, bouncyBall.image, 0, SSD1306_WHITE);
	SSD1306_DrawBMP(enemyReflector.x, enemyReflector.y, enemyReflector.image, 0, SSD1306_WHITE);
	for (int i = 0; i < BALLS; i++){
		if (bouncyBalls[i].life == alive){
			SSD1306_DrawBMP(bouncyBalls[i].x, bouncyBalls[i].y, bouncyBalls[i].image, 0, SSD1306_WHITE);
			if (bouncyBalls[i].y >= 4 && bouncyBalls[i].y <= 16) drawLeaderboard = false;
		}
	}
	if (!(bouncyBall.y >= 4 && bouncyBall.y <= 16 && bouncyBall.life == alive) && drawLeaderboard) {
		SSD1306_DrawBMP(23,12,PongLeaderboard,0,SSD1306_WHITE);
		if(English) SSD1306_DrawString(31,3,"ROUND",SSD1306_WHITE);
		else SSD1306_DrawString(31,3,"NIVEL",SSD1306_WHITE);
		SSD1306_DrawChar(64,3,p.round,SSD1306_WHITE);
		SSD1306_DrawChar(79,3,p.score,SSD1306_WHITE);
		SSD1306_DrawChar(85,3,'v',SSD1306_WHITE);
		SSD1306_DrawChar(91,3,enemyReflector.score,SSD1306_WHITE);
	}
	SSD1306_OutBuffer();  // takes 25ms
	if (goodGuy.score+enemyReflector.score >= bouncyBall.score) {
		Delay100ms(5);
		bouncyBall.score++;
	}
	p.needToDraw = false;       // semaphore
}

int menuPointer = 0;
bool menu = true;

void goToMenu(void) {
	while (menu) {
		while (menuPointer >= 0 && menuPointer < 1340 && menu) {
			if (English) {
				SSD1306_DrawBMP(79,19,ball,0,SSD1306_WHITE);
				SSD1306_DrawBMP(103,29,antiball,0,SSD1306_WHITE);
				SSD1306_DrawBMP(88,39,antiball,0,SSD1306_WHITE);
			}
			else {
				SSD1306_DrawBMP(82,19,ball,0,SSD1306_WHITE);
				SSD1306_DrawBMP(106,29,antiball,0,SSD1306_WHITE);
				SSD1306_DrawBMP(85,39,antiball,0,SSD1306_WHITE);
			}
			SSD1306_OutBuffer();
			if ((GPIO_PORTA_DATA_R&0x4) == 0x0) {
				while((GPIO_PORTA_DATA_R&0x4) == 0x0) {}
				menu = false;
			}
		}
		while (menuPointer >= 1340 && menuPointer < 2680 && menu) {
			if (English) {
				SSD1306_DrawBMP(79,19,antiball,0,SSD1306_WHITE);
				SSD1306_DrawBMP(103,29,ball,0,SSD1306_WHITE);
				SSD1306_DrawBMP(88,39,antiball,0,SSD1306_WHITE);
			}
			else {
				SSD1306_DrawBMP(82,19,antiball,0,SSD1306_WHITE);
				SSD1306_DrawBMP(106,29,ball,0,SSD1306_WHITE);
				SSD1306_DrawBMP(85,39,antiball,0,SSD1306_WHITE);
			}
			SSD1306_OutBuffer();
			if ((GPIO_PORTA_DATA_R&0x4) == 0x0) {
				SSD1306_ClearBuffer();
				if (English) SSD1306_DrawString(28,24,"It's pong...",SSD1306_WHITE);
				else SSD1306_DrawString(34,24,"Es pong...",SSD1306_WHITE);
				SSD1306_OutBuffer();
				Delay100ms(30);
				SSD1306_ClearBuffer();
				if (English) {
					SSD1306_DrawString(52,14,"Play",SSD1306_WHITE);
					SSD1306_DrawString(28,24,"Instructions",SSD1306_WHITE);
					SSD1306_DrawString(43,34,"Spanish",SSD1306_WHITE);
				}
				else {
					SSD1306_DrawString(49,14,"Tocar",SSD1306_WHITE);
					SSD1306_DrawString(25,24,"Instrucciones",SSD1306_WHITE);
					SSD1306_DrawString(46,34,"Ingles",SSD1306_WHITE);
				}
				SSD1306_OutBuffer();
				while((GPIO_PORTA_DATA_R&0x4) == 0x0) {}
			}
		}
		while (menuPointer >= 2680 && menuPointer <= 4096 && menu) {
			if (English) {
				SSD1306_DrawBMP(79,19,antiball,0,SSD1306_WHITE);
				SSD1306_DrawBMP(103,29,antiball,0,SSD1306_WHITE);
				SSD1306_DrawBMP(88,39,ball,0,SSD1306_WHITE);
			}
			else {
				SSD1306_DrawBMP(82,19,antiball,0,SSD1306_WHITE);
				SSD1306_DrawBMP(106,29,antiball,0,SSD1306_WHITE);
				SSD1306_DrawBMP(85,39,ball,0,SSD1306_WHITE);
			}
			SSD1306_OutBuffer();
			if ((GPIO_PORTA_DATA_R&0x4) == 0x0) {
				English = !English;
				SSD1306_ClearBuffer();
				if (English) {
					SSD1306_DrawString(52,14,"Play",SSD1306_WHITE);
					SSD1306_DrawString(28,24,"Instructions",SSD1306_WHITE);
					SSD1306_DrawString(43,34,"Spanish",SSD1306_WHITE);
				}
				else {
					SSD1306_DrawString(49,14,"Tocar",SSD1306_WHITE);
					SSD1306_DrawString(25,24,"Instrucciones",SSD1306_WHITE);
					SSD1306_DrawString(46,34,"Ingles",SSD1306_WHITE);
				}
				SSD1306_OutBuffer();
				while((GPIO_PORTA_DATA_R&0x4) == 0x0) {}
			}
		}
	}
}

void introAnimation(void) {
	SSD1306_ClearBuffer();
  SSD1306_DrawBMP(54, 9, PongScreen1, 0, SSD1306_WHITE);
  SSD1306_OutBuffer();
	ADC_Init();
	Delay100ms(1);
	SSD1306_ClearBuffer();
	SSD1306_DrawBMP(42, 15, PongScreen2, 0, SSD1306_WHITE);
	SSD1306_OutBuffer();
	Delay100ms(1);
	SSD1306_ClearBuffer();
	SSD1306_DrawBMP(29, 21, PongScreen3, 0, SSD1306_WHITE);
	SSD1306_OutBuffer();
	Delay100ms(1);
	SSD1306_ClearBuffer();
	SSD1306_DrawBMP(14, 27, PongScreen4, 0, SSD1306_WHITE);
	SSD1306_OutBuffer();
	Delay100ms(20);
	SSD1306_ClearBuffer();
	SSD1306_DrawBMP(14, 27, PongScreen5, 0, SSD1306_WHITE);
	SSD1306_OutBuffer();
	Delay100ms(4);
	SSD1306_ClearBuffer();
	SSD1306_DrawBMP(14, 27, PongScreen6, 0, SSD1306_WHITE);
	SSD1306_OutBuffer();
	Delay100ms(2);
	SSD1306_ClearBuffer();
	SSD1306_DrawBMP(14, 27, PongScreen7, 0, SSD1306_WHITE);
	SSD1306_OutBuffer();
	Delay100ms(2);
	SSD1306_ClearBuffer();
	SSD1306_DrawBMP(14, 27, PongScreen8, 0, SSD1306_WHITE);
	SSD1306_OutBuffer();
	Delay100ms(2);
	SSD1306_ClearBuffer();
	SSD1306_DrawBMP(14, 27, PongScreen9, 0, SSD1306_WHITE);
	SSD1306_OutBuffer();
	Delay100ms(5);
	SSD1306_DrawString(32,34,"(Gone Wrong)",SSD1306_WHITE);
	SSD1306_OutBuffer();
	Delay100ms(10);
}


int main(void){
	uint32_t time=0;
  DisableInterrupts();
  // pick one of the following three lines, all three set to 80 MHz
  //PLL_Init();                   // 1) call to have no TExaS debugging
  TExaS_Init(&LogicAnalyzerTask); // 2) call to activate logic analyzer
  //TExaS_Init(&ScopeTask);       // or 3) call to activate analog scope PD2
  SSD1306_Init(SSD1306_SWITCHCAPVCC);
  SSD1306_OutClear();
	SysTick_Init(80000*50);
  Profile_Init(); // PA3,PA2,PF3,PF2,PF1 
  Random_Init(1);
	EnableInterrupts();
	introAnimation();
	SSD1306_ClearBuffer();
	if (English) { //draw static menu
		SSD1306_DrawString(52,14,"Play",SSD1306_WHITE);
		SSD1306_DrawString(28,24,"Instructions",SSD1306_WHITE);
		SSD1306_DrawString(43,34,"Spanish",SSD1306_WHITE);
	}
	else {
		SSD1306_DrawString(49,14,"Tocar",SSD1306_WHITE);
		SSD1306_DrawString(25,24,"Instrucciones",SSD1306_WHITE);
		SSD1306_DrawString(46,34,"Ingles",SSD1306_WHITE);
	}
	SSD1306_OutBuffer();
	bool DrawPointer = false;
	goToMenu(); //menu time
	bouncyBall.score = goodGuy.score+enemyReflector.score;
	while(goodGuy.life == alive){
    if (goodGuy.needToDraw){
			Draw(goodGuy);
		}
  }
	SSD1306_OutClear();
	SSD1306_ClearBuffer();
  SSD1306_DrawBMP(2, 62, PongScreen4, 0, SSD1306_INVERSE);
  SSD1306_OutBuffer();
}

// You can't use this timer, it is here for starter code only 
// you must use interrupts to perform delays
int delayCounter = 0;

void delay() {
	delayCounter++;
}

void Delay100ms(uint32_t count){
	delayCounter = 0;
	Timer1_Init(delay,8000000);
  while (delayCounter < count) {}
}



void SysTick_Handler(void){
	if (menu) {
		menuPointer = ADC_In();
	}
	else if (goodGuy.score + enemyReflector.score >= bouncyBall.score) {
		goodGuy.needToDraw = true;
	}
	else if (!menu) {
		uint32_t data = (reverseMode) ? (PRECISION - ADC_In()) : ADC_In();
		goodGuy.move(data);
		if (bouncyBall.life == alive) bouncyBall.move(0);
			enemyReflector.move(-1);
		for (int i = 0; i < BALLS; i++){
		if (bouncyBalls[i].life == alive){
			bouncyBalls[i].move(0);
			}
		}
		if (goodGuy.round == '5' || goodGuy.round == '9'){
			for(int i = 0; i < numLasers; i++) {
				if (lasers[i].life == alive) {
					lasers[i].vx = -10;
					lasers[i].moveLaser();
				}
			}
		}
	}
}

void PeriodicLaserHandler() {
	lasers[0].x = enemyReflector.x - 6;
	lasers[0].y = enemyReflector.y - REFLECTORH/2;
	lasers[0].vx = -10;
	lasers[0].isEnemy = true;
	lasers[0].isProjectile = true;
	lasers[0].length = 2;
	lasers[0].life = alive;
}
//------------------------------implement functions of sprite below-----------------------------------------------

//reset conditions
void Sprite::reset() {
		enemyReflector.length = REFLECTORH;
		enemyReflector.image = reflector;
		enemyReflector.vy = AIYSPEED;
		bouncyBall.ax = 0;
		bouncyBall.vx = -3;
		reverseMode = false;
		for (int i = 0; i < BALLS; i++){
				bouncyBalls[i].life = dead; //spawn 2 balls
				bouncyBalls[i].x = (Random() % 20) + 50;
				bouncyBalls[i].y = (Random() % 60);
				bouncyBalls[i].vy = (Random() % 5);
		}
}


//reset and move on to next round. after round 3, rounds are random in nature...to be implemented
void Sprite::nextRound(){
	goodGuy.score = '0';
	enemyReflector.score = '0';
	bouncyBall.score = goodGuy.score+enemyReflector.score;
	goodGuy.round++; //dictates the stage of the game we are in
	enemyReflector.round++;
	if (enemyReflector.round == '2'){
			enemyReflector.length = 22;
			enemyReflector.image = biggerReflector;
			enemyReflector.vy += 1;
	} else if (enemyReflector.round == '3'){
		  reset();
			enemyReflector.vy += 2;
			bouncyBall.ax = 1;
	} else if (enemyReflector.round == '4'){
			reset();
			reverseMode = true;
			pUps.powerUpActivate(); //enable PA4 powerup interrupt
			for (int i = 0; i < 2; i++){
				bouncyBalls[i].life = alive; //spawn 2 balls
				bouncyBalls[i].x = (Random() % 20) + 50;
				bouncyBalls[i].y = (Random() % 60);
				bouncyBalls[i].vy = (Random() % 5);
				if (Random() % 1 == 1){ //mix up up and down directions as well as x directions
					bouncyBalls[i].vy *= -1;
					bouncyBalls[i].vx *= -1;
				}
			}
			enemyReflector.vy += 2;
			bouncyBall.life = dead; //main ball is out
	} else if (enemyReflector.round == '5') {
			reset();
			pUps.powerUpActivate();
			for (int i = 0; i < BALLS; i++){
				bouncyBalls[i].life = dead;
			}
			enemyReflector.vy += 2;
			numLasers = 1;
			Timer0_Init(PeriodicLaserHandler,80000*50*35);
	} else if (enemyReflector.round == '6') {
	}
}

//point given based on who scored (0 means enemy scored, 1 means goodguy scored)
void Sprite::PointScored(int who) {
    if (who == 0) enemyReflector.score++;
    else if (who == 1) goodGuy.score++;
    if (goodGuy.score == '3' ||enemyReflector.score == '3' ){
			nextRound();
		} else if (enemyReflector.score == '3'){
			//implement losing mechanic/screen
			goodGuy.life = dead;
		}
		if (goodGuy.round != '4' && goodGuy.round != '5'){
			bouncyBall.x = 63;
			bouncyBall.y = 31;
			bouncyBall.vy = 0;
			bouncyBall.vx = 0;
			bouncyBall.life = alive;
			bouncyBall.vx = -6;
		}
		else if (goodGuy.round == '4') {
			for (int i = 0; i < 2; i++){
				bouncyBalls[i].life = alive; //spawn 2 balls
				bouncyBalls[i].x = (Random() % 20) + 50;
				bouncyBalls[i].y = (Random() % 60);
				bouncyBalls[i].vy = (Random() % 5);
				if (Random() % 1 == 1){ //mix up and down directions as well as x directions
					bouncyBalls[i].vy *= -1;
					bouncyBalls[i].vx *= -1;
				}
				} //spawn 2 balls
		}
		else if (goodGuy.round == '5') {
			//TODO: write reset requirements
			bouncyBall.x = 63;
			bouncyBall.y = 31;
			bouncyBall.vy = 0;
			bouncyBall.vx = 0;
			bouncyBall.life = alive;
			bouncyBall.vx = -3;
		}
}

int absDiff(int x, int y){
	int diff = x - y;
	if (diff < 0) diff *= -1;
	return diff;
}

int abs(int x){
	if (x < 0) return x * -1;
	else return x;
}

bool Sprite::collision(Sprite first, Sprite second){
	int f_centerX = first.x + BALLH/2;
	int s_centerX = second.x + BALLH/2;
	int f_centerY = first.y + first.length/2;
	int s_centerY = second.y + second.length/2;
	
	if (absDiff(f_centerX,s_centerX) <= first.length && absDiff(f_centerY, s_centerY) <= BALLH){
		return true;
	}
	return false;
}

void Sprite::wallPhysics(){
		if ((x+vx <= goodGuy.x + REFLECTORW) && ((y - 4 <= goodGuy.y) && (y >= goodGuy.y - REFLECTORH - 1))){
				x = goodGuy.x + REFLECTORW;
				vx = -vx;
				vx += ax;
				if (goodGuy.dir == up){
					vy = goodGuy.lasty - goodGuy.y;
					vy = (MAXYVELOCITY < vy) ? -MAXYVELOCITY : -vy;
				} else if (goodGuy.dir == down){
					vy = goodGuy.y - goodGuy.lasty;
					vy = (MAXYVELOCITY < vy) ? MAXYVELOCITY : vy;
				} else {
					vy = 0;		
				}
			} else if ((x+vx >= enemyReflector.x) && ((y - 4 <= enemyReflector.y) && (y + vy >= enemyReflector.y - enemyReflector.length-1))){
				x = enemyReflector.x - 1;
				vx = -vx;
				vx -= ax;
				if (enemyReflector.dir == up){
					vy = enemyReflector.lasty - enemyReflector.y;
					vy = (MAXYVELOCITY < vy) ? -MAXYVELOCITY : -vy;
				} else if (enemyReflector.dir == down) {
					vy = enemyReflector.y - enemyReflector.lasty;
					vy = (MAXYVELOCITY < vy) ? MAXYVELOCITY : vy;
				} else {
					vy = 0;
				}
			}
}

void Sprite::physics() {
	if (goodGuy.round != '4'){
		wallPhysics();
	} else {
		//account for ball collisions
		for (int i = 0; i < BALLS; i++){ //replace 2 with a var
			if (this != &bouncyBalls[i] && bouncyBalls[i].life == alive){
				if (collision(*this,bouncyBalls[i])){
					int tempx = vx;
					int tempy = vy;
					vx = bouncyBalls[i].vx;
					vy = bouncyBalls[i].vy;
					bouncyBalls[i].vx = tempx;
					bouncyBalls[i].vy =  tempy;
				}
			}
			wallPhysics();
		}
	}
}

Sprite *findClosestBall(int balls){
	Sprite *p = &bouncyBalls[0];
	for (int i = 1; i < balls; i++){
		if (bouncyBalls[i].x > p->x){
			p = &bouncyBalls[i]; 
		}
	}
	return p;
}

//how the reflector makes decisions about movement
void Sprite::logic() {
	if (enemyReflector.round != '4'){
			if (bouncyBall.y + bouncyBall.vy < y-length/2){ //if ball is above reflector
				y -= vy;
				if(y-length <= 2){
					y = length + 2;
				}
			} else if (bouncyBall.y + bouncyBall.vy - 4 > y - length/2){ //if ball is lower than reflector
				y += vy;
				if (y >= 63){
					y = 63;
				}
			} //implement a way for AI to check player's position so it can score on them
		} else {
			Sprite *p = findClosestBall(2);
			if (p->y + p->vy < y-length/2){ //if ball is above reflector
				y -= vy;
				if(y-length <= 2){
					y = length + 2;
				}
			} else if (p->y + p->vy - 4 > y - length/2){ //if ball is lower than reflector
				y += vy;
				if (y >= 63){
					y = 63;
				}
			}
		}
}

	//laser move conditions
void Sprite::moveLaser() {
	if (life == alive && isProjectile && isEnemy) {
		x -= 10;
		needToDraw = true;
		if (x <= 1) {
			life = dead;
		}
	}
	if ((x <= goodGuy.x + REFLECTORW) && (y <= goodGuy.y) && (y > goodGuy.y - REFLECTORH)){
		needToDraw = false;
		enemyReflector.PointScored(0);
	}
}

//initializer for projectiles
Sprite::Sprite(){
	x = 61;
	y = 31;
	vy = 0;
	vx = -3;
	ax = 0;
	ay = 0;
	length = BALLH;
	lasty = 0;
	isProjectile = true;
	life = dead;
	isEnemy = false;
	for(int i = 0; i < 25; i++) if(this == &lasers[i]) isEnemy = true;
	image = ball; //default image - change as necessary
}

//---------------------------powerup function initializations-------------

int powerups::randomizer() {
	return Random() % 100;
}

void powerups::activatePower() {
	if (powerUpReady){
		int index = hash(randomizer());
		(*list[index])(goodGuy, enemyReflector, bouncyBall,&reverseMode, &noCollisionsMode);
		powerUpReady = false;
	}
}

void powerups::powerUpActivate() {
	Switch_Init();
	powerUpReady = true;
	GPIO_PORTA_DATA_R |= 0x10; //LED ON
}

