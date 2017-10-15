#include <ui.h>

#define FPF_IMPLEMENTATION
#include <five_pixel_font.h>

static uint8_t ftex[FPF_TEXTURE_WIDTH * FPF_TEXTURE_HEIGHT];

GLuint tex;

void ui_string(float x, float y, float scale, char* str)
{
	size_t i = 0;
	float rx = x;
	float ry = y;

	while (str[i] != 0)
	{
		char c = str[i];
		if (c > '~') { c = ' '; }

		// Get texture coordinates for the glyph.
		float left, top, right, bottom;

		fpf_get_glyph_gl_texture_coordinates(c, &left, &top, &right, &bottom);

		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
		glBindTexture(GL_TEXTURE_2D, tex);

		glBegin(GL_QUADS);
		{
			glColor3f(1.0f, 1.0f, 1.0f);
			// Top left
			glTexCoord2f(left, bottom);
			glVertex3f(rx, ry, 0.5f);
			// Top right
			glTexCoord2f(right, bottom);
			glVertex3f(rx + scale, ry, 0.5f);
			// Bottom Right
			glTexCoord2f(right, top);
			glVertex3f(rx + scale, ry + scale, 0.5f);
			// Bottom Left
			glTexCoord2f(left, top);
			glVertex3f(rx, ry + scale, 0.5f);

		}
		glEnd();

		glBindTexture(GL_TEXTURE_2D, 0);

		i++;
		rx += scale;
	}
}

void ui_init()
{
	fpf_create_alpha_texture(ftex, FPF_TEXTURE_WIDTH * FPF_TEXTURE_HEIGHT,
		FPF_TEXTURE_WIDTH, FPF_VECTOR_Y_AXIS);
	
	// Load the font into GL

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D,0,GL_ALPHA,FPF_TEXTURE_WIDTH,FPF_TEXTURE_HEIGHT,0,
		GL_ALPHA,GL_UNSIGNED_BYTE,ftex);


	glBindTexture(GL_TEXTURE_2D, 0);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void ui_draw(ship_t* ship, world_t* world)
{

	// Draw panel

	glBegin(GL_QUADS);
	{
		glColor4f(0.2f, 0.2f, 0.2f, 0.5f);

		// Top left
		glVertex3f(-1.0f, 1.0f, 0.5f);
		// Bottom left
		glVertex3f(-1.0f, 0.8f, 0.5f);
		// Bottom Right
		glVertex3f(1.0f, 0.8f, 0.5f);
		// Top right
		glVertex3f(1.0f, 1.0f, 0.5f);
	}
	glEnd();

	// Draw texts
	char buff[128];
	char* mode = "ERR";

	if (ship->mode == ship_simple)
	{
		mode = "MONO";
	}
	else if (ship->mode == ship_dual)
	{
		mode = "DUAL";
	}
	else if(ship->mode == ship_triple)
	{
		mode = "TRIPLE";
	}
	else if (ship->mode == ship_laser)
	{
		mode = "LASER";
	}


	sprintf(buff, "%i-%s-L:%i [%i]", world->score, mode, world->level, world->health);
	ui_string(-1, 0.85, 0.1, buff);

	// Draw hurt overlay

	if (world->hurtTimer > 0.0f)
	{
		glBegin(GL_QUADS);
		{
			glColor4f(1.0f, 0.2f, 0.2f, world->hurtTimer);

			glVertex3f(-1.0f, -1.0f, 1.0f);
			glVertex3f(1.0f, -1.0f, 1.0f);
			glVertex3f(1.0f, 1.0f, 1.0f);
			glVertex3f(-1.0f, 1.0f, 1.0f);
		}
		glEnd();
	}

	if (world->health <= 0 && ship->mode == ship_simple)
	{
		if (world->restartTimer == -1)
		{
			world->restartTimer = 8;
			world->restartSTimer = 1.0f;
		}
		glBegin(GL_QUADS);
		{
			glColor3f(0.0f, 0.0f, 0.0f);

			glVertex3f(-1.0f, -1.0f, 1.0f);
			glVertex3f(1.0f, -1.0f, 1.0f);
			glVertex3f(1.0f, 1.0f, 1.0f);
			glVertex3f(-1.0f, 1.0f, 1.0f);
		}
		glEnd();

		sprintf(buff, "YOU LOST");
		ui_string(-1, 0.85, 0.1, buff);
		sprintf(buff, "SCORE: %i", world->score);
		ui_string(-1, 0.45, 0.1, buff);
		sprintf(buff, "GAME WILL RESET");
		ui_string(-1, 0, 0.1, buff);
		sprintf(buff, "IN %i SECONDS!", world->restartTimer);
		ui_string(-1, -0.15, 0.1, buff);
		sprintf(buff, "64Kb HELL BY TATJAM");
		ui_string(-1, -0.45, 0.1, buff);
		sprintf(buff, "C + GLFW + OPENGL");
		ui_string(-1, -0.85, 0.1, buff);
	}

}