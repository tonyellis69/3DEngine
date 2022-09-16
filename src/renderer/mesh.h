#pragma once

#include <vector>
#include <string>

#include "glm/glm.hpp"



//#include "buf.h"
#include "buf2.h"

enum TMesh { meshTrisIndexed, meshTriStripIndexed };

struct vc {
	glm::vec3 v;
	int c;
};

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
//
//struct TDrawable{
//	//TMeshRec meshRec;
//	//std::shared_ptr<CBuf2> buf;
//};

//struct TMultiDrawable {
//	std::vector<TMeshRec> meshes;
////std::shared_ptr<CBuf2> buf;
//};

struct TVertData {
	std::vector<TMeshRec> meshes;
	std::vector<glm::vec3> vertices;
	std::vector<unsigned int> indices;
};

struct TModelMesh {
	std::string name;
	glm::mat4 matrix;
	TMeshRec meshRec;
	//TDrawable draw;


	glm::vec4 colour = { 1.0f,1.0f,1.0f,1.0f }; //TO DO: scrap
	std::vector<glm::vec4> palette = { { 1.0f,1.0f,1.0f,1.0f } };
	std::vector<glm::vec4>* pPalette;
};



/** A class for the intermediate storage and management of 
	the vertices of a model, on the PC-side. */
class CMesh {
public:
	CMesh();
	void calculateVertexNormals();
	void exportToBuffer(CBuf2& buf);
	void clear();
	void mergeUniqueVerts();
	void linesToLineStrip();
	void closeLineLoops(std::vector<unsigned int>& loopSizes);
	void addAdjacencyVerts();

	TMeshRec add(CMesh& mesh);


	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<unsigned int> indices;
	std::vector<unsigned int> vertColours;
	std::vector<glm::vec2> texCoords;

	TMesh type;

private:
	unsigned int makeAdjacencyVert(glm::vec3& A, glm::vec3& B);
	void orderLines();

};


