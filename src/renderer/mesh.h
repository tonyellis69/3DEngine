#pragma once

#include <vector>
#include <string>

#include "glm/glm.hpp"



//#include "buf.h"
#include "buf2.h"

enum TMesh { meshTrisIndexed, meshTriStripIndexed };

/** Stores the data for drawing a mesh, but not 
	the mesh itself. */
struct TMeshRec {
	int indexStart;
	int indexSize;
	int vertStart;

	bool isLine = true;
	bool isLineLoop = false;
};

struct TMeshExtents {
	glm::vec3 furthestVert;
	glm::vec3 BBmin; 
	glm::vec3 BBmax;
};

/** Holds the reference data for a model or part of a multipart
	model, including all sub-parts. Does not store the actual vertices. */
struct TModelData {
	std::string name;
	glm::mat4 matrix;
	std::vector<TMeshRec> meshes;
	std::vector<TModelData> subModels;

	TMeshExtents extents;
};

struct TModelMesh {
	std::string name;
	glm::mat4 matrix;
	TMeshRec mesh;

	std::shared_ptr<CBuf2> buf;
	glm::vec4 colour = { 1.0f,1.0f,1.0f,1.0f }; 

};


/** A class for the intermediate storage and management of 
	the vertices of a model, on the PC-side. */
class CMesh {
public:
	CMesh();
	void calculateVertexNormals();
	//void exportToBuffer(CBuf& buf);
	void exportToBuffer(CBuf2& buf);
	CBuf2 exportToBuffer();
	void clear();
	void mergeUniqueVerts();
	void linesToLineStrip();
	void closeLineLoops(std::vector<unsigned int>& loopSizes);
	void addAdjacencyVerts();

	unsigned int makeAdjacencyVert(glm::vec3& A, glm::vec3& B);


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


