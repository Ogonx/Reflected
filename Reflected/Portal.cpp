#include "Portal.h"

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