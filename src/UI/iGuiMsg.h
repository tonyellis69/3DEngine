#pragma once

#include "glm/glm.hpp"

struct TGUImessage {
public:
	int msg;
	int value;
	int value2;
	float fValue;
	glm::i32vec2 pos;
};


/** An abstract interface for receiving messages from
	GUI controls. */
class IGUImsg {
public:
	virtual void GUImsg(int ctrlId, TGUImessage& msg) = 0;

};