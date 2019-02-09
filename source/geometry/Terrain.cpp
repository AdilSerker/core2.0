#include "Terrain.h"

#include <iostream>
using namespace std;

double random(double min, double max)
{
    double d = (double)rand() / RAND_MAX;
    return min + d * (max - min);
}

void diamondSquare(std::vector<std::vector<float>> *data, int size, int level, float range)
{
    if (level < 1)
        return;

    for (int i = 0; i < size - 1; i += level)
    {
        for (int j = 0; j < size - 1; j += level)
        {
            float a = (*data)[i][j];
            float b = (*data)[i][j + level];
            float c = (*data)[i + level][j];
            float d = (*data)[i + level][j + level];

            (*data)[i + level / 2][j + level / 2] = (a + b + c + d) / 4 + random(-range, range);
        }
    }

    for (int i = 0; i < size - 1; i += level)
    {
        for (int j = 0; j < size - 1; j += level)
        {
            float a = (*data)[i][j];
            float b = (*data)[i][j + level];
            float c = (*data)[i + level][j];
            float d = (*data)[i + level][j + level];
            float e0 = (*data)[i + level / 2][j + level / 2];

            float e1 = (i - level / 2 > 0) ? (*data)[i - level / 2][j + level / 2] : e0;
            float e2 = (j - level / 2 > 0) ? (*data)[i + level / 2][j - level / 2] : e0;
            float e3 = (j + level + level / 2) < size ? (*data)[i + level / 2][j + level + level / 2] : e0;
            float e4 = (i + level + level / 2) < size ? (*data)[i + level + level / 2][j - level / 2] : e0;

            (*data)[i][j + level / 2] = (e1 == e0) ? (a + c + e0) / 3 + random(-range, range) : (a + c + e0 + e1) / 4 + random(-range, range);
            (*data)[i + level / 2][j] = (e2 == e0) ? (a + b + e0) / 3 + random(-range, range) : (a + b + e0 + e2) / 4 + random(-range, range);
            (*data)[i + level][j + level / 2] = (e4 == e0) ? (a + b + e0) / 3 + random(-range, range) : (b + d + e0 + e4) / 4 + random(-range, range);
            (*data)[i + level / 2][j + level] = (e3 == e0) ? (a + b + e0) / 3 + random(-range, range) : (c + d + e0 + e3) / 4 + random(-range, range);
        }
    }

    diamondSquare(data, size, level / 2, range / 2);
}

float Terrain::sample(glm::vec2 pos)
{
    int w = data.size();
    int h = data[0].size();

    pos.x = (pos.x / hscale) + w / 2;
    pos.y = (pos.y / hscale) + h / 2;

    float a0 = fmod(pos.x, 1.0);
    float a1 = fmod(pos.y, 1.0);

    int x0 = (int)std::floor(pos.x), x1 = (int)std::ceil(pos.x);
    int y0 = (int)std::floor(pos.y), y1 = (int)std::ceil(pos.y);

    x0 = x0 < 0 ? 0 : x0;
    x0 = x0 >= w ? w - 1 : x0;
    x1 = x1 < 0 ? 0 : x1;
    x1 = x1 >= w ? w - 1 : x1;
    y0 = y0 < 0 ? 0 : y0;
    y0 = y0 >= h ? h - 1 : y0;
    y1 = y1 < 0 ? 0 : y1;
    y1 = y1 >= h ? h - 1 : y1;

    float s0 = vscale * (data[x0][y0] - offset);
    float s1 = vscale * (data[x1][y0] - offset);
    float s2 = vscale * (data[x0][y1] - offset);
    float s3 = vscale * (data[x1][y1] - offset);

    return (s0 * (1 - a0) + s1 * a0) * (1 - a1) + (s2 * (1 - a0) + s3 * a0) * a1;
}

Terrain::Terrain(float size, int divs, float smax, float tmax)
{
    std::vector<GLfloat> p(3 * (divs + 1) * (divs + 1));
    std::vector<GLfloat> n(3 * (divs + 1) * (divs + 1));
    std::vector<GLfloat> tex(2 * (divs + 1) * (divs + 1));
    std::vector<GLuint> el(6 * divs * divs);

    hscale = size / divs;
    vscale = 1.0f;

    int w = divs + 1;

    for (int x = 0; x <= divs; ++x)
    {
        std::vector<float> row;

        for (int y = 0; y <= divs; ++y)
        {
            float f = 0.0f;
            row.push_back(f);
        }
        data.push_back(row);
    }

    float range = (size - 1) / 4;

    srand(time(NULL));

    data[0][0] = random(0, range);
    data[0][divs] = random(0, range);
    data[divs][0] = random(0, range);
    data[divs][divs] = random(0, range);

    diamondSquare(&data, divs + 1, divs, range / 2);

    glm::vec3 *posns = (glm::vec3 *)malloc(sizeof(glm::vec3) * (divs + 1) * (divs + 1));
    glm::vec3 *norms = (glm::vec3 *)malloc(sizeof(glm::vec3) * (divs + 1) * (divs + 1));

    for (int x = 0; x < w; x++)
        for (int y = 0; y < w; y++)
        {
            float cx = hscale * x, cy = hscale * y, cw = hscale * w, ch = hscale * w;
            posns[x + y * w] =
                glm::vec3(cx - cw / 2, sample(glm::vec2(cx - cw / 2, cy - ch / 2)),
                          cy - ch / 2);
        }

    for (int x = 0; x < w; x++)
        for (int y = 0; y < w; y++)
        {

            norms[x + y * w] =
                (x > 0 && x < w - 1 && y > 0 && y < w - 1)
                    ? glm::normalize(glm::mix(
                          glm::cross(posns[(x + 0) + (y + 1) * w] - posns[x + y * w],
                                     posns[(x + 1) + (y + 0) * w] - posns[x + y * w]),
                          glm::cross(posns[(x + 0) + (y - 1) * w] - posns[x + y * w],
                                     posns[(x - 1) + (y + 0) * w] - posns[x + y * w]),
                          0.5))
                    : glm::vec3(0, 1, 0);
        }

    float texi = smax / divs;
    float texj = tmax / divs;
    int vidx = 0, tidx = 0;
    for (int i = 0; i <= divs; i++)
    {
        for (int j = 0; j <= divs; j++)
        {
            p[3 * (j + i * (divs + 1)) + 0] = posns[j + i * (divs + 1)].x;
            p[3 * (j + i * (divs + 1)) + 1] = posns[j + i * (divs + 1)].y;
            p[3 * (j + i * (divs + 1)) + 2] = posns[j + i * (divs + 1)].z;
            n[3 * (j + i * (divs + 1)) + 0] = norms[j + i * (divs + 1)].x;
            n[3 * (j + i * (divs + 1)) + 1] = norms[j + i * (divs + 1)].y;
            n[3 * (j + i * (divs + 1)) + 2] = norms[j + i * (divs + 1)].z;

            tex[tidx] = j * texi;
            tex[tidx + 1] = i * texj;

            vidx += 3;
            tidx += 2;
        }
    }

    GLuint rowStart, nextRowStart;
    int idx = 0;
    for (int i = 0; i < divs; i++)
    {
        rowStart = (GLuint)(i * (divs + 1));
        nextRowStart = (GLuint)((i + 1) * (divs + 1));
        for (int j = 0; j < divs; j++)
        {
            el[idx] = rowStart + j;
            el[idx + 1] = nextRowStart + j;
            el[idx + 2] = nextRowStart + j + 1;
            el[idx + 3] = rowStart + j;
            el[idx + 4] = nextRowStart + j + 1;
            el[idx + 5] = rowStart + j + 1;
            idx += 6;
        }
    }

    initBuffers(&el, &p, &n, &tex);
}

void Terrain::render(GLSLProgram *shader, glm::mat4 view, glm::mat4 proj)
{
    if (vao == 0)
        return;

    glBindVertexArray(vao);

    shader->setUniform("Kd", 0.2f, 0.2f, 0.2f);
    shader->setUniform("Ks", 0.9f, 0.9f, 0.9f);
    shader->setUniform("Ka", 0.1f, 0.1f, 0.1f);
    shader->setUniform("Shininess", 180.0f);

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 mv = view * model;

    shader->setUniform("ModelViewMatrix", mv);
    shader->setUniform("NormalMatrix",
                       glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
    shader->setUniform("MVP", proj * mv);

    glDrawElements(GL_TRIANGLES, nVerts, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
