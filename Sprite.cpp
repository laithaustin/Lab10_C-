// Sprite.cpp
// Runs on TM4C123
// Provide functions for Sprite Class
// Last Modified: 1/16/2021 
// Student names: Laith Altarabishi and John Duncan

#include "Sprite.h"
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#define BALLSPEED -6 //1 pixel per 50 ms
#define AIYSPEED 1 //1 pixel per 50 ms


void Sprite::move(int data) {
		//reflector move conditions
		if (life == alive && !isProjectile && data >= 0){
			lasty = y;
			y = (data*(63+8+22))/4096; //compute y
			//set direction
			if (y > lasty) dir = down;
			else if (y < lasty) dir = up;
			else dir = fix;
			//edge conditions
			if (y < length + 2) y = length + 2;
			else if (y > 62) y = 62;
			needToDraw = true;
		}
		//ball move conditions
		else if (life == alive && isProjectile && !isEnemy) {
				physics();
				x += vx;
				y += vy;
				//edge conditions
				if (y < 4) {
						y = 4;
						vy = -vy;
				}
					
				else if (y > 62) {
					y = 62;
					vy = -vy;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                
				}
				if ((x >= 122 || x <= 1) && !isEnemy){  //death condition if ball goes out of bounds
					this->life = dead;
					int who = 0;
					if (x >= 122) who = 1;
					PointScored(who);
        }
				needToDraw = true;
		}
		//enemy reflector conditions
		else if (life == alive && data <0){
			//if (round == 1){
			logic();
			if (round == 6) 
			needToDraw = true;
		}
}


//init sprite
Sprite::Sprite(int x1, int y1, int l, const uint8_t* p, bool objType, bool isEnemy){
	 x = x1;
	 y = y1;
	 ax = 0;
	 ay = 0;
	 length = l;
	 vx = 0;
	 score = '0';
	 round = '1';
	 vy = 0;
	 lasty = 0;
	 dir = fix;
	 needToDraw = true;
	 life = alive;
	 image = p;
	 isProjectile = objType; //check if it's a projectile
	 if (isProjectile){
			vx = BALLSPEED; //INITIALLY WILL MOVE HORIZONTALLY
	 }
	 if (isEnemy){
			vy = AIYSPEED;
	 }
}


