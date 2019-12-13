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
	std::vector<CMesh>& CImporter::getMeshes();
	CMesh& getSingleMesh();

private:
	void processNode(aiNode* node, const aiScene* scene);
	CMesh processMesh(aiMesh* mesh, const aiScene* scene);
	

	std::vector<CMesh> meshes;

	CMesh singleMesh; ///<All the imported 3D model objects stored as one mesh object.
};


