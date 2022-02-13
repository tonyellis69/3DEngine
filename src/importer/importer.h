#pragma once

#include <string>
#include <vector>

#include "glm/glm.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../renderer/mesh.h"
#include "model.h"

class CImporter {
public:
	void loadFile(const std::string& filename);
	//void addFrame(const std::string& filename);
	CMesh& getSingleMesh();
	TModelData getMeshNodes();
	CModel getModel();
	TDrawable getHexTile(std::vector<glm::vec4>& colours);

private:
	TModelData processNode(aiNode* node, const aiScene* scene);
	std::tuple<TMeshRec,TMeshExtents> processMesh(aiMesh* mesh, const aiNode* node);
	void copyMatrix(aiMatrix4x4& src, glm::mat4& dest);
	void updateNodeExtents(TModelData& node, TModelData& subNode);

	//std::vector<CMesh> meshes;

	CMesh singleMesh; ///<All the imported 3D model objects stored as one mesh object.

	TModelData rootNode; 

	std::vector<TModelMesh> modelMeshes;
};


