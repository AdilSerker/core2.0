#include "Camera.h"
#include <iostream>
#include <GLFW/glfw3.h>
extern GLFWwindow *window;

Camera::Camera() : position(glm::vec3(0, 10, 10)),
                   horizontalAngle(M_PI),
                   verticalAngle(0.0f),
                   initialFoV(45.0f),
                   speed(8.0f),
                   mouseSpeed(0.005f)
{
    ViewMatrix = glm::lookAt(
        glm::vec3(0.0f, 10.35f, 0.85f),
        glm::vec3(0.0f, -0.25f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f));
}

void Camera::computeMatricesFromInputs()
{
    const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    int WINDOW_WIDTH = mode->width;
    int WINDOW_HEIGHT = mode->height;

    // glfwGetTime is called only once, the first time this function is called
    static double lastTime = glfwGetTime();

    // Compute time difference between current and last frame
    double currentTime = glfwGetTime();
    float deltaTime = float(currentTime - lastTime);

    // // Get mouse position
    // double xpos, ypos;
    // glfwGetCursorPos(window, &xpos, &ypos);

    // // Compute new orientation
    // horizontalAngle += mouseSpeed * float(WINDOW_WIDTH / 2 - xpos);
    // verticalAngle += mouseSpeed * float(WINDOW_HEIGHT / 2 - ypos);

    // glfwSetCursorPos(window, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        verticalAngle += deltaTime * speed * 0.5;
    }
    // Move backward
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        verticalAngle -= deltaTime * speed * 0.5;
    }
    // Strafe right
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        horizontalAngle += deltaTime * speed * 0.5;
    }
    // Strafe left
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        horizontalAngle -= deltaTime * speed * 0.5;
    }

    // Direction : Spherical coordinates to Cartesian coordinates conversion
    glm::vec3 direction(
        cos(verticalAngle) * sin(horizontalAngle),
        sin(verticalAngle),
        cos(verticalAngle) * cos(horizontalAngle));

    // Right vector
    glm::vec3 right = glm::vec3(
        sin(horizontalAngle - M_PI / 2.0f),
        0,
        cos(horizontalAngle - M_PI / 2.0f));

    // Up vector
    glm::vec3 up = glm::cross(right, direction);

    // Move forward
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        position += direction * deltaTime * (speed + mouseSpeed);
    }
    // Move backward
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        position -= direction * deltaTime * speed;
    }
    // Strafe right
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        position += right * deltaTime * speed;
    }
    // Strafe left
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        position -= right * deltaTime * speed;
    }
    // Strafe up
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
    {
        position += up * deltaTime * speed;
    }
    // Strafe down
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
    {
        position -= up * deltaTime * speed;
    }

    float FoV = initialFoV; // - 5 * glfwGetMouseWheel(); // Now GLFW 3 requires setting up a callback for this. It's a bit too complicated for this beginner's tutorial, so it's disabled instead.

    // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
    ProjectionMatrix = glm::perspective(glm::radians(FoV), WINDOW_WIDTH * 1.0f / WINDOW_HEIGHT, 0.1f, 90.0f);
    // Camera matrix
    ViewMatrix = glm::lookAt(
        position,             // Camera is here
        position + direction, // and looks here : at the same position, plus "direction"
        up                    // Head is up (set to 0,-1,0 to look upside-down)
    );

    // For the next frame, the "last time" will be "now"
    lastTime = currentTime;
}