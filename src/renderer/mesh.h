#pragma once

#include <vector>
#include <string>

#include "glm/glm.hpp"



#include "buf.h"
#include "buf2.h"

enum TMesh { meshTrisIndexed, meshTriStripIndexed };

struct TMeshRec {
	int indexStart;
	int indexSize;
	int vertStart;
	glm::vec4 colour;
	bool isLine = true;
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
	void exportToBuffer(CBuf2& buf);
	void clear();
	void mergeUniqueVerts();
	void linesToLineStrip();
	void addAdjacencyVerts();

	TMeshRec add(CMesh& mesh);

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<unsigned int> indices;
	std::vector<glm::vec2> texCoords;
	//std::vector<glm::i32vec3> triangles;
	TMesh type;

private:
	void orderLines();

};


