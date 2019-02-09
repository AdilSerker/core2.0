#include "Torus.h"

#include <cstdio>
#include <cmath>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

Torus::Torus(GLfloat outerRadius, GLfloat innerRadius, GLuint nsides, GLuint nrings)
{
    GLuint faces = nsides * nrings;
    int nVerts = nsides * (nrings + 1); // One extra ring to duplicate first ring

    // Points
    std::vector<GLfloat> p(3 * nVerts);
    // Normals
    std::vector<GLfloat> n(3 * nVerts);
    // Tex coords
    std::vector<GLfloat> tex(2 * nVerts);
    // Elements
    std::vector<GLuint> el(6 * faces);

    // Generate the vertex data
    float ringFactor = glm::two_pi<float>() / nrings;
    float sideFactor = glm::two_pi<float>() / nsides;
    int idx = 0, tidx = 0;
    for (GLuint ring = 0; ring <= nrings; ring++)
    {
        float u = ring * ringFactor;
        float cu = cos(u);
        float su = sin(u);
        for (GLuint side = 0; side < nsides; side++)
        {
            float v = side * sideFactor;
            float cv = cos(v);
            float sv = sin(v);
            float r = (outerRadius + innerRadius * cv);
            p[idx] = r * cu;
            p[idx + 1] = r * su;
            p[idx + 2] = innerRadius * sv;
            n[idx] = cv * cu * r;
            n[idx + 1] = cv * su * r;
            n[idx + 2] = sv * r;
            tex[tidx] = u / glm::two_pi<float>();
            tex[tidx + 1] = v / glm::two_pi<float>();
            tidx += 2;
            // Normalize
            float len = sqrt(n[idx] * n[idx] +
                             n[idx + 1] * n[idx + 1] +
                             n[idx + 2] * n[idx + 2]);
            n[idx] /= len;
            n[idx + 1] /= len;
            n[idx + 2] /= len;
            idx += 3;
        }
    }

    idx = 0;
    for (GLuint ring = 0; ring < nrings; ring++)
    {
        GLuint ringStart = ring * nsides;
        GLuint nextRingStart = (ring + 1) * nsides;
        for (GLuint side = 0; side < nsides; side++)
        {
            int nextSide = (side + 1) % nsides;
            // The quad
            el[idx] = (ringStart + side);
            el[idx + 1] = (nextRingStart + side);
            el[idx + 2] = (nextRingStart + nextSide);
            el[idx + 3] = ringStart + side;
            el[idx + 4] = nextRingStart + nextSide;
            el[idx + 5] = (ringStart + nextSide);
            idx += 6;
        }
    }

    initBuffers(&el, &p, &n, &tex);
}

void Torus::render(GLSLProgram *shader, glm::mat4 view, glm::mat4 proj)
{
    if (vao == 0)
        return;

    glBindVertexArray(vao);
    
    shader->setUniform("Kd", 0.9f, 0.2f, 0.1f);
    shader->setUniform("Ka", 0.6f, 0.6f, 0.6f);
    shader->setUniform("Ks", 0.8f, 0.8f, 0.8f);
    shader->setUniform("Shininess", 100.0f);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(15.0f,0.0f,5.0f));
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f,0.0f,0.0f));

    glm::mat4 mv = view * model;
    shader->setUniform("ModelViewMatrix", mv);
    shader->setUniform("NormalMatrix",
                    glm::mat3( glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2]) ));
    shader->setUniform("MVP", proj * mv);

    glDrawElements(GL_TRIANGLES, nVerts, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
