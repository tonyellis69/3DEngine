#pragma once

#include <vector>

#include "glm/glm.hpp"

enum TMesh { meshTrisIndexed, meshTriStripIndexed };


/** A class for storing vertices organised for drawing. */
template<class T>
class CMesh {
public:


	std::vector<T> vertices;
	std::vector<T> normals;
	std::vector<unsigned int> indices;
	TMesh type;
};