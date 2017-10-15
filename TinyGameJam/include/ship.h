#pragma once


#include <GLFW/glfw3.h>
#include <stdbool.h>
#include <stdio.h>

typedef struct bullet
{
	bool active;
	float x, y, vx, vy, w;
	float time;

	float sx, sy;

} bullet_t;

#define SHIP_BULLETS 128

enum ship_mode
{
	ship_simple,
	ship_dual,
	ship_triple,
	ship_laser
};

typedef struct ship
{
	float x, y, w, speed;

	float timer;

	int mode;
	
	bullet_t bullets[SHIP_BULLETS];

} ship_t;

void ship_shoot(ship_t* ship);

ship_t ship_init();

void bullet_draw(bullet_t* bullet);

void bullet_update(bullet_t* bullet, float dt);

bullet_t bullet_init(float x, float y);

void ship_draw(ship_t* ship);

void ship_update(ship_t* ship, float dt, bool up, bool down, bool right, bool left, bool fire);