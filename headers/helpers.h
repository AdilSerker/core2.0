#ifndef HELPERS_H
#define HELPERS_H

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>

/* Helper Functions */

glm::vec3 mix_directions(glm::vec3 x, glm::vec3 y, float a);
glm::mat4 mix_transforms(glm::mat4 x, glm::mat4 y, float a);
glm::quat quat_exp(glm::vec3 l);
glm::vec2 segment_nearest(glm::vec2 v, glm::vec2 w, glm::vec2 p);

#endif //HELPERS_H
