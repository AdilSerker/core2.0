#ifndef SCENE_H
#define SCENE_H

#include "GLSLProgram.h"
#include "TriangleMesh.h"
#include "Camera.h"

class Scene
{
private:
  GLSLProgram shader;

  glm::mat4 model;

  std::vector<TriangleMesh *> shapes;

  void compileAndLinkShader();

public:
  Scene() {}
  ~Scene();

  void initScene();
  void addShape(TriangleMesh *mesh);
  void render(glm::mat4 view, glm::mat4 proj);
};

#endif //SCENE_H
