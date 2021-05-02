// powerups.cpp
// Runs on TM4C123
// Provide functions for Sprite Class
// Last Modified: 1/16/2021 
// Student names: Laith Altarabishi and John Duncan

#include "powerups.h"
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"



void superBalls(Sprite &reflector, Sprite &enemy, Sprite &ball, bool *revMode, bool *noColMode){
	ball.vx *= 2;
	ball.vy *= 2;
}

void invisibleBall(Sprite &reflector, Sprite &enemy, Sprite &ball, bool *revMode, bool *noColMode){
	ball.image = antiball;
}

void intangibleBall(Sprite &reflector, Sprite &enemy, Sprite &ball, bool *revMode, bool *noColMode){
	*noColMode = true;
}

void death(Sprite &reflector, Sprite &enemy, Sprite &ball, bool *revMode, bool *noColMode){
	reflector.life = dead; //implement premature death screen
}

void slowAI(Sprite &reflector, Sprite &enemy, Sprite &ball, bool *revMode, bool *noColMode){
	enemy.vy = 1;
}

void revControls(Sprite &reflector, Sprite &enemy, Sprite &ball, bool *revMode, bool *noColMode){
	*revMode = true;
}

void expand(Sprite &reflector, Sprite &enemy, Sprite &ball, bool *revMode, bool *noColMode){
	
}

void shrink(Sprite &reflector, Sprite &enemy, Sprite &ball, bool *revMode, bool *noColMode){

}

void maxSize(Sprite &reflector, Sprite &enemy, Sprite &ball, bool *revMode, bool *noColMode){

}

void freezeAI(Sprite &reflector, Sprite &enemy, Sprite &ball, bool *revMode, bool *noColMode){
	enemy.vy = 0;
}

powerups::powerups(Sprite &reflector, Sprite &enemy, Sprite &ball, bool *revMode, bool *noColMode) {
	powerUpReady = false;
	list[0] = *superBalls;
	list[1] = *invisibleBall;
	list[2] = *intangibleBall;
	list[3] = *death;
	list[4] = *slowAI;
	list[5] = *revControls;
	list[6] = *expand;
	list[7] = *shrink;
	list[8] = *maxSize;
	list[9] = *freezeAI;
}

int powerups::hash (int val){
	if (val >= 0 && val <= 24){ //superball probility
		return 0;
	} else if (val >= 25 && val <= 29){ //invisible ball
		return 1;
	} else if (val >= 30 && val <= 34) {//intangibile ball
		return 2;
	} else if (val >= 35 && val <= 39) { //death
		return 3;
	} else if (val >= 40 && val <= 44) { //slow ai
		return 4;
	} else if (val >= 45 && val <= 54) { //rev controls
		return 5;
	} else if (val >= 55 && val <= 64) { //expand
		return 6;
	} else if (val >= 65 && val <= 74) { //shrink
		return 7;
	} else if (val >= 75 && val <= 79) { //maxSize
		return 8;
	} else { //freeze AI
		return 9;
	}
		
}



