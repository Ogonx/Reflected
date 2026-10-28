#include <glad/glad.h>
#include "Portal.h"
#include <glm/gtc/matrix_transform.hpp>

glm::vec4 portalClipPlane(const Portal& p)
{
    glm::vec3 n = -p.normal;
    float d = -glm::dot(n, p.pos) - 0.01f;
    return glm::vec4(n, d);
}

bool isInPortalOpening(const Portal& p, glm::vec3 pos)
{
    glm::vec3 side = glm::normalize(glm::cross(p.normal, glm::vec3(0.0f, 1.0f, 0.0f)));
    float sideDist = glm::dot(pos - p.pos, side);
    return glm::abs(sideDist) < p.width * 0.5f;
}

bool isInPortalGap(const Portal& p, glm::vec3 pos)
{
    float frontDist = glm::dot(pos - p.pos, p.normal);
    return isInPortalOpening(p, pos) && glm::abs(frontDist) < 0.6f;
}

unsigned int createPortalVAO(const Portal& p)
{
    glm::vec3 side = glm::normalize(glm::cross(p.normal, glm::vec3(0.0f, 1.0f, 0.0f)));
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    glm::vec3 bottomLeft = p.pos - side * (p.width * 0.5f);
    glm::vec3 bottomRight = p.pos + side * (p.width * 0.5f);
    glm::vec3 topLeft = bottomLeft + up * p.height;
    glm::vec3 topRight = bottomRight + up * p.height;

    float vertices[] = {
        bottomLeft.x,  bottomLeft.y,  bottomLeft.z,  0.0f, 0.0f,  p.normal.x, p.normal.y, p.normal.z,
        bottomRight.x, bottomRight.y, bottomRight.z, 1.0f, 0.0f,  p.normal.x, p.normal.y, p.normal.z,
        topRight.x,    topRight.y,    topRight.z,    1.0f, 1.0f,  p.normal.x, p.normal.y, p.normal.z,
        topLeft.x,     topLeft.y,     topLeft.z,     0.0f, 1.0f,  p.normal.x, p.normal.y, p.normal.z
    };

    unsigned int indices[] = { 0, 1, 2, 0, 2, 3 };

    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
    return VAO;
}

glm::mat4 portalFrame(const Portal& p) {

    glm::vec3 out = p.normal;
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 side = glm::normalize(glm::cross(up, out));

    glm::mat4 frame(1.0f);
    frame[0] = glm::vec4(side, 0.0f);
    frame[1] = glm::vec4(up, 0.0f);
    frame[2] = glm::vec4(out, 0.0f);
    frame[3] = glm::vec4(p.pos, 1.0f);
    return frame;
}

glm::mat4 portalTransform(const Portal& from, const Portal& to)
{
    glm::mat4 turnAround = glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    return portalFrame(to) * turnAround * glm::inverse(portalFrame(from));
}

glm::vec4 portalExitPlane(const Portal& p)
{
    float d = -glm::dot(p.normal, p.pos) - 0.01f;
    return glm::vec4(p.normal, d);
}