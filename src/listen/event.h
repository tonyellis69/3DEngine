#pragma once

#include <string>
#include <variant>

#include <glm/glm.hpp>

enum TEventType { eNone, eMouseMove, eLeftClick, eMouseOff, eHotTextHover, 
	eMouseEnterWindow, eMouseExitWindow, eKeyDown, eKeyUp, eKeyRepeat, 
	eGameEvent};

enum TMouseButton {};

struct TMouseButtonEvent {


};

class CEvent {
public:
	CEvent() {}
	//~CEvent() {}
	TEventType type;
	union {
		
		unsigned int guiID;
		int i1;
	};
	int i2;
	glm::i32vec2 pos;
	//std::variant<glm::i32vec2, unsigned int, std::string> data;
	std::string* hotTxt;
};