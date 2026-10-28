#include "Room.h"
#include <glm/gtc/matrix_transform.hpp>

static const float HALF = 8.0f;
static const float WALL_HEIGHT = 4.5f;

struct WallDef {
    glm::vec3 origin;
    glm::vec3 uAxis;
    glm::vec3 normal;
};

static void addQuad(std::vector<float>& verts, std::vector<unsigned int>& indices,
    glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d, glm::vec3 n)
{
    unsigned int start = (unsigned int)(verts.size() / 8);
    glm::vec3 corners[4] = { a, b, c, d };
    for (const glm::vec3& p : corners)
        verts.insert(verts.end(), { p.x, p.y, p.z, 0.0f, 0.0f, n.x, n.y, n.z });
    indices.insert(indices.end(), { start, start + 1, start + 2, start, start + 2, start + 3 });
}

static glm::vec3 wallPoint(const WallDef& w, float u, float y)
{
    return w.origin + w.uAxis * u + glm::vec3(0.0f, y, 0.0f);
}

static void addWall(std::vector<float>& verts, std::vector<unsigned int>& indices, const WallDef& w,
    bool hasHole, float holeMin, float holeMax, float holeTop)
{
    if (!hasHole)
    {
        addQuad(verts, indices, wallPoint(w, -HALF, 0.0f), wallPoint(w, HALF, 0.0f),
            wallPoint(w, HALF, WALL_HEIGHT), wallPoint(w, -HALF, WALL_HEIGHT), w.normal);
        return;
    }

    holeMin = glm::clamp(holeMin, -HALF, HALF);
    holeMax = glm::clamp(holeMax, -HALF, HALF);
    holeTop = glm::clamp(holeTop, 0.0f, WALL_HEIGHT);

    if (holeMin > -HALF)
        addQuad(verts, indices, wallPoint(w, -HALF, 0.0f), wallPoint(w, holeMin, 0.0f),
            wallPoint(w, holeMin, WALL_HEIGHT), wallPoint(w, -HALF, WALL_HEIGHT), w.normal);

    if (holeMax < HALF)
        addQuad(verts, indices, wallPoint(w, holeMax, 0.0f), wallPoint(w, HALF, 0.0f),
            wallPoint(w, HALF, WALL_HEIGHT), wallPoint(w, holeMax, WALL_HEIGHT), w.normal);

    if (holeTop < WALL_HEIGHT)
        addQuad(verts, indices, wallPoint(w, holeMin, holeTop), wallPoint(w, holeMax, holeTop),
            wallPoint(w, holeMax, WALL_HEIGHT), wallPoint(w, holeMin, WALL_HEIGHT), w.normal);
}

RoomMesh createRoomMesh(const Room& room, int roomIndex, const std::vector<Portal>& portals,
    unsigned int floorVAO, unsigned int floorTexture, unsigned int wallTexture)
{
    WallDef walls[4] = {
        { glm::vec3(0.0f, 0.0f, -HALF), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f,  1.0f) },
        { glm::vec3(0.0f, 0.0f,  HALF), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f) },
        { glm::vec3(-HALF, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f) },
        { glm::vec3(HALF, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f) }
    };

    std::vector<float> verts;
    std::vector<unsigned int> indices;

    for (const WallDef& w : walls)
    {
        bool hasHole = false;
        float holeMin = 0.0f, holeMax = 0.0f, holeTop = 0.0f;

        for (const Portal& p : portals)
        {
            if (p.room != roomIndex)
                continue;
            if (glm::dot(p.normal, w.normal) < 0.9f)
                continue;

            glm::vec3 local = (p.pos - room.position) / room.roomSize;
            if (glm::abs(glm::dot(local - w.origin, w.normal)) > 0.1f)
                continue;

            float centre = glm::dot(local, w.uAxis);
            float halfWidth = (p.width * 0.5f) / glm::dot(room.roomSize, w.uAxis);

            holeMin = centre - halfWidth;
            holeMax = centre + halfWidth;
            holeTop = p.height / room.roomSize.y;
            hasHole = true;
            break;
        }

        addWall(verts, indices, w, hasHole, holeMin, holeMax, holeTop);
    }

    addQuad(verts, indices,
        glm::vec3(-HALF, WALL_HEIGHT, -HALF), glm::vec3(HALF, WALL_HEIGHT, -HALF),
        glm::vec3(HALF, WALL_HEIGHT, HALF), glm::vec3(-HALF, WALL_HEIGHT, HALF),
        glm::vec3(0.0f, -1.0f, 0.0f));

    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), verts.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);

    return { floorVAO, VAO, (unsigned int)indices.size(), floorTexture, wallTexture };
}

void drawRoom(Shader& shader, const Room& room, const RoomMesh& mesh, const glm::mat4& offset)
{
    glm::mat4 roomMatrix = glm::scale(glm::translate(glm::mat4(1.0f), room.position), room.roomSize);

    shader.setMat4("roomModel", glm::scale(glm::mat4(1.0f), room.roomSize));
    shader.setMat4("model", offset * roomMatrix);
    shader.setVec3("lightPos", glm::vec3(offset * glm::vec4(room.lightPos, 1.0f)));
    shader.setVec3("lightColor", room.lightColor);
    shader.setFloat("grainAmount", room.grainAmount);
    shader.setInt("useTexture", 1);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mesh.floorTexture);
    glBindVertexArray(mesh.floorVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glBindTexture(GL_TEXTURE_2D, mesh.wallTexture);
    glBindVertexArray(mesh.wallsVAO);
    glDrawElements(GL_TRIANGLES, mesh.wallIndexCount, GL_UNSIGNED_INT, 0);
}

glm::vec3 clampToRoom(const Room& room, glm::vec3 pos, float margin)
{
    float halfX = HALF * room.roomSize.x;
    float halfZ = HALF * room.roomSize.z;

    pos.x = glm::clamp(pos.x, room.position.x - halfX + margin, room.position.x + halfX - margin);
    pos.z = glm::clamp(pos.z, room.position.z - halfZ + margin, room.position.z + halfZ - margin);

    return pos;
}