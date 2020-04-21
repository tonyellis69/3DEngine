#pragma once

#include <string>

#include "glm/glm.hpp"

#include "../renderer/buf.h"

struct THexDraw;
class CLineModel;

/** An interface class for using CHexRenderer. */
class IHexRenderer {
public:
	virtual void drawLines(THexDraw& drawData) = 0;
	virtual void drawLineModel(THexDraw& drawData) = 0;
	virtual CBuf* getBuffer(const std::string& bufName) = 0;
	virtual CLineModel getLineModel(const std::string& name) = 0;


};

struct THexDraw {
	glm::mat4* worldMatrix;
	glm::vec4* colour;
	CBuf* buf;
	CLineModel* lineModel;
};