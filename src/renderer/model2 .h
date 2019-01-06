#pragma once

#include "mesh.h"
#include "shaderDraw.h"


/** A class encapsulating a 3D drawable object. */
class CModel2 {
public:


	CMesh mesh;
	CShaderDraw shader;
};