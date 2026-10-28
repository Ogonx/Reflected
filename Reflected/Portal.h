#pragma once
#include <glm/glm.hpp>

struct Portal {
    glm::vec3 pos;
    glm::vec3 normal;
    float width;
    float height;
    int room;
    int linkedPortal;
};

glm::vec4 portalClipPlane(const Portal& p);
bool isInPortalOpening(const Portal& p, glm::vec3 pos);
bool isInPortalGap(const Portal& p, glm::vec3 pos);
unsigned int createPortalVAO(const Portal& p);
glm::mat4 portalFrame(const Portal& p);
glm::mat4 portalTransform(const Portal& from, const Portal& to);
glm::vec4 portalExitPlane(const Portal& p);