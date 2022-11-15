#pragma once

#include <string>
#include <variant>

#include <glm/glm.hpp>

enum TGUIEventType { eNone, eMouseMove, eLeftClick, eMouseOff, eHotTextHover, 
	eMouseEnterWindow, eMouseExitWindow, eKeyDown, eKeyUp, eKeyRepeat};

enum TMouseButton {};

struct TMouseButtonEvent {


};

class CGUIevent {
public:
	CGUIevent() {}
	//~CEvent() {}
	TGUIEventType type;
	union {
		
		unsigned int guiID;
		int i1;
	};
	int i2;
	glm::i32vec2 pos;
	//std::variant<glm::i32vec2, unsigned int, std::string> data;
	std::string* hotTxt;
};