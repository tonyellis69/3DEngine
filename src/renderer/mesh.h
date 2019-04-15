#pragma once

#include <vector>

#include "glm/glm.hpp"

enum TMesh { meshTrisIndexed, meshTriStripIndexed };


/** A class for storing vertices organised for drawing. */
template<class T>
class CMesh {
public:
	void calculateVertexNormals();

	std::vector<T> vertices;
	std::vector<T> normals;
	std::vector<unsigned int> indices;
	std::vector<glm::vec2> texCoords;
	TMesh type;
};

template<class T>
inline void CMesh<T>::calculateVertexNormals() {
	normals.resize(vertices.size());
	//for each face, find the normal,
	//make that the normal for the verts
	for (unsigned int v = 0; v < vertices.size(); v += 3) {
		T edge1 = vertices[indices[v]] - vertices[indices[v + 1]];
		T edge2 = vertices[indices[v+2] ] - vertices[indices[v+1]];
		T normal = glm::cross(edge2, edge1);
		normals[indices[v]] = normals[indices[v + 1]] = normals[indices[v + 2]] = normal;

	}
}
