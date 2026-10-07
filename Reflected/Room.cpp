#include "Room.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>

void drawRoom(Shader& shader, const Room& room, const RoomMesh& mesh) {
    shader.setMat4("model", glm::scale(glm::translate(glm::mat4(1.0f), room.position), room.roomSize));
    shader.setVec3("lightPos", room.lightPos);
    shader.setVec3("lightColor", room.lightColor);
    shader.setFloat("grainAmount", room.grainAmount);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mesh.floorTexture);
    shader.setInt("useTexture", 1);
    glBindVertexArray(mesh.floorVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mesh.wallTexture);
    shader.setInt("useTexture", 1);
    glBindVertexArray(mesh.wallsVAO);
    glDrawElements(GL_TRIANGLES, 30, GL_UNSIGNED_INT, 0);
}
