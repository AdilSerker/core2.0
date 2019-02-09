#ifndef GLSL_PROGRAM_H
#define GLSL_PROGRAM_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>

#include <GL/glew.h>
#include <glm/glm.hpp>

class GLSLProgram
{
  public:
    GLSLProgram();
    ~GLSLProgram();

    void use();

    int getHandle();

    void load(const char *vertex_file_path, const char *fragment_file_path);

    void setUniform(const char *name, float x, float y, float z);

    void setUniform(const char *name, const glm::vec2 &v);

    void setUniform(const char *name, const glm::vec3 &v);

    void setUniform(const char *name, const glm::vec4 &v);

    void setUniform(const char *name, const glm::mat4 &m);

    void setUniform(const char *name, const glm::mat3 &m);

    void setUniform(const char *name, float val);

    void setUniform(const char *name, int val);

    void setUniform(const char *name, bool val);

    void setUniform(const char *name, GLuint val);

  private:
    GLuint handle;
    GLuint vs, fs;
    std::map<std::string, int> uniformLocations;

    GLint getUniformLocation(const char *name);
};

#endif //GLSL_PROGRAM_H
