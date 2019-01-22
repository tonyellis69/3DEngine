#pragma once

#include <vector>

#include "glm/glm.hpp"

enum TMesh { meshTrisIndexed, meshTriStripIndexed };


/** A class for storing vertices organised for drawing. */
class CMesh {
public:


	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<unsigned int> indices;
	TMesh type;
};