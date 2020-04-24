#pragma once

#include <string>

#include "glm/glm.hpp"

#include "../renderer/buf.h"

struct THexDraw;
class CLineModel;

/** An interface class for using CHexRenderer. */
class IHexRenderer {
public:
	virtual void drawLineModel(CLineModel& lineModel) = 0;
	//virtual CBuf* getBuffer(const std::string& bufName) = 0;
	virtual CLineModel getLineModel(const std::string& name) = 0;


};

