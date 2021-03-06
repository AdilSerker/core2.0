#ifndef APP_CORE_H
#define APP_CORE_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Scene.h"
#include "Areas.h"
#include "CameraOrbit.h"
#include "Camera.h"
#include "Terrain.h"

#include "Character.h"

#include "TriangleMesh.h"

class App
{
	GLFWwindow *window;

	Scene *scene;
	Areas *area;

	Character *character;

	void initWindow();
	void update();
	void render();

  public:
	bool isFreeCamera;
	CameraOrbit *cameraOrbit;
	Camera *camera;

	App();
	~App();
	void init();
	void run();
	void getInput(glm::vec2 *direction_velocity, int *vel, int *strafe, bool *is_crouched);
};

#endif //APP_CORE_H
