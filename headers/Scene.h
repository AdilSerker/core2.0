#ifndef SCENE_H
#define SCENE_H

#include "GLSLProgram.h"
#include "TriangleMesh.h"
#include "Camera.h"
#include "Character.h"

class Scene
{
private:
  GLSLProgram shader;

  std::vector<TriangleMesh *> shapes;

  Character *character;

  void compileAndLinkShader();

public:
  Scene();
  ~Scene();

  void initScene();
  void addShape(TriangleMesh *mesh);
  void addChar(Character *character);
  void render(glm::mat4 view, glm::mat4 proj);
};

#endif //SCENE_H
