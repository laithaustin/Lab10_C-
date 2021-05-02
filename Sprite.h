// Sprite.h
// Runs on TM4C123
// Defines class members and functions for sprites
// Created: 4/25/2021 
// Laith Altarabishi and John Duncan
// Last modified: 4/25/2021


#ifndef SPRITE_H
#define SPRITE_H
#include <stdint.h>
#include "Images.h"

#define rHeight 14
#define bHeight 4

typedef enum {dead, alive} status_t;
typedef enum {up, down, fix} direction_t;
class Sprite{ 
	public:
		int x;
		int vx;
		int y; 
		int vy;
		int ax;
		int ay;
		int length;
		char score;
		char round;
		int lasty;
		direction_t dir;
		const uint8_t *image;
		status_t life;
		bool pUpsEnabled;
		bool isProjectile;
		bool isEnemy;
		bool needToDraw; //asmaphore
	
	public:
		Sprite(int x, int y, int length, const uint8_t *image, bool type, bool isEnemy); //constructor
		Sprite();
		void move(int data); //describes movement for every 50 ms interrupt
		void PointScored(int who);
		void nextRound();
		void physics(); //physics for ball collisions and kinematics
		void wallPhysics(); //interactions specific to ball to wall action
		void logic(); //reasoning for enemy AI
		void draw(); //our means of outputting to display
		void reset(); //reset conditions of program
		bool collision(Sprite first, Sprite second);
	
};



#endif
