#ifndef SCENE_H
#define SCENE_H

#include "GLSLProgram.h"
#include "TriangleMesh.h"
#include "Camera.h"

class Scene
{
private:
  GLSLProgram shader;

  glm::mat4 model, view, projection;

  std::vector<TriangleMesh *> shapes;

  Camera *camera;

  void setMatrices();
  void compileAndLinkShader();

public:
  Scene() {}
  ~Scene();

  void initScene();
  void update();
  void render();
};

#endif //SCENE_H
