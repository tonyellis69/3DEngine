#include "importer.h"


/** Load a line-based model and write it to the given buffer. */
void CImporter::loadFile(const std::string& filename) {
	Assimp::Importer importer;
	meshes.clear();

	const aiScene* scene = importer.ReadFile(filename, aiProcess_JoinIdenticalVertices | aiProcess_Triangulate);
	
	processNode(scene->mRootNode, scene);

}

/** Return a vector of the currently loaded meshes. */
std::vector<CMesh>& CImporter::getMeshes() {
	return meshes;
}

/** Recursively retrieve all the verts etc from each node in the scene. */
void CImporter::processNode(aiNode* node, const aiScene* scene) {
	for (unsigned int m = 0; m < node->mNumMeshes; m++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[m]];
		meshes.push_back(processMesh(mesh, scene));
	}

	for (unsigned int n = 0; n < node->mNumChildren; n++)
		processNode(node->mChildren[n],scene);


}

/**	Return a mesh object holding all the data from this assimp mesh. */
CMesh CImporter::processMesh(aiMesh* mesh, const aiScene* scene) {
	CMesh ourMesh;

	//get verts
	for (unsigned int v = 0; v < mesh->mNumVertices; v++) {
		glm::vec3 vert = { mesh->mVertices[v].x, mesh->mVertices[v].y, mesh->mVertices[v].z };
		ourMesh.vertices.push_back(vert);

		//normals and texture coords go here
	}

	//get indices to verts
	for (unsigned int f = 0; f < mesh->mNumFaces; f++) {
		aiFace face = mesh->mFaces[f];
		for (unsigned int i = 0; i < face.mNumIndices; i++)
			ourMesh.indices.push_back(face.mIndices[i]);
	}

	return ourMesh;
}
