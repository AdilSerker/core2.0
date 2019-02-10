#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <GL/glew.h>

GLuint loadBMP_custom(const char * imagepath);
GLuint loadDDS(const char * imagepath);

#endif