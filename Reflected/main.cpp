#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <direct.h>
#include "Header.h"
#include "Texture.h"
#include "Room.h"
#include "Camera.h"
#include "Portal.h"

const unsigned int WIDTH = 1920;
const unsigned int HEIGHT = 1080;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

int currentRoom = 0;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    const float cameraSpeed = 5.0f * deltaTime;
    const float cameraSpeedS = 2.5f * deltaTime;

    glm::vec3 front = camera.front;
    front.y = 0.0f;
    front = glm::normalize(front);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.pos += cameraSpeed * front;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.pos -= cameraSpeedS * front;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.pos -= glm::normalize(glm::cross(front, camera.up)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.pos += glm::normalize(glm::cross(front, camera.up)) * cameraSpeed;

    camera.pos.y = 1.3f;
}

void updateRoom(const std::vector<Portal>& portals)
{
    for (const Portal& p : portals)
    {
        if (p.room != currentRoom)
            continue;

        float frontDist = glm::dot(camera.pos - p.pos, p.normal);

        if (isInPortalOpening(p, camera.pos) && frontDist < 0.0f)
        {
            const Portal& exit = portals[p.linkedPortal];
            glm::mat4 throughPortal = portalTransform(p, exit);

            camera.pos = glm::vec3(throughPortal * glm::vec4(camera.pos, 1.0f));
            camera.front = glm::normalize(glm::vec3(throughPortal * glm::vec4(camera.front, 0.0f)));
            camera.yaw = glm::degrees(atan2(camera.front.z, camera.front.x));

            currentRoom = exit.room;
            return;
        }
    }
}

void updateCollision(const std::vector<Room>& rooms, const std::vector<Portal>& portals)
{
    const float margin = 0.3f;
    glm::vec3 clamped = clampToRoom(rooms[currentRoom], camera.pos, margin);

    for (const Portal& p : portals)
    {
        if (p.room != currentRoom)
            continue;

        float frontDist = glm::dot(camera.pos - p.pos, p.normal);

        if (isInPortalOpening(p, camera.pos) && frontDist < 1.0f)
        {
            float wanted = glm::dot(camera.pos, p.normal);
            float actual = glm::dot(clamped, p.normal);
            clamped += p.normal * (wanted - actual);
        }
    }

    camera.pos = clamped;
}

glm::mat4 obliqueProjection(glm::mat4 proj, glm::vec4 clipPlane)
{
    glm::vec4 q;
    glm::vec4 c;

    q.x = (glm::sign(clipPlane.x) + proj[2][0]) / proj[0][0];
    q.y = (glm::sign(clipPlane.y) + proj[2][1]) / proj[1][1];
    q.z = -1.0f;
    q.w = (1.0f + proj[2][2]) / proj[3][2];

    c = clipPlane * (2.0f / glm::dot(clipPlane, q));

    proj[0][2] = c.x;
    proj[1][2] = c.y;
    proj[2][2] = c.z + 1.0f;
    proj[3][2] = c.w;

    return proj;
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

    float floorVertices[] = {
        -8.0f, 0.0f, -8.0f,  0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
         8.0f, 0.0f, -8.0f,  2.0f, 0.0f,  0.0f, 1.0f, 0.0f,
         8.0f, 0.0f,  8.0f,  2.0f, 2.0f,  0.0f, 1.0f, 0.0f,
        -8.0f, 0.0f,  8.0f,  0.0f, 2.0f,  0.0f, 1.0f, 0.0f
    };

    unsigned int floorIndices[] = {
        0, 1, 2,  0, 2, 3
    };

    Shader roomShader("shader.vs", "shader.fs");

    unsigned int floorVAO, floorVBO, floorEBO;
    glGenVertexArrays(1, &floorVAO);
    glGenBuffers(1, &floorVBO);
    glGenBuffers(1, &floorEBO);
    glBindVertexArray(floorVAO);
    glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), floorVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, floorEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(floorIndices), floorIndices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);

    unsigned int floorTexture = loadTexture("floor.jpg");
    unsigned int wallTexture = loadTexture("walls.jpg");

    std::vector<Room> rooms = {
        { glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 4.2f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 0.0f },
        { glm::vec3(0.0f, 0.0f, 60.0f), glm::vec3(1.0f, 2.0f, 1.0f), glm::vec3(0.0f, 4.2f, 60.0f), glm::vec3(0.35f, 0.35f, 0.4f), 0.08f }
    };

    std::vector<Portal> portals = {
        { glm::vec3(8.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), 2.0f, 3.5f, 0, 1 },
        { glm::vec3(0.0f, 0.0f, 52.0f), glm::vec3(0.0f, 0.0f, 1.0f), 2.0f, 3.5f, 1, 0 }
    };

    std::vector<RoomMesh> meshes;
    for (int i = 0; i < (int)rooms.size(); i++)
        meshes.push_back(createRoomMesh(rooms[i], i, portals, floorVAO, floorTexture, wallTexture));

    std::vector<unsigned int> portalVAOs;
    for (const Portal& p : portals)
        portalVAOs.push_back(createPortalVAO(p));

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);
        updateRoom(portals);
        updateCollision(rooms, portals);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        glm::mat4 projection = glm::perspective(glm::radians(camera.fov), (float)WIDTH / HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = glm::lookAt(camera.pos, camera.pos + camera.front, camera.up);

        roomShader.use();
        roomShader.setMat4("projection", projection);
        roomShader.setMat4("view", view);
        roomShader.setVec3("viewPos", camera.pos);
        roomShader.setInt("texture1", 0);
        roomShader.setFloat("time", currentFrame);

        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
        glDepthMask(GL_FALSE);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        roomShader.setMat4("model", glm::mat4(1.0f));

        for (int i = 0; i < (int)portals.size(); i++)
        {
            if (portals[i].room != currentRoom)
                continue;

            glStencilFunc(GL_ALWAYS, i + 1, 0xFF);
            glBindVertexArray(portalVAOs[i]);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }

        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        glDepthMask(GL_TRUE);
        glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

        for (int i = 0; i < (int)portals.size(); i++)
        {
            const Portal& p = portals[i];
            if (p.room != currentRoom)
                continue;

            const Portal& exit = portals[p.linkedPortal];
            glm::mat4 throughPortal = portalTransform(p, exit);
            glm::mat4 portalView = view * glm::inverse(throughPortal);

            glStencilFunc(GL_EQUAL, i + 1, 0xFF);
            roomShader.setMat4("view", portalView);

            glm::vec4 viewPlane = glm::transpose(glm::inverse(portalView)) * portalExitPlane(exit);
            roomShader.setMat4("projection", obliqueProjection(projection, viewPlane));

            drawRoom(roomShader, rooms[exit.room], meshes[exit.room]);
        }

        roomShader.setMat4("view", view);

        glStencilFunc(GL_EQUAL, 0, 0xFF);
        roomShader.setMat4("projection", projection);
        drawRoom(roomShader, rooms[currentRoom], meshes[currentRoom]);

        for (const Portal& p : portals)
        {
            if (p.room == currentRoom && isInPortalGap(p, camera.pos))
            {
                const Portal& exit = portals[p.linkedPortal];
                glm::mat4 backThrough = glm::inverse(portalTransform(p, exit));
                drawRoom(roomShader, rooms[exit.room], meshes[exit.room], backThrough);
            }
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}