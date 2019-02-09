#ifndef TORUS_H
#define TORUS_H

#include "TriangleMesh.h"

class Torus : public TriangleMesh
{
  public:
	Torus(GLfloat outerRadius, GLfloat innerRadius, GLuint nsides, GLuint nrings);
  void render(GLSLProgram *shader, glm::mat4 view, glm::mat4 proj);
};

#endif //TORUS_H
