#include "Room.h"
#include <glm/gtc/matrix_transform.hpp>

void drawRoom(Shader& shader, const Room& room, const RoomMesh& mesh)
{
    shader.setMat4("model", glm::scale(glm::translate(glm::mat4(1.0f), room.position), room.roomSize));
    shader.setVec3("lightPos", room.lightPos);
    shader.setVec3("lightColor", room.lightColor);
    shader.setFloat("grainAmount", room.grainAmount);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mesh.floorTexture);
    shader.setInt("useTexture", 1);
    glBindVertexArray(mesh.floorVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glBindTexture(GL_TEXTURE_2D, mesh.wallTexture);
    glBindVertexArray(mesh.wallsVAO);
    glDrawElements(GL_TRIANGLES, 30, GL_UNSIGNED_INT, 0);
}

glm::vec3 clampToRoom(const Room& room, glm::vec3 pos, float margin)
{
    float halfX = 8.0f * room.roomSize.x;
    float halfZ = 8.0f * room.roomSize.z;

    pos.x = glm::clamp(pos.x, room.position.x - halfX + margin, room.position.x + halfX - margin);
    pos.z = glm::clamp(pos.z, room.position.z - halfZ + margin, room.position.z + halfZ - margin);

    return pos;
}

float roomMinX(const Room& room)
{
    return room.position.x - 8.0f * room.roomSize.x;
}

float roomMaxX(const Room& room)
{
    return room.position.x + 8.0f * room.roomSize.x;
}