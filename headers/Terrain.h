#ifndef TERRAIN_H
#define TERRAIN_H

#include "TriangleMesh.h"
#include <glm/glm.hpp>

class Terrain : public TriangleMesh
{
    float hscale, vscale;
    float offset;
    std::vector<std::vector<float>> data;

    void init(float size, int divs, int range, float smax, float tmax = 1.0f);

  public:
    Terrain(float size, int divs, float smax = 1.0f, float tmax = 1.0f);
    Terrain(float size, int divs, int range, float smax = 1.0f, float tmax = 1.0f);
    void render(GLSLProgram *shader, glm::mat4 view, glm::mat4 proj);
    float sample(glm::vec2 pos);
};

#endif //TERRAIN_H
