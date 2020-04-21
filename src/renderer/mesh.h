#pragma once

#include <vector>
#include <string>

#include "glm/glm.hpp"

#include "buf.h"

enum TMesh { meshTrisIndexed, meshTriStripIndexed };

struct TMeshRec {
	int indexStart;
	int indexSize;
};

struct TModelNode {
	std::string name;
	glm::mat4 matrix;
	std::vector<TMeshRec> meshes;
	std::vector<TModelNode> subNodes;
};


/** A class for storing vertices organised for drawing. */

class CMesh {
public:
	CMesh();
	void calculateVertexNormals();
	void exportToBuffer(CBuf& buf);
	void clear();

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<unsigned int> indices;
	std::vector<glm::vec2> texCoords;
	TMesh type;

	int frameCount;
	int frameIndices; ///<Number of vertices per frame.

};



