#pragma once

#include "3dobject.h"
#include "mesh.h"
//#include "buf.h"
#include "material2.h"
#include "renderTexture.h"

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
	void setTexture(CRenderTexture* texture);

	void calcBBextent();

	CMesh mesh; ///<Stores the verts locally.
	CBuf2 buffer; ///<Stores the verts in graphics memory.

	CMaterial2 material; ///TO DO: should this be a pointer to a material instead?
	glm::vec3 scaleVec;

	glm::vec3 BBextent;

private:
	void* drawCallee; //<The object whose draw callback we will use.
	callbackFunctionPtr drawCallback; //<Pointer to our draw callback function.
};