#pragma once

#include <GLFW/glfw3.h>
#include <stdbool.h>
#include <stdio.h>

#include <ship.h>
#include <world.h>

void ui_draw(ship_t* ship, world_t* world);

void ui_init();

void ui_string(float x, float y, float scale, char* str);