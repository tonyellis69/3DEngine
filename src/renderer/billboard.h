#pragma once

#include "renderModel.h"

/** A class for billboards. */
class CBillboard : public CRenderModel {
public:
	CBillboard(glm::vec3& pos, glm::vec2& size);
	void setSize(glm::vec2& size);
	void drawNew();

	glm::vec2 size;
};
