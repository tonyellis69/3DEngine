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
	TModelData getMeshNodes();

private:
	TModelData processNode(aiNode* node, const aiScene* scene);
	std::tuple<TMeshRec,TMeshExtents> processMesh(aiMesh* mesh, const aiNode* node);
	void copyMatrix(aiMatrix4x4& src, glm::mat4& dest);
	void updateNodeExtents(TModelData& node, TModelData& subNode);

	//std::vector<CMesh> meshes;

	CMesh singleMesh; ///<All the imported 3D model objects stored as one mesh object.

	TModelData rootNode; 
};


