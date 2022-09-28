#pragma once
#include "renderer/mesh.h"

/** General class defining a model consisting of one or more meshes. */
class CModel {
public:
	void addMesh(TModelMesh& mesh);
	void setMainMesh(const std::string& name);
	TModelMesh* getMainMesh();
	TModelMesh* getMesh(const std::string& name);
	bool circleCollision(glm::vec3& segA, glm::vec3& segB);
	bool BBcollision(glm::vec3& segA, glm::vec3& segB);
	float getRadius();

	CBuf2 buf;
	std::vector<glm::vec4> palette{ { 1.0f,0.0f,0.0f,1.0f } };

	std::vector<TModelMesh> meshes;

	//glm::mat4 tmpMatrix;
	TMeshExtents extents;

private:
	int mainMeshIdx = -1;

};