#pragma once

#include <string>
#include <vector>

#include "glm/glm.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../renderer/mesh.h"

class CImporter {
public:
	void loadFile(const std::string& filename);
	//void addFrame(const std::string& filename);
	CMesh& getSingleMesh();
	TModelNode getMeshNodes();

private:
	TModelNode processNode(aiNode* node, const aiScene* scene);
	std::tuple<TMeshRec,TMeshExtents> processMesh(aiMesh* mesh, const aiNode* node);
	void copyMatrix(aiMatrix4x4& src, glm::mat4& dest);
	void updateNodeExtents(TModelNode& node, TModelNode& subNode);

	//std::vector<CMesh> meshes;

	CMesh singleMesh; ///<All the imported 3D model objects stored as one mesh object.

	TModelNode rootNode; 
};


