#include "importer.h"

#include <string.h>

/** Load a line-based model and write it to the given buffer. */
void CImporter::loadFile(const std::string& filename) {
	Assimp::Importer importer;
	//rootRec = NULL;
	singleMesh.clear();


	const aiScene* scene = importer.ReadFile(filename , /*aiProcess_JoinIdenticalVertices |*/ 0   /*aiProcess_Triangulate*/) ;
	
	aiNode* startNode = scene->mRootNode;
	if (startNode->mNumMeshes == 0 && startNode->mNumChildren == 1)
		startNode = startNode->mChildren[0];
	
	rootNode = processNode(startNode, scene);

	//Post-processing should happen here, now we have a finished model of one
	//or more nodes
	//singleMesh holds all the verts


}



/** Return a single mesh composed of all the objects imported. */
CMesh& CImporter::getSingleMesh() {
	return singleMesh;
}

TModelData CImporter::getMeshNodes() {
	return rootNode;
}

/** Create a model from the imported scene and return it. */
CModel CImporter::getModel() {
	CModel model;

	auto buf = std::make_shared<CBuf2>();
	singleMesh.exportToBuffer(*buf);

	for (auto& mesh : modelMeshes) {
		model.meshes.push_back(mesh);
		model.meshes.back().buf = buf;
	}
	
	model.extents = rootNode.extents; //temp!
	return model;
}

/** Recursively retrieve all the verts etc from each node in the scene. */
TModelData CImporter::processNode(aiNode* node, const aiScene* scene) {
	TModelData currentNode;
	currentNode.name = node->mName.C_Str();
	copyMatrix(node->mTransformation,currentNode.matrix);

	for (unsigned int m = 0; m < node->mNumMeshes; m++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[m]];
		auto[meshRec,extents] = processMesh(mesh, node);
		currentNode.meshes.push_back(meshRec);
		currentNode.extents = extents;
		TModelMesh modelMesh = { node->mName.C_Str(),currentNode.matrix,meshRec };
		modelMeshes.push_back(modelMesh);
	}

	for (unsigned int n = 0; n < node->mNumChildren; n++) {
		TModelData newNode = processNode(node->mChildren[n], scene);
		currentNode.subModels.push_back(newNode);

		updateNodeExtents(currentNode, newNode);
	}
	

	return currentNode;
}

/**	Return a mesh object holding all the data from this assimp mesh. */
std::tuple<TMeshRec, TMeshExtents> CImporter::processMesh(aiMesh* mesh, const aiNode* node) {
	CMesh localMesh;
	TMeshExtents localExtents;

	//get verts
	for (unsigned int v = 0; v < mesh->mNumVertices; v++) {
		glm::vec3 vert = { mesh->mVertices[v].x, mesh->mVertices[v].y, mesh->mVertices[v].z };
		localMesh.vertices.push_back(vert);
		//normals and texture coords go here
		localExtents.furthestVert = glm::max(localExtents.furthestVert, vert);
		localExtents.BBmin = glm::min(localExtents.BBmin, vert);
		localExtents.BBmax = glm::max(localExtents.BBmax, vert);
	}

	//get indices to verts
	for (unsigned int f = 0; f < mesh->mNumFaces; f++) {
		aiFace face = mesh->mFaces[f];
		for (unsigned int i = 0; i < face.mNumIndices; i++) {
			localMesh.indices.push_back(face.mIndices[i]);
		}
	}

	bool isLine;
	bool isLineLoop = false;
	//if (std::string(mesh->mName.C_Str()).find("solid") != std::string::npos)
	if (std::string(node->mName.C_Str()).find("solid") != std::string::npos)
		isLine = false;
	else {
		isLine = true;
		if (std::string(mesh->mName.C_Str()).find("line_loop") != std::string::npos) {
			isLineLoop = true;
			if (mesh->mNumFaces > 1) {
				std::vector<unsigned int> loopSizes;
				for (unsigned int f = 0; f < mesh->mNumFaces-1; f++)
					loopSizes.push_back(mesh->mFaces[f].mNumIndices);
				localMesh.closeLineLoops(loopSizes);
			}

		}
		else {
			localMesh.mergeUniqueVerts();
			localMesh.linesToLineStrip();
		}
		localMesh.addAdjacencyVerts();
	}



	//add to singleMesh
	TMeshRec localMeshRec = singleMesh.add(localMesh);
	localMeshRec.isLine = isLine;
	localMeshRec.isLineLoop = isLineLoop;
	return { localMeshRec, localExtents };
}

void CImporter::copyMatrix(aiMatrix4x4& src, glm::mat4& dest) {
	dest[0].x = (float)src.a1;
	dest[0].y = (float)src.a2;
	dest[0].z = (float)src.a3;
	dest[0].w = (float)src.a4;

	dest[1].x = (float)src.b1;
	dest[1].y = (float)src.b2;
	dest[1].z = (float)src.b3;
	dest[1].w = (float)src.b4;

	dest[2].x = (float)src.b1;
	dest[2].y = (float)src.b2;
	dest[2].z = (float)src.b3;
	dest[2].w = (float)src.b4;

	dest[3].x = (float)src.d1;
	dest[3].y = (float)src.d2;
	dest[3].z = (float)src.d3;
	dest[3].w = (float)src.d4;
}


/** Modify node's extents if the subnode out-extends them. */
void CImporter::updateNodeExtents(TModelData& node, TModelData& subNode) {
	node.extents.furthestVert = glm::max(node.extents.furthestVert, subNode.extents.furthestVert);
	node.extents.BBmin = glm::min(node.extents.BBmin, subNode.extents.BBmin);
	node.extents.BBmax = glm::max(node.extents.BBmax, subNode.extents.BBmax);
}




