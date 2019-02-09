#include "App.h"

void App::update()
{
	const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	int WINDOW_WIDTH = mode->width / 1.5;
	int WINDOW_HEIGHT = mode->height / 1.5;

	glm::vec3 cameraPos = camera->getPosition();

	camera->setPosition(glm::vec3(cameraPos.x, ter->sample(glm::vec2(cameraPos.x, cameraPos.z)) + 2, cameraPos.z));
	camera->computeMatricesFromInputs(window);

	glfwSetCursorPos(window, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
}

void App::render()
{
	scene->render(camera->getViewMatrix(), camera->getProjectionMatrix());
}

App::App()
{
	initWindow();
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

	this->ter = new Terrain(100.0f, 512);
	scene->addShape(this->ter);

	this->camera = new Camera();
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
		glfwGetPrimaryMonitor(),
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
	while (!glfwWindowShouldClose(window) && !glfwGetKey(window, GLFW_KEY_ESCAPE))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		update();
		render();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
}
