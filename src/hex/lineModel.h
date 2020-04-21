#pragma once

#include "renderer/buf.h"

#include "renderer/mesh.h"

/** A class encapsulating the vertex buffer, sub meshes 
	and 3D operations required to draw a line model. */
class CLineModel {
public:

	TModelNode meshNodes;
	CBuf* buffer;
};