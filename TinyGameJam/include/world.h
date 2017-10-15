#pragma once

#include <GLFW/glfw3.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ship.h>

enum enemy_pattern
{
	enemy_static,
	enemy_ver,
	enemy_hor,
	enemy_circ,
	enemy_sines,
	enemy_intense
};

enum enemy_type
{
	enemy_collider,
	enemy_shooter,
	enemy_fshooter,
	enemy_laser,

};

typedef struct enemy_bullet
{
	float x, y, vx, vy;
	bool active;

} enemy_bullet_t;

typedef struct enemy
{
	// x, y, size, time, width, phase
	float x, y, s, t, w, p;
	float rx, ry;
	int pattern;
	int enemy;
	bool alive;

	enemy_bullet_t bullets[SHIP_BULLETS];

	float timer;

} enemy_t;

#define MAX_ENEMIES 64

enum
{
	powerup_health,
	powerup_mode,
	powerup_superhealth
};

typedef struct powerup
{
	float x, y, vx, vy;
	int type;
	float t;
	bool active;

} powerup_t;

typedef struct explosion
{
	float x, y, s, t;
	bool active;

} explosion_t;

#define WORLD_STARS 128

typedef struct star
{
	float x, y, l;
	float r, g, b;
} star_t;

#define MAX_POWERUPS 32

typedef struct world
{
	int score;
	int level;
	enemy_t enemies[MAX_ENEMIES];
	explosion_t explosions[MAX_ENEMIES/4];
	star_t stars[WORLD_STARS];
	powerup_t powerups[MAX_POWERUPS];

	float hurtTimer;

	int restartTimer;
	float restartSTimer;

	int health;

} world_t;

enemy_t enemy_init(float x, float y, float s, float p, int pattern, int enemy);

void enemy_path(enemy_t* enemy);

int enemy_update(enemy_t* enemy, float dt, ship_t* player, world_t* world);

void enemy_draw(enemy_t* enemy);

world_t* world_init();

void world_update(world_t* world, float dt, ship_t* player);

void world_spawn(world_t* world);

void world_add_enemy(world_t* world, float x, float y, float s, float p, int pattern, int enemy);

void world_add_powerup(world_t* world, float x, float y, float vx, float vy, int type);

void powerup_update(powerup_t* power, float dt, ship_t* player, world_t* world);

void world_draw(world_t* world);

void powerup_draw(powerup_t* power);