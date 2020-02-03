#pragma once

#include "glm/glm.hpp"

#include "../renderer/buf.h"

struct THexDraw;

/** An interface class for using CHexRenderer. */
class IHexRenderer {
public:
	virtual void drawLines(THexDraw& drawData) = 0;



};

struct THexDraw {
	glm::mat4* worldMatrix;
	glm::vec4* colour;
	CBuf* buf;
};