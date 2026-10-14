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