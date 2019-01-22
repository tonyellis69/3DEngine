#pragma once

#include "3dobject.h"
#include "mesh.h"
#include "buf.h"

class CModel2;
typedef void(*callbackFunctionPtr)(void*, CModel2*);

/** A class encapsulating a 3D drawable object. */
class CModel2 : public C3dObject {
public:
	CModel2();
	void setDrawCallout(callbackFunctionPtr cb, void* callee);
	void loadMesh(CMesh& newMesh);
	void bufferMesh();
	void draw();
	void scale(glm::vec3& scaleVec);
	void orientateWorldMatrix();

	CMesh mesh;
	CBuf buffer;
	glm::vec4 colour;

	glm::vec3 scaleVec;

private:
	void* drawCallee; //<The object whose draw callback we will use.
	callbackFunctionPtr drawCallback; //<Pointer to our draw callback function.
};