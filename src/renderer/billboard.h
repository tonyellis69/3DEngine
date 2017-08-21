#pragma once

#include "renderModel.h"

/** A class for billboards. */
//TO DO: billboards should be provided by either the engine or
//the renderer - probably engine as they are more high concept
//can then be tidily initialised with the renderer.
class CBillboard : public CRenderModel {
public:
	CBillboard(glm::vec3& pos, glm::vec2& size, CRenderer* renderer);
	void setSize(glm::vec2& size);
	void drawNew();

	glm::vec2 size;
};
