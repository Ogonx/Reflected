#pragma once
#include <glm/glm.hpp>
#include "Header.h"

struct Room {
    glm::vec3 position;
    glm::vec3 roomSize;
    glm::vec3 lightPos;
    glm::vec3 lightColor;
    float grainAmount;
};

struct RoomMesh {
    unsigned int floorVAO;
    unsigned int wallsVAO;
    unsigned int floorTexture;
    unsigned int wallTexture;
};

void drawRoom(Shader& shader, const Room& room, const RoomMesh& mesh);