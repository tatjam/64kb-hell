#include <GLFW/glfw3.h>
#include <ship.h>
#include <ui.h>
#include <world.h>
#include <time.h>


void update_keys(bool* u, bool* d, bool* r, bool* l, bool* f, GLFWwindow* window)
{
	*u = false;
	*d = false;
	*r = false;
	*l = false;
	*f = false;
	// xd

	int us, ds, rs, ls, fs;
	int us2, ds2, rs2, ls2;
	us = glfwGetKey(window, GLFW_KEY_UP);
	ds = glfwGetKey(window, GLFW_KEY_DOWN);
	rs = glfwGetKey(window, GLFW_KEY_RIGHT);
	ls = glfwGetKey(window, GLFW_KEY_LEFT);
	fs = glfwGetKey(window, GLFW_KEY_SPACE);
	us2 = glfwGetKey(window, GLFW_KEY_W);
	ds2 = glfwGetKey(window, GLFW_KEY_S);
	rs2 = glfwGetKey(window, GLFW_KEY_D);
	ls2 = glfwGetKey(window, GLFW_KEY_A);

	if (us == GLFW_PRESS || us2 == GLFW_PRESS)
	{
		*u = true;
	}
	if (ds == GLFW_PRESS || ds2 == GLFW_PRESS)
	{
		*d = true;
	}
	if (rs == GLFW_PRESS || rs2 == GLFW_PRESS)
	{
		*r = true;
	}
	if (ls == GLFW_PRESS || ls2 == GLFW_PRESS)
	{
		*l = true;
	}
	if (fs == GLFW_PRESS)
	{
		*f = true;
	}
}

int main(void)
{
	GLFWwindow* window;

	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	window = glfwCreateWindow(640, 640, "64Kb Hell", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	srand(time(NULL));

	while (true)
	{
		ui_init();

		ship_t ship = ship_init();
		double lastFrame = 0, currentFrame = 0, dtd = 0;
		float dt = 0.0f;

		bool u = 0, d = 0, r = 0, l = 0, f = 0;


		glViewport(0, 0, 640, 640);

		ship.mode = ship_simple;


		world_t* world = world_init();

		//glEnable(GL_DEPTH_TEST);
		//glTranslatef(0.0f, 0.0f, -2.0f);

		while (!glfwWindowShouldClose(window))
		{

			// Check for game finish 
			if(world->restartTimer == 0)
			{
				// This effectively restars the game
				break;
			}

			currentFrame = glfwGetTime();

			update_keys(&u, &d, &r, &l, &f, window);

			ship_update(&ship, dt, u, d, r, l, f);

			world_update(world, dt, &ship);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			ship_draw(&ship);
			world_draw(world);
			ui_draw(&ship, world);



			dtd = currentFrame - lastFrame;
			dt = (float)dtd;
			lastFrame = currentFrame;

			glfwSwapBuffers(window);

			glfwPollEvents();
		}

		if (glfwWindowShouldClose(window))
		{
			// Actual quit
			glfwTerminate();
			return 0;
		}

		free(world);
	}

	glfwTerminate();
	return 0;

}