#pragma once

#include <vector>

#include <glm/glm.hpp>

class CSimpleMesh {
public:

	std::vector<glm::vec3> verts;
	std::vector<int> indices;
};