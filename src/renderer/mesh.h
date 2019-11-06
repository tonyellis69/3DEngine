#pragma once

#include <vector>

#include "glm/glm.hpp"

#include "buf.h"

enum TMesh { meshTrisIndexed, meshTriStripIndexed };


/** A class for storing vertices organised for drawing. */

class CMesh {
public:
	void calculateVertexNormals();
	void writeToBufferv3i(CBuf& buf);

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<unsigned int> indices;
	std::vector<glm::vec2> texCoords;
	TMesh type;
};



