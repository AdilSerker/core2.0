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
    delete camera;
}

void Scene::initScene()
{
    compileAndLinkShader();

    this->camera = new Camera();

    shapes.push_back(new Terrain(100.0f, 1024));

    shader.setUniform("Fog.maxDist", 80.0f);
    shader.setUniform("Fog.minDist", 1.0f);
    shader.setUniform("Fog.color", vec3(0.5f, 0.5f, 0.5f));
}

void Scene::compileAndLinkShader()
{
    shader.load("./shaders/phong.vert", "./shaders/phong.frag");

    shader.use();
}

void Scene::setMatrices()
{
    camera->computeMatricesFromInputs();

    view = camera->getViewMatrix();
    projection = camera->getProjectionMatrix();
}

void Scene::render()
{
    setMatrices();

    shader.setUniform("Light.position", view * glm::vec4(0.0f, 1.0f, 1.0f, 0.0f));
    shader.setUniform("Light.intensity", vec3(0.8f, 0.8f, 0.8f));

    for (std::vector<TriangleMesh *>::iterator it = shapes.begin(); it != shapes.end(); ++it)
    {
        (*it)->render(&shader, view, projection);
    }
}
