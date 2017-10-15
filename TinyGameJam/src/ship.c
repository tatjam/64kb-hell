#include <ship.h>

ship_t ship_init()
{
	ship_t out;

	out.x = 0.0f;
	out.y = 0.0f;
	out.w = 0.1f;
	out.speed = 1.0f;
	out.timer = 0.0f;
	out.mode = ship_simple;

	for (size_t i = 0; i < SHIP_BULLETS; i++)
	{
		out.bullets[i].active = false;
	}

	return out;
}

bullet_t bullet_init(float x, float y)
{
	bullet_t out;

	out.x = x;
	out.y = y;
	out.sx = x;
	out.sy = y;
	out.w = 0.05f;
	out.active = true;
	out.time = 0.0f;
	return out;
}

void bullet_draw(bullet_t* bullet)
{
	float c = bullet->time / 1.0f;
	if (c > 1.0f)
	{
		c = 1.0f;
	}

	c = 1.0f - c;

	glBegin(GL_TRIANGLES);
	{
		glColor3f(c, c * 0.2f + 0.4f, c * 0.4f + 0.3f);

		// Top vertex, at ship position + 1/2 width
		glVertex3f(bullet->x + bullet->w / 2, bullet->y, 1.0f);

		// Bottom left
		glVertex3f(bullet->x, bullet->y - bullet->w, 1.0f);

		// Bottom right
		glVertex3f(bullet->x + bullet->w, bullet->y - bullet->w, 1.0f);
	}
	glEnd();

	glBegin(GL_LINES);
	{
		glColor4f(c * 0.5f, c * 0.2f + 0.4f, c * 0.4f + 0.3f, c);

		glVertex3f(bullet->x + bullet->w / 2, bullet->y + bullet->w / 2, 1.0f);
		glVertex3f(bullet->sx + bullet->w / 2, bullet->sy + bullet->w / 2, 1.0f);
	}
	glEnd();

}

void ship_draw(ship_t* ship)
{
	for (size_t i = 0; i < SHIP_BULLETS; i++)
	{
		if (ship->bullets[i].active == true)
		{
			bullet_draw(&ship->bullets[i]);
		}
	}

	float c = ship->timer;
	if (c < 0) { c = 0; }
	if (c > 1.0f) { c = 1; };

	// Body is always present
	glBegin(GL_TRIANGLES);
	{
		glColor3f(1.0f, (1 - c) * 0.7f, (1 - c) * 0.5f);
	

		// Top vertex, at ship position + 1/2 width
		glVertex3f(ship->x + ship->w / 2, ship->y, 1.0f);

		glColor3f(1.0f, (1 - c) * 1.0f, (1 - c) * 1.0f);

		// Bottom left
		glVertex3f(ship->x, ship->y - ship->w, 1.0f);

		// Bottom right
		glVertex3f(ship->x + ship->w, ship->y - ship->w, 1.0f);

	}
	glEnd();

	// Draw mode specific things

}

void bullet_update(bullet_t* bullet, float dt)
{
	bullet->x += bullet->vx * dt;
	bullet->y += bullet->vy * dt;

	if (bullet->x < -1 || bullet->x > 1 || bullet->y < -1 || bullet->y > 1)
	{
		bullet->active = false;
	}

	bullet->time += dt;
}

void ship_shoot(ship_t* ship)
{
	if (ship->mode == ship_simple)
	{
		for (size_t i = 0; i < SHIP_BULLETS; i++)
		{
			if (ship->bullets[i].active == false)
			{
				ship->bullets[i] = bullet_init(ship->x, ship->y);
				// Set speed
				ship->bullets[i].vx = 0;
				ship->bullets[i].vy = 2.0f;

				ship->timer = 0.4f;

				break;
			}
		}
	}
	else if (ship->mode == ship_dual)
	{
		// First bullet
		for (size_t i = 0; i < SHIP_BULLETS; i++)
		{
			if (ship->bullets[i].active == false)
			{
				ship->bullets[i] = bullet_init(ship->x, ship->y);
				// Set speed
				ship->bullets[i].vy = 2.0f;
				ship->bullets[i].vx = -0.5f;

				break;
			}
		}
		// Second bullet
		for (size_t i = 0; i < SHIP_BULLETS; i++)
		{
			if (ship->bullets[i].active == false)
			{
				ship->bullets[i] = bullet_init(ship->x, ship->y);
				// Set speed
				ship->bullets[i].vy = 2.0f;
				ship->bullets[i].vx = 0.5f;

				break;
			}
		}

		ship->timer = 0.4f;
	}
	else if (ship->mode == ship_triple)
	{
		for (size_t i = 0; i < 3; i++)
		{
			for (size_t j = 0; j < SHIP_BULLETS; j++)
			{
				if (ship->bullets[j].active == false)
				{
					ship->bullets[j] = bullet_init(ship->x, ship->y);

					if (i == 0)
					{
						ship->bullets[i].vy = 2.0f;
						ship->bullets[i].vx = -0.5f;

					}
					else if (i == 1)
					{
						ship->bullets[i].vy = 2.0f;
						ship->bullets[i].vx = 0.0f;
					}
					else
					{
						ship->bullets[i].vy = 2.0f;
						ship->bullets[i].vx = 0.5f;
					}

					break;
				}
			}
		}

		ship->timer = 0.35f;
	}
	else
	{
		for (size_t i = 0; i < SHIP_BULLETS; i++)
		{
			if (ship->bullets[i].active == false)
			{
				ship->bullets[i] = bullet_init(ship->x, ship->y);
				// Set speed
				ship->bullets[i].vx = 0;
				ship->bullets[i].vy = 2.0f;

				break;
			}
		}

		ship->timer = 0.08f;
	}
}

void ship_update(ship_t* ship, float dt, bool up, bool down, bool right, bool left, bool fire)
{
	for (size_t i = 0; i < SHIP_BULLETS; i++)
	{
		if (ship->bullets[i].active == true)
		{
			bullet_update(&ship->bullets[i], dt);
		}
	}

	if (right)
	{
		ship->x += ship->speed * dt;
	}

	if (left)
	{
		ship->x -= ship->speed * dt;
	}

	if (down)
	{
		ship->y -= ship->speed * dt;
	}

	if (up)
	{
		ship->y += ship->speed * dt;
	}

	if (fire && ship->timer <= 0.0f)
	{
		ship_shoot(ship);
	}

	if (ship->x < -1)
	{
		ship->x = -1;
	}

	if (ship->x > 1 - ship->w)
	{
		ship->x = 1 - ship->w;
	}

	if (ship->y < -1 + ship->w)
	{
		ship->y = -1 + ship->w;
	}

	if (ship->y > 1)
	{
		ship->y = 1;
	}

	ship->timer -= dt;
}
