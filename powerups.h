// powerups.h
// Runs on TM4C123
// Outlines the powerup utility in the game
// Created: 5/1/2021 
// Laith Altarabishi and John Duncan
// Last modified: 4/25/2021


#ifndef POWERUPS_H
#define POWERUPS_H
#include <stdint.h>
#include "Sprite.h"
#define POWERUPS 10

typedef void (*functions)(Sprite &reflector, Sprite &enemy, Sprite &ball, bool *revMode, bool *noColMode);
class powerups{
	
	public:	
	functions list[POWERUPS];//allocate space for function pointers
	bool powerUpReady; //semaphore flag that describes the state of powers
	void activatePower(); //accesses random powerup function
  powerups(Sprite &reflector, Sprite &enemy, Sprite &ball, bool *revMode, bool *noColMode);
	int randomizer(); //returns a randomized val between 0-99
	int hash(int val); //returns an index based on randomized value
	void powerUpActivate(); //turns on led and and flag
	
};


#endif
