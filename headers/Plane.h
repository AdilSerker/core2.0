#ifndef PLANE_H
#define PLANE_H

#include "TriangleMesh.h"

class Plane : public TriangleMesh
{
  public:
    Plane(float xsize, float zsize, int xdivs, int zdivs, float smax = 1.0f, float tmax = 1.0f);
    void render(GLSLProgram *shader, glm::mat4 view, glm::mat4 proj);
};

#endif //PLANE_H
