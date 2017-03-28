#pragma once

#include "3dobject.h"
#include <glew.h>//for GL_TRIANGLES

/** Class for 3d polygon models. 
	NB: because vertices are ultimately stored with the graphics hardware,
	ie, 'server side', we don't actually store them with the model.*/
class CModel : public C3dObject {
public:
	CModel();
	CModel(glm::vec3& pos);
	void init();

	//unsigned int hBuffer; ///<Handle referencing object's buffered vertex data.
	//int colourOffset; ///<Start of colour values in buffer.
	//int noVerts;///<Total vertices in this model.
	//int nAttribs; ///<Attributes per vertex.
	//unsigned int hVAO; ///<References vertex array object for rendering this model.
//	unsigned int hIndex; ///<Handle referencing index object for rendering this model.
	//int indexSize; ///<Number of indices.
	//int normOffset;

	int drawMode; ///<Triangles, lines etc

//	int nTris; ///<Number of triangles

	unsigned int hVertexObj; ///<The vertex object internally assigned to this model.

private:

};