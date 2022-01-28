#pragma once

#include <string>
#include <variant>

#include <glm/glm.hpp>

enum TEventType { eNone, eMouseMove, eMouseOff, eHotTextHover, eMouseEnterWindow, eMouseExitWindow};

class CEvent {
public:
	CEvent() {}
	//~CEvent() {}
	TEventType type;
	union {
		glm::i32vec2 pos;
		unsigned int guiID;
		int i1;
	};
	//std::variant<glm::i32vec2, unsigned int, std::string> data;
	std::string* hotTxt;
};