#pragma once

//#include "renderer\3dobject.h"
#include "renderer/model.h"
#include "vertBufs.h"
#include <vector>
#include "dynBuf.h"



/** Class for creating meshes out of 3D isosurfaces.*/
class CIsoSurfer : public CModel {
public:
	CIsoSurfer();
	~CIsoSurfer();
	void setSample(float* _data);
	void createGeometry(int width, int height, int depth);
	bool marchingCube(float& x, float& y, float& z);
	float fGetOffset(float fValue1, float fValue2, float fValueDesired);
	float fSample(float x, float y, float z);
	float fSample2(float x, float y, float z);
	void vGetNormal(glm::vec3 &rfNormal, float fX, float fY, float fZ);
	
	


	float* data;//[101][101][60]; ///<Pointer to the raw volume data.

	float fTargetValue; 

	float volWidth; ///<Width of volume in cubes.
	float volHeight; ///<Height of volume in cubes.
	float volDepth; ///<Depth of volume in cubes.

	float cubeSize; ///<Dimension of cubes.


//	CDynBuf<vBuf::T3Dvert> verts; ///<Container for all the vertices.

	vBuf::T3Dvert* pVertexBuffer; ///<Points to next free space in a user-supplied buffer.

	float afCubeValue[8];
	glm::vec3 asEdgeVertex[12];
	glm::vec3 asEdgeNorm[12];

	glm::vec4 colour; ///Colour volume will be shaded in.


	int noVerts; ///<Originally in CModel but I took it out
};

// For any edge, if one vertex is inside of the surface and the other is outside of the surface
//  then the edge intersects the surface
// For each of the 8 vertices of the cube can be two possible states : either inside or outside of the surface
// For any cube the are 2^8=256 possible sets of vertex states
// This table lists the edges intersected by the surface for all 256 possible vertex states
// There are 12 edges.  For each entry in the table, if edge #n is intersected, then bit #n is set to 1

