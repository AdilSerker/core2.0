#include "Scene.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using glm::mat4;
using glm::vec3;

#include <iostream>

#include "Plane.h"
#include "Torus.h"
#include "Teapot.h"
#include "Terrain.h"

Scene::~Scene()
{
    for (std::vector<TriangleMesh *>::iterator it = shapes.begin(); it != shapes.end(); ++it)
    {
        delete (*it);
    }

    shapes.clear();
}

void Scene::initScene()
{
    compileAndLinkShader();

    // shapes.push_back(new Terrain(100.0f, 512));

    shader.setUniform("Fog.maxDist", 80.0f);
    shader.setUniform("Fog.minDist", 1.0f);
    shader.setUniform("Fog.color", vec3(0.71f, 0.95f, 1.0f));
}

void Scene::addShape(TriangleMesh *mesh)
{
    shapes.push_back(mesh);
}

void Scene::compileAndLinkShader()
{
    shader.load("./shaders/phong.vert", "./shaders/phong.frag");

    shader.use();
}

void Scene::render(glm::mat4 view, glm::mat4 proj)
{
    shader.setUniform("Light.position", view * glm::vec4(0.0f, 1.0f, 1.0f, 0.0f));
    shader.setUniform("Light.intensity", vec3(0.8f, 0.8f, 0.8f));

    for (std::vector<TriangleMesh *>::iterator it = shapes.begin(); it != shapes.end(); ++it)
    {
        (*it)->render(&shader, view, proj);
    }
}
