#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Header.h"
#include <direct.h>
#include "stb_image.h"

// window
const unsigned int WIDTH = 1920;
const unsigned int HEIGHT = 1080;

// camera
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float yaw = -90.0f;
float pitch = 0.0f;
float lastX = WIDTH / 2.0f;
float lastY = HEIGHT / 2.0f;

bool firstMouse = true;
float fov = 90.0f;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse) {
        lastX = (float)xpos;
        lastY = (float)ypos;
        firstMouse = false;
    }

    float xoffset = (float)xpos - lastX;
    float yoffset = lastY - (float)ypos;
    lastX = (float)xpos;
    lastY = (float)ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(direction);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    const float cameraSpeed = 5.0f * deltaTime;
    const float cameraSpeedS = 2.5f * deltaTime;

    glm::vec3 front = cameraFront;
    front.y = 0.0f;
    front = glm::normalize(front);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * front;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeedS * front;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(front, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(front, cameraUp)) * cameraSpeed;

    cameraPos.y = 1.3f;
    float margin = 0.3f; // how close u can get to a wall
    cameraPos.x = glm::clamp(cameraPos.x, -7.7f + margin, 7.7f - margin);
    cameraPos.z = glm::clamp(cameraPos.z, -7.7f + margin, 7.7f - margin);
}



int main()
{
    _chdir("D:\\Reflected\\Reflected");

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Reflected", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);

    float vertices[] = {
        // FLOOR (normal pointing up)
        -8.0f, 0.0f, -8.0f,  0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
         8.0f, 0.0f, -8.0f,  2.0f, 0.0f,  0.0f, 1.0f, 0.0f,
         8.0f, 0.0f,  8.0f,  2.0f, 2.0f,  0.0f, 1.0f, 0.0f,
        -8.0f, 0.0f,  8.0f,  0.0f, 2.0f,  0.0f, 1.0f, 0.0f,

        // BACK WALL
        -8.0f, 0.0f, -8.0f,  0.0f, 0.0f,  0.0f, 0.0f, 1.0f,
         8.0f, 0.0f, -8.0f,  2.0f, 0.0f,  0.0f, 0.0f, 1.0f,
         8.0f, 4.5f, -8.0f,  2.0f, 2.0f,  0.0f, 0.0f, 1.0f,
        -8.0f, 4.5f, -8.0f,  0.0f, 2.0f,  0.0f, 0.0f, 1.0f,

        // FRONT WALL
        -8.0f, 0.0f,  8.0f,  0.0f, 0.0f,  0.0f, 0.0f, -1.0f,
         8.0f, 0.0f,  8.0f,  2.0f, 0.0f,  0.0f, 0.0f, -1.0f,
         8.0f, 4.5f,  8.0f,  2.0f, 2.0f,  0.0f, 0.0f, -1.0f,
        -8.0f, 4.5f,  8.0f,  0.0f, 2.0f,  0.0f, 0.0f, -1.0f,

        // LEFT WALL
        -8.0f, 0.0f,  8.0f,  0.0f, 0.0f,  1.0f, 0.0f, 0.0f,
        -8.0f, 0.0f, -8.0f,  2.0f, 0.0f,  1.0f, 0.0f, 0.0f,
        -8.0f, 4.5f, -8.0f,  2.0f, 2.0f,  1.0f, 0.0f, 0.0f,
        -8.0f, 4.5f,  8.0f,  0.0f, 2.0f,  1.0f, 0.0f, 0.0f,

        // RIGHT WALL
         8.0f, 0.0f, -8.0f,  0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
         8.0f, 0.0f,  8.0f,  2.0f, 0.0f, -1.0f, 0.0f, 0.0f,
         8.0f, 4.5f,  8.0f,  2.0f, 2.0f, -1.0f, 0.0f, 0.0f,
         8.0f, 4.5f, -8.0f,  0.0f, 2.0f, -1.0f, 0.0f, 0.0f,

         // CEILING
         -8.0f, 4.5f, -8.0f,  0.0f, 0.0f,  0.0f, -1.0f, 0.0f,
          8.0f, 4.5f, -8.0f,  2.0f, 0.0f,  0.0f, -1.0f, 0.0f,
          8.0f, 4.5f,  8.0f,  2.0f, 2.0f,  0.0f, -1.0f, 0.0f,
         -8.0f, 4.5f,  8.0f,  0.0f, 2.0f,  0.0f, -1.0f, 0.0f,
    };

    float mirrorVertices[] = {
        // position          // texcoord  // normal
        8.0f, 1.0f, -1.0f,  0.0f, 0.0f,  -1.0f, 0.0f, 0.0f,  // bottom left
        8.0f, 1.0f,  1.0f,  1.0f, 0.0f,  -1.0f, 0.0f, 0.0f,  // bottom right
        8.0f, 3.5f,  1.0f,  1.0f, 1.0f,  -1.0f, 0.0f, 0.0f,  // top right
        8.0f, 3.5f, -1.0f,  0.0f, 1.0f,  -1.0f, 0.0f, 0.0f   // top left
    };

    unsigned int mirrorIndices[] = {
        0, 1, 2,
        0, 2, 3
    };

    unsigned int floorIndices[] = {
        0, 1, 2,  0, 2, 3
    };

    unsigned int wallIndices[] = {
        4,  5,  6,   4,  6,  7,
        8,  9,  10,  8,  10, 11,
        12, 13, 14,  12, 14, 15,
        16, 17, 18,  16, 18, 19,
        20, 21, 22,  20, 22, 23
    };

    Shader roomShader("shader.vs", "shader.fs");

    unsigned int VBO;
    unsigned int floorVAO, floorEBO;
    unsigned int wallsVAO, wallsEBO;
    unsigned int mirrorVAO, mirrorVBO, mirrorEBO;

    // shared VBO
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // floor VAO
    glGenVertexArrays(1, &floorVAO);
    glGenBuffers(1, &floorEBO);
    glBindVertexArray(floorVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, floorEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(floorIndices), floorIndices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);

    // walls VAO
    glGenVertexArrays(1, &wallsVAO);
    glGenBuffers(1, &wallsEBO);
    glBindVertexArray(wallsVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, wallsEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(wallIndices), wallIndices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);

    // mirror VAO
    glGenVertexArrays(1, &mirrorVAO);
    glGenBuffers(1, &mirrorVBO);
    glGenBuffers(1, &mirrorEBO);
    glBindVertexArray(mirrorVAO);
    glBindBuffer(GL_ARRAY_BUFFER, mirrorVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(mirrorVertices), mirrorVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mirrorEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(mirrorIndices), mirrorIndices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);

    // floor texture
    unsigned int floorTexture;
    glGenTextures(1, &floorTexture);
    glBindTexture(GL_TEXTURE_2D, floorTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_set_flip_vertically_on_load(true);
    int width, height, nrChannels;
    unsigned char* data = stbi_load("floor.jpg", &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "Failed to load floor texture" << std::endl;
    }
    stbi_image_free(data);

    // wall texture
    unsigned int wallTexture;
    glGenTextures(1, &wallTexture);
    glBindTexture(GL_TEXTURE_2D, wallTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    data = stbi_load("walls.jpg", &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "Failed to load wall texture" << std::endl;
    }
    stbi_image_free(data);

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        roomShader.use();
        roomShader.setMat4("projection", glm::perspective(glm::radians(fov), (float)WIDTH / HEIGHT, 0.1f, 100.0f));
        roomShader.setMat4("view", glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp));
        roomShader.setMat4("model", glm::mat4(1.0f));
        roomShader.setVec3("lightPos", glm::vec3(0.0f, 4.2f, 0.0f));
        roomShader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
        roomShader.setVec3("viewPos", cameraPos);
        roomShader.setInt("texture1", 0);

        // ---- PASS 1 - mark mirror shape in stencil buffer ----
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
        glDepthMask(GL_FALSE);
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        glBindVertexArray(mirrorVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        glDepthMask(GL_TRUE);

        // pass 2 - draw mirror world
        glStencilFunc(GL_EQUAL, 1, 0xFF);
        glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

        // reflect view around the mirror plane x=8
      
        glm::vec3 reflectedCameraPos;
        reflectedCameraPos.x = -16.0f - cameraPos.x;
        reflectedCameraPos.y = cameraPos.y;
        reflectedCameraPos.z = cameraPos.z;

        glm::vec3 reflectedFront = cameraFront;
        reflectedFront.x = -reflectedFront.x;

        glm::mat4 mirrorView = glm::lookAt(
            reflectedCameraPos,
            reflectedCameraPos + reflectedFront,
            cameraUp
        );

        roomShader.setMat4("view", mirrorView);
        roomShader.setMat4("model", glm::mat4(1.0f)); // normal model matrix

        // draw floor
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, floorTexture);
        roomShader.setInt("useTexture", 1);
        glBindVertexArray(floorVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // draw walls
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, wallTexture);
        roomShader.setInt("useTexture", 1);
        glBindVertexArray(wallsVAO);
        glDrawElements(GL_TRIANGLES, 30, GL_UNSIGNED_INT, 0);

        // ---- PASS 3 - draw normal room ----
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

        roomShader.setMat4("view", glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, floorTexture);
        roomShader.setInt("useTexture", 1);
        glBindVertexArray(floorVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, wallTexture);
        roomShader.setInt("useTexture", 1);
        glBindVertexArray(wallsVAO);
        glDrawElements(GL_TRIANGLES, 30, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
