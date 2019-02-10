#include "App.h"

#include <iostream>
using namespace std;

void App::update()
{
	const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	int WINDOW_WIDTH = mode->width / 1.5;
	int WINDOW_HEIGHT = mode->height / 1.5;

	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	glfwSetCursorPos(window, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);

	camera->update(xpos, ypos);
	float distance = glm::length(camera->target - character->getPosition());

	if (distance > 100)
	{
		glm::vec3 new_target = glm::normalize(character->getPosition() - camera->target) * (distance - 100);

		camera->target += new_target;
	}

	glm::vec2 direction_velocity = glm::vec2();
	int vel = -32768;
	int strafe = -32768;
	bool is_crouched = false;

	getInput(&direction_velocity, &vel, &strafe, &is_crouched);
	if (character != nullptr)
	{
		character->update_move(direction_velocity, camera->direction(), vel, strafe, is_crouched);
	}
}

void App::getInput(glm::vec2 *direction_velocity, int *vel, int *strafe, bool *is_crouched)
{

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		direction_velocity->y += 32768;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		direction_velocity->x += 32768;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		direction_velocity->y -= 32768;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		direction_velocity->x -= 32768;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		*vel += 65535;
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)
	{
		*strafe += 65535;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		*is_crouched = true;
	}
}

void App::render()
{
	scene->render(camera->getViewMatrix(), camera->getProjectionMatrix());
}

App::App()
{
	initWindow();

	character = nullptr;
}

App::~App()
{
	delete scene;
	delete camera;
}

void App::init()
{
	this->scene = new Scene();
	scene->initScene();

	this->ter = new Terrain(5000.0f, 512);
	scene->addShape(this->ter);

	this->area = new Areas();
	area->clear();

	this->character = new Character();
	// cout << "char inited"
	// 	 << "\n";
	character->reset_position(glm::vec2(0, 0), ter, area);
	scene->addChar(character);
	// cout << "char inject to scene"
	// 	 << "\n";
	this->camera = new CameraOrbit();
	// cout << "camera inited"
	// 	 << "\n";
}

void App::initWindow()
{
	if (!glfwInit())
	{
		fprintf(stderr, "GLFW Initialization error\n");
		exit(-1);
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	int WINDOW_WIDTH = mode->width / 1.5;
	int WINDOW_HEIGHT = mode->height / 1.5;

	window = glfwCreateWindow(
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		"OpenGL template",
		NULL, // glfwGetPrimaryMonitor(),
		NULL);
	if (window == NULL)
	{
		fprintf(stderr, "Window initialization error\n");
		glfwTerminate();
		exit(-1);
	}

	std::cout << mode->width << "x" << mode->height << "\n";
	glfwMakeContextCurrent(window);

	glewExperimental = true;
	if (glewInit() != GLEW_OK)
	{
		fprintf(stderr, "GLEW Initialization error\n");
		exit(-1);
	}

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glClearColor(0.71f, 0.95f, 1.0f, 0.0f);

	glEnable(GL_DEPTH_TEST);

	glDepthFunc(GL_LESS);

	glEnable(GL_CULL_FACE);
}
void App::run()
{
	cout << "run"
		 << "\n";
	while (!glfwWindowShouldClose(window) && !glfwGetKey(window, GLFW_KEY_ESCAPE))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		update();
		// cout << "updated"
		// 	 << "\n";
		render();
		// cout << "rendered"
		// 	 << "\n";

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	cout << "close"
		 << "\n";
	glfwTerminate();
}
