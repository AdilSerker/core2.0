#ifndef CAMERA_H
#define CAMERA_H

#include <cmath>

#include <glm/glm.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
public:
  Camera();
  void computeMatricesFromInputs();
  glm::mat4 getViewMatrix() const { return ViewMatrix; }
  glm::mat4 getProjectionMatrix() const { return ProjectionMatrix; }

  glm::vec3 getPosition() const { return position; }

private:
  glm::mat4 ViewMatrix;
  glm::mat4 ProjectionMatrix;

  glm::vec3 position;

  float horizontalAngle;
  float verticalAngle;
  float initialFoV;
  float speed;
  float mouseSpeed;
};

#endif //CAMERA_H
