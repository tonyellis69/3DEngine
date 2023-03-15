#pragma once

#include <string>
#include <variant>

#include <glm/glm.hpp>

enum TGUIEventType { eNone, eMouseMove, eLeftClick, eMouseOff, eHotTextHover, 
	eMouseEnterWindow, eMouseExitWindow, eKeyDown, eKeyUp, eKeyRepeat, eMouseWheel,
	eLeftDown, eLeftUp, eRightDown, eRightUp, eMiddleDown, eMiddleUp };

enum TGUIkeyMod {eModNone,eModShift, eModCtrl, eModAlt, eModSuper};


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
		float f1;
		int key;
		float dx;
	};
	union {
		int i2;
		float f2;
		float dy;
	};
	glm::i32vec2 pos;
	//std::variant<glm::i32vec2, unsigned int, std::string> data;
	std::string* hotTxt;
	TGUIkeyMod keyMod = eModNone;
};