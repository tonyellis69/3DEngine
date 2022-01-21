#pragma once

#include <glm/glm.hpp>

enum TWinEventType { eMouseMove };

class CWinEvent {
public:
	CWinEvent(TWinEventType type, int x, int y) {
		this->type = type;
		screenPos = { x,y };
	}



	TWinEventType type;
	glm::i32vec2 screenPos;


};