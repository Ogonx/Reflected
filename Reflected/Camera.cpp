#include "Camera.h"
#include <GLFW/glfw3.h>


Camera camera = {
    glm::vec3(0.0f, 0.0f, 3.0f),
    glm::vec3(0.0f, 0.0f, -1.0f),
    glm::vec3(0.0f, 1.0f, 0.0f),
    -90.0f,
    0.0f,
    960.0f,
    540.0f,
    true,
    90.0f
};

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (camera.firstMouse) {
        camera.lastX = (float)xpos;
        camera.lastY = (float)ypos;
        camera.firstMouse = false;
    }

    float xoffset = (float)xpos - camera.lastX;
    float yoffset = camera.lastY - (float)ypos;
    camera.lastX = (float)xpos;
    camera.lastY = (float)ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    camera.yaw += xoffset;
    camera.pitch += yoffset;

    if (camera.pitch > 89.0f) camera.pitch = 89.0f;
    if (camera.pitch < -89.0f) camera.pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
    direction.y = sin(glm::radians(camera.pitch));
    direction.z = sin(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
    camera.front = glm::normalize(direction);
}