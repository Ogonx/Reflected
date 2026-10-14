#include "Portal.h"

glm::vec4 portalClipPlane(const Portal& p) {

	glm::vec3 n = -p.normal;
	float d = -glm::dot(n, p.pos) - 0.01f;
	return glm::vec4(n, d);
}