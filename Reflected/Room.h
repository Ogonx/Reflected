#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "Header.h"
#include "Portal.h"

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
    unsigned int wallIndexCount;
    unsigned int floorTexture;
    unsigned int wallTexture;
};

void drawRoom(Shader& shader, const Room& room, const RoomMesh& mesh, const glm::mat4& offset = glm::mat4(1.0f));
glm::vec3 clampToRoom(const Room& room, glm::vec3 pos, float margin);
RoomMesh createRoomMesh(const Room& room, int roomIndex, const std::vector<Portal>& portals,
    unsigned int floorVAO, unsigned int floorTexture, unsigned int wallTexture);