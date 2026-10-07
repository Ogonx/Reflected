#pragma once
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

struct Camera {
    glm::vec3 pos;
    glm::vec3 front;
    glm::vec3 up;
    float yaw;
    float pitch;
    float lastX;
    float lastY;
    bool firstMouse;
    float fov;
};

extern Camera camera;

void mouse_callback(GLFWwindow* window, double xpos, double ypos);