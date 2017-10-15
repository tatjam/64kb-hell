#include <world.h>

enemy_t enemy_init(float x, float y, float s, float p, int pattern, int enemy)
{
	enemy_t out;

	out.x = x;
	out.y = y;
	out.s = s;
	out.p = p;
	out.rx = 0.0f;
	out.ry = 0.0f;
	out.t = 0.0f;
	out.pattern = pattern;
	out.enemy = enemy;
	out.w = 0.1f;
	out.alive = true;

	for (size_t i = 0; i < SHIP_BULLETS; i++)
	{
		out.bullets[i].active = false;
		out.bullets[i].x = 0;
		out.bullets[i].y = 0;
	}

	out.timer = 0.0f;

	return out;
}

void enemy_path(enemy_t* enemy)
{
	if (enemy->pattern == enemy_static)
	{
		enemy->rx = enemy->x;
		enemy->ry = enemy->y;
	}
	else if(enemy->pattern == enemy_hor)
	{
		enemy->rx = sinf(enemy->t + enemy->p) * enemy->s + enemy->x;
		enemy->ry = enemy->y;
	}
	else if (enemy->pattern == enemy_ver)
	{
		enemy->rx = enemy->x;
		enemy->ry = sinf(enemy->t + enemy->p) * enemy->s + enemy->y;
	}
	else if (enemy->pattern == enemy_circ)
	{
		enemy->rx = sinf(enemy->t + enemy->p) * enemy->s + enemy->x;
		enemy->ry = cosf(enemy->t + enemy->p) * enemy->s + enemy->y;
	}
	else if (enemy->pattern == enemy_sines)
	{
		enemy->rx = sinf(enemy->t + enemy->p) * enemy->s + enemy->x;
		enemy->ry = sinf(enemy->t * 2 + enemy->p) * enemy->s + enemy->y;
	}
	else if (enemy->pattern == enemy_intense)
	{
		enemy->rx = sinf(enemy->t * 2 + enemy->p) * enemy->s + enemy->x;
		enemy->ry = sinf(enemy->t * 3 + enemy->p) * enemy->s + enemy->y;
	}
}

void world_add_powerup(world_t* world, float x, float y, float vx, float vy, int type)
{
	for (size_t i = 0; i < MAX_POWERUPS; i++)
	{
		if (world->powerups[i].active == false)
		{
			world->powerups[i].active = true;
			world->powerups[i].x = x;
			world->powerups[i].y = y;
			world->powerups[i].vx = vx;
			world->powerups[i].vy = vy;
			world->powerups[i].type = type;
			break;
		}
	}
}

int enemy_update(enemy_t* enemy, float dt, ship_t* player, world_t* world)
{
	if (enemy->alive)
	{
		enemy->t += dt;

		enemy_path(enemy);

		// Check bullets and stuff
		for (size_t i = 0; i < SHIP_BULLETS; i++)
		{
			if (player->bullets[i].active)
			{
				// Check distance
				float dist = sqrtf(powf(player->bullets[i].x - enemy->rx, 2)
					+ powf(player->bullets[i].y - enemy->ry, 2));

				if (dist <= enemy->w * 1.2f)
				{
					player->bullets[i].active = false;

					for (size_t j = 0; j < MAX_ENEMIES / 4; j++)
					{
						if (world->explosions[j].active == false)
						{
							world->explosions[j].active = true;
							world->explosions[j].x = enemy->rx;
							world->explosions[j].y = enemy->ry;
							world->explosions[j].t = 0.0f;

							int chance = 780;

							// Increace chances for weak players
							if (player->mode == ship_simple)
							{
								chance -= 80;
							}

							if (world->health < 5)
							{
								chance -= 100;
							}

							world->score += (enemy->enemy + 1) * 5;

							// Powerup?
							if (rand() % 1000 > chance)
							{
								float nvx = 0.0f, nvy = 0.0f;

								nvx = (float)rand() / RAND_MAX*2.0 - 1.0;
								nvy = (float)rand() / RAND_MAX*2.0 - 1.0;

								nvx *= 0.2f;
								nvy *= 0.2f;

								if (rand() % 1000 > 700)
								{
									// Upgrade
									world_add_powerup(world, enemy->rx, enemy->ry, nvx, nvy, powerup_mode);
								}
								else
								{
									// Health
									if (rand() % 1000 > 850)
									{
										// Superhealth
										world_add_powerup(world, enemy->rx, enemy->ry, nvx, nvy, powerup_superhealth);
									}
									else
									{
										// Normal health
										world_add_powerup(world, enemy->rx, enemy->ry, nvx, nvy, powerup_health);
									}
								}
							}

							break;
						}
					}

					enemy->alive = false;
					break;
				}
			}
		}
	}

	// Attack
	if (enemy->enemy == enemy_collider && enemy->alive)
	{

	}
	else
	{
		// Bullets
		for (size_t i = 0; i < SHIP_BULLETS; i++)
		{
			if (enemy->bullets[i].active)
			{
				enemy->bullets[i].x += enemy->bullets[i].vx * dt;
				enemy->bullets[i].y += enemy->bullets[i].vy * dt;

				if (enemy->bullets[i].x < -1 || enemy->bullets[i].x > 1 ||
					enemy->bullets[i].y < -1 || enemy->bullets[i].y > 1)
				{
					enemy->bullets[i].active = false;
				}

				float dist = sqrtf(powf(enemy->bullets[i].x - player->x, 2)
					+ powf(enemy->bullets[i].y - player->y, 2));

				if (dist < player->w * 0.8f)
				{
					enemy->bullets[i].active = false;
					world->health--;
					world->hurtTimer = 1.0f;
				}
			}
		}

		// Shoot 

		if (enemy->timer < 0.0f)
		{
			if (enemy->alive)
			{
				for (size_t i = 0; i < SHIP_BULLETS; i++)
				{
					if (enemy->bullets[i].active == false)
					{
						enemy->bullets[i].x = enemy->rx;
						enemy->bullets[i].y = enemy->ry;
						enemy->bullets[i].vx = -(enemy->rx - player->x);
						enemy->bullets[i].vy = -(enemy->ry - player->y);
						enemy->bullets[i].active = true;
						break;
					}
				}
			}

			if (enemy->alive)
			{
			
				if (enemy->enemy == enemy_shooter)
				{
					enemy->timer = 1.0f;
				}
				else if (enemy->enemy == enemy_fshooter)
				{
					enemy->timer = 0.5f;
				}
				else if (enemy->enemy == enemy_laser)
				{
					if (rand() % 1000 > 940)
					{
						enemy->timer = 1.5f;
					}
					else
					{
						enemy->timer = 0.1f;
					}
				}
			}
			else
			{
				enemy->timer = 10000.0f;
			}
		}

		enemy->timer -= dt;
	}

	if (enemy->alive)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

void powerup_update(powerup_t* power, float dt, ship_t* player, world_t* world)
{
	if (power->active)
	{
		power->t += dt * 5;
		power->x += power->vx * dt;
		power->y += power->vy * dt;

		if (power->x < -1.0 || power->x > 1.0 || power->y < -1.0 || power->y > 1.0)
		{
			power->active = false;
		}

		float dist = sqrtf(powf(power->x - player->x, 2)
			+ powf(power->y - player->y, 2));

		if (dist < 0.15f)
		{
			if (power->type == powerup_mode)
			{
				player->mode++;
				if (player->mode > ship_laser)
				{
					player->mode = ship_laser;
				}
			}
			else if (power->type == powerup_health)
			{
				world->health++;
				if (world->health > 10)
				{
					world->health = 10;
				}
			}
			else if (power->type == powerup_superhealth)
			{
				world->health+=5;
				if (world->health > 10)
				{
					world->health = 10;
				}
			}
			power->active = false;
		}
	}
}

void enemy_draw(enemy_t* enemy)
{
	for (size_t i = 0; i < SHIP_BULLETS; i++)
	{
		if (enemy->bullets[i].active)
		{

			glColor3f(1.0f, 0.8f, 0.3f);

			glBegin(GL_QUADS);
			{
				// Top left
				glVertex3f(enemy->bullets[i].x, enemy->bullets[i].y, 1.0f);

				// Bottom left
				glVertex3f(enemy->bullets[i].x, enemy->bullets[i].y - 0.02f, 1.0f);

				// Bottom right
				glVertex3f(enemy->bullets[i].x + 0.02f, enemy->bullets[i].y - 0.02f, 1.0f);

				// Top right
				glVertex3f(enemy->bullets[i].x + 0.02f, enemy->bullets[i].y, 1.0f);
			}
			glEnd();
		}
	}

	if (enemy->alive)
	{

		float c = enemy->timer;
		c *= c;
		if (c > 1.0f) { c = 1.0f; }

		glColor3f((1.0f - (c)) + 0.2f, (0.5f - c - 0.2f) + 0.1f, c * 0.3f);

		if (enemy->enemy == enemy_collider)
		{
			glBegin(GL_TRIANGLES);
			{
				// Top vertex, at ship position + 1/2 width
				glVertex3f(enemy->rx + enemy->w / 2, enemy->ry, 1.0f);

				// Bottom left
				glVertex3f(enemy->rx, enemy->ry - enemy->w, 1.0f);

				// Bottom right
				glVertex3f(enemy->rx + enemy->w, enemy->ry - enemy->w, 1.0f);
			}
			glEnd();
		}
		else if (enemy->enemy == enemy_shooter)
		{
			glBegin(GL_TRIANGLES);
			{
				// Top vertex, at ship position + 1/2 width
				glVertex3f(enemy->rx + enemy->w / 2, enemy->ry, 1.0f);

				// Bottom left
				glVertex3f(enemy->rx, enemy->ry - enemy->w, 1.0f);

				// Bottom right
				glVertex3f(enemy->rx + enemy->w, enemy->ry - enemy->w, 1.0f);
			}
			glEnd();

			glBegin(GL_TRIANGLES);
			{
				// Bottom vertex, at ship position + 1/2 width
				glVertex3f(enemy->rx + enemy->w / 2, enemy->ry - enemy->w, 1.0f);

				// Bottom left
				glVertex3f(enemy->rx, enemy->ry, 1.0f);

				// Bottom right
				glVertex3f(enemy->rx + enemy->w, enemy->ry, 1.0f);
			}
			glEnd();
		}
		else if (enemy->enemy == enemy_fshooter)
		{
			glBegin(GL_TRIANGLES);
			{
				// Top vertex, at ship position + 1/2 width
				glVertex3f(enemy->rx + enemy->w / 2, enemy->ry, 1.0f);

				// Bottom left
				glVertex3f(enemy->rx, enemy->ry - enemy->w, 1.0f);

				// Bottom right
				glVertex3f(enemy->rx + enemy->w, enemy->ry - enemy->w, 1.0f);
			}
			glEnd();

			glBegin(GL_TRIANGLES);
			{
				// Bottom vertex, at ship position + 1/2 width
				glVertex3f(enemy->rx + enemy->w / 2, enemy->ry - enemy->w, 1.0f);

				// Bottom left
				glVertex3f(enemy->rx, enemy->ry + 0.05f, 1.0f);

				// Bottom right
				glVertex3f(enemy->rx + enemy->w, enemy->ry + 0.05f, 1.0f);
			}
			glEnd();
		}
		else if (enemy->enemy == enemy_laser)
		{
			glBegin(GL_QUADS);
			{
				// Top left
				glVertex3f(enemy->rx, enemy->ry - enemy->w, 1.0f);


				// Bottom left
				glVertex3f(enemy->rx, enemy->ry + 0.05f, 1.0f);


				// Bottom right
				glVertex3f(enemy->rx + enemy->w, enemy->ry + 0.05f, 1.0f);

				// Top right
				glVertex3f(enemy->rx + enemy->w, enemy->ry, 1.0f);




			}
			glEnd();

			glBegin(GL_LINES);
			{
				glVertex3f(enemy->rx + enemy->w, enemy->ry - enemy->w / 2, 1.0f);
				glVertex3f(enemy->rx + enemy->w + 0.15f, enemy->ry - enemy->w / 2, 1.0f);
			}
			glEnd();

			glBegin(GL_LINES);
			{
				glVertex3f(enemy->rx + enemy->w, enemy->ry - enemy->w / 2, 1.0f);
				glVertex3f(enemy->rx + enemy->w - 0.05f, enemy->ry - enemy->w / 3, 1.0f);
			}
			glEnd();

		}
	}
}

void world_update(world_t* world, float dt, ship_t* player)
{

	world->hurtTimer -= dt * 2;
	world->restartSTimer -= dt;
	if (world->restartSTimer < 0.0f)
	{
		world->restartSTimer = 1.0f;
		if (world->restartTimer != -1)
		{
			world->restartTimer--;
		}
	}

	size_t count = 0;

	for (size_t i = 0; i < MAX_ENEMIES; i++)
	{
		count += enemy_update(&world->enemies[i], dt, player, world);
	}

	for (size_t i = 0; i < MAX_ENEMIES/4; i++)
	{
		if (world->explosions[i].active)
		{
			world->explosions[i].t += dt * 3.5f;
			world->explosions[i].s = (sin(world->explosions[i].t) / 2 + 0.5f) * 0.35f;

			if (world->explosions[i].t >= 1.0f)
			{
				world->explosions[i].active = false;
			}

		}
	}

	for (size_t i = 0; i < WORLD_STARS; i++)
	{
		world->stars[i].y -= world->stars[i].l * dt;
		
		if (world->stars[i].y < -1.0)
		{
			world->stars[i].y = 1.0f;
		}
	}

	for (size_t i = 0; i < MAX_POWERUPS; i++)
	{
		powerup_update(&world->powerups[i], dt, player, world);
	}

	if (count == 0)
	{
		// Next level
		world->level++;
		world->score += world->level * 5;

		// Spawn next level
		world_spawn(world);
	}

	if (world->health <= 0)
	{
		if (player->mode == 0)
		{
			// LOSE
			
		}
		else if (player->mode == 1)
		{
			player->mode = 0;
		}
		else
		{
			player->mode-=2;
			world->health = 5;
		}
	}
}

float randf()
{
	return (float)rand() / RAND_MAX*2.0 - 1.0; //float in range -1 to 1
}

void world_spawn(world_t* world)
{
	//srand(world->level * 1043240 % 40041);

	if (world->level <= 0)
	{
		world_add_enemy(world, 0.2f, 0.5f, randf() * 0.8f, randf() * 3.0f, 0, enemy_laser);
	}
	else if (world->level < 5)
	{
		// Only hor, ver, and circle movers
		// Only colliders
		
		for (size_t i = 0; i < world->level + 1; i++)
		{
			world_add_enemy(world, randf() * 0.8f, randf() * 0.8f, randf() * 0.8f, randf() * 3.0f, rand() % 3, rand() % 3);
		}

	}
	else if (world->level < 10)
	{

		for (size_t i = 0; i < world->level - 2; i++)
		{
			world_add_enemy(world, randf() * 0.8f, randf() * 0.8f, randf() * 0.8f, randf() * 3.0f, rand() % 5, rand() % 3);
		}
	}
	else if (world->level < 15)
	{
		// All movers
		// Colliders and slow/med shooters
		// < 7 bois

		for (size_t i = 0; i < world->level - 2; i++)
		{
			world_add_enemy(world, randf() * 0.8f, randf() * 0.8f, randf() * 0.8f, randf() * 3.0f, rand() % 5, rand() % 5);
		}
	}
	else
	{
		// All shit
		// < 14 bois

		for (size_t i = 0; i < rand() % 35; i++)
		{
			world_add_enemy(world, randf() * 0.8f, randf() * 0.8f, randf() * 0.8f, randf() * 3.0f, rand() % 6, rand() % 5);
		}
	}
}

void world_add_enemy(world_t* world, float x, float y, float s, float p, int pattern, int enemy)
{
	for (size_t i = 0; i < MAX_ENEMIES; i++)
	{
		if (world->enemies[i].alive == false)
		{
			world->enemies[i] = enemy_init(x, y, s, p, pattern, enemy);
			break;
		}
	}
}

void powerup_draw(powerup_t* power)
{
	if (power->active)
	{
		if (power->type == powerup_health)
		{
			// + sign

			// H quad
			glBegin(GL_QUADS);
			{
				glColor3f(1.0f, 0.1f, 0.1f);

				// Top left
				glVertex3f(power->x - 0.05f, power->y, 0.5f);
				// Top right
				glVertex3f(power->x + 0.05f, power->y, 0.5f);
				// Bot right
				glVertex3f(power->x + 0.05f, power->y + 0.02f, 0.5f);
				// Bot left
				glVertex3f(power->x - 0.05f, power->y + 0.02f, 0.5f);
			}
			glEnd();

			// V Quad
			glBegin(GL_QUADS);
			{
				glColor3f(1.0f, 0.1f, 0.1f);
				// Top left
				glVertex3f(power->x - 0.01f, power->y + 0.06f, 0.5f);
				// Top right
				glVertex3f(power->x + 0.01f, power->y + 0.06f, 0.5f);
				// Bot right
				glVertex3f(power->x + 0.01f, power->y - 0.04f, 0.5f);
				// Bot left
				glVertex3f(power->x - 0.01f, power->y - 0.04f, 0.5f);
			}
			glEnd();

		}
		else if (power->type == powerup_mode)
		{
			// arrow thing
			// Top triangle
			glBegin(GL_TRIANGLES);
			{
				glColor3f(0.4f, 0.4f, 1.0f);

				// Top
				glVertex3f(power->x + 0.01f, power->y + 0.04f, 0.5f);

				// Left
				glVertex3f(power->x - 0.02f, power->y, 0.5f);
		
				// Right
				glVertex3f(power->x + 0.04f, power->y, 0.5f);
			}
			glEnd();

			glBegin(GL_QUADS);
			{
				glColor3f(0.8f, 0.8f, 1.0f);

				// Top left
				glVertex3f(power->x, power->y, 0.5f);

				// Top right
				glVertex3f(power->x + 0.02f, power->y, 0.5f);

				// Bot right
				glVertex3f(power->x + 0.02f, power->y - 0.05f, 0.5f);

				// Bot left
				glVertex3f(power->x, power->y - 0.05f, 0.5f);
			}
			glEnd();
		}
		else if (power->type == powerup_superhealth)
		{
			float r = (sin(power->t) + 1.0f) / 2.0f;
			float g = (sin(power->t + 0.2f) + 1.0f) / 2.0f;
			float b = (cos(power->t) + 1.0f) / 2.0f;

			glBegin(GL_QUADS);
			{
				glColor3f(r, g, b);

				// Top left
				glVertex3f(power->x - 0.05f, power->y, 0.5f);
				// Top right
				glVertex3f(power->x + 0.05f, power->y, 0.5f);
				// Bot right
				glVertex3f(power->x + 0.05f, power->y + 0.02f, 0.5f);
				// Bot left
				glVertex3f(power->x - 0.05f, power->y + 0.02f, 0.5f);
			}
			glEnd();

			// V Quad
			glBegin(GL_QUADS);
			{
				glColor3f(r, g, b);
				// Top left
				glVertex3f(power->x - 0.01f, power->y + 0.06f, 0.5f);
				// Top right
				glVertex3f(power->x + 0.01f, power->y + 0.06f, 0.5f);
				// Bot right
				glVertex3f(power->x + 0.01f, power->y - 0.04f, 0.5f);
				// Bot left
				glVertex3f(power->x - 0.01f, power->y - 0.04f, 0.5f);
			}
			glEnd();
		}
	}
}

void world_draw(world_t* world)
{
	// Draw star backdrop

	for (size_t i = 0; i < WORLD_STARS; i++)
	{
		glBegin(GL_POINTS);
		{
			glColor3f(world->stars[i].r, world->stars[i].g, world->stars[i].b);
			glVertex3f(world->stars[i].x, world->stars[i].y, 0.8f);
		}
		glEnd();
	}

	for (size_t i = 0; i < MAX_ENEMIES; i++)
	{
		enemy_draw(&world->enemies[i]);
	}

	for (size_t i = 0; i < MAX_ENEMIES/4; i++)
	{
		if (world->explosions[i].active)
		{
			glBegin(GL_QUADS);
			{
				glColor3f(1.0f - world->explosions[i].t, (1.0f - world->explosions[i].t) * 0.7f, (1.0f - world->explosions[i].t) * 0.5f);
 
				glVertex3f(world->explosions[i].x - world->explosions[i].s * world->explosions[i].t, 
					world->explosions[i].y - world->explosions[i].s * world->explosions[i].t, 0.5f);

				glVertex3f(world->explosions[i].x + world->explosions[i].s * world->explosions[i].t,
					world->explosions[i].y - world->explosions[i].s * world->explosions[i].t, 0.5f);

				glVertex3f(world->explosions[i].x + world->explosions[i].s * world->explosions[i].t,
					world->explosions[i].y + world->explosions[i].s * world->explosions[i].t, 0.5f);

				glVertex3f(world->explosions[i].x - world->explosions[i].s * world->explosions[i].t,
					world->explosions[i].y + world->explosions[i].s * world->explosions[i].t, 0.5f);

			}
			glEnd();
		}
	}

	for (size_t i = 0; i < MAX_POWERUPS; i++)
	{
		powerup_draw(&world->powerups[i]);
	}
}


world_t* world_init()
{
	world_t* out = calloc(1, sizeof(world_t));

	out->restartTimer = -1;
	out->restartSTimer = 0.0f;

	for (size_t i = 0; i < MAX_ENEMIES; i++)
	{
		out->enemies[i] = enemy_init(0, 0, 0, 0, 0, 0);
		out->enemies[i].alive = false;
	}

	for (size_t i = 0; i < MAX_ENEMIES / 4; i++)
	{
		out->explosions[i].active = false;
	}

	for (size_t i = 0; i < MAX_POWERUPS; i++)
	{
		out->powerups[i].active = false;
	}

	for (size_t i = 0; i < WORLD_STARS; i++)
	{
		out->stars[i].x = randf();
		out->stars[i].y = randf();
		out->stars[i].l = (randf() + 1.0f)/2;
		out->stars[i].r = ((randf() / 2) + 0.5f)*2;
		out->stars[i].g = ((randf() / 2) + 0.5f) * 2;
		out->stars[i].b = ((randf() / 2) + 0.5f) * 2;
	}


	out->score = 0;
	out->level = 0;
	out->health = 10;
	out->hurtTimer = 0.0f;

	world_spawn(out);

	return out;
}