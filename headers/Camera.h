#ifndef CAMERA_H
#define CAMERA_H

#include <cmath>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
  public:
	Camera();
	void computeMatricesFromInputs(GLFWwindow *window);
	glm::mat4 getViewMatrix() const { return ViewMatrix; }
	glm::mat4 getProjectionMatrix() const { return ProjectionMatrix; }

	glm::vec3 getPosition() const { return position; }

	void setPosition(glm::vec3 pos) { position = pos; }

	glm::vec3 getDirection() const { return direct; }

  private:
	glm::mat4 ViewMatrix;
	glm::mat4 ProjectionMatrix;

	glm::vec3 position;
	glm::vec3 direct;

	float horizontalAngle;
	float verticalAngle;
	float initialFoV;
	float speed;
	float mouseSpeed;
};

#endif //CAMERA_H
