#include "CameraOrbit.h"

#include <GLFW/glfw3.h>

#define CAMERA_SPEED 0.03
float mouseSpeed = 0.3f;

CameraOrbit::CameraOrbit()
{

	pitch = M_PI / 6;
	yaw = 0;
	target = glm::vec3(0);
	distance = 300;

	invert_y = -1;

	const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	WINDOW_WIDTH = mode->width / 1.5;
	WINDOW_HEIGHT = mode->height / 1.5;
}

void CameraOrbit::update(double xpos, double ypos)
{

	float horizontalAngle = mouseSpeed * float(WINDOW_WIDTH / 2 - xpos);
	float verticalAngle = invert_y * mouseSpeed * float(WINDOW_HEIGHT / 2 - ypos);

	pitch = glm::clamp(pitch + verticalAngle * CAMERA_SPEED, -M_PI / 8, 2 * M_PI / 5);
	yaw += (horizontalAngle)*CAMERA_SPEED;
}

glm::vec3 CameraOrbit::position()
{
	glm::vec3 posn = glm::mat3(glm::rotate(yaw, glm::vec3(0, 1, 0))) * glm::vec3(distance, 0, 0);
	glm::vec3 axis = glm::normalize(glm::cross(posn, glm::vec3(0, 1, 0)));
	return glm::mat3(glm::rotate(pitch, axis)) * posn + target;
}

glm::vec3 CameraOrbit::direction()
{
	return glm::normalize(target - position());
}

glm::mat4 CameraOrbit::getViewMatrix()
{
	return glm::lookAt(position(), target, glm::vec3(0, 1, 0));
}

glm::mat4 CameraOrbit::getProjectionMatrix()
{
	return glm::perspective(45.0f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 1.0f, 5000.0f);
}
