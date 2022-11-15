#pragma once

#include "GUIbase.h"

#include <string>

#include "guiEvent.h"
//#include "guiMsg.h"

class CguiBase : public CGUIbase {
public:
	CguiBase() : CGUIbase() {}
	CguiBase(int x, int y, int w, int h) : CGUIbase(x, y, w, h) {}

	void add(CGUIbase* child);
	virtual void onEvent(CGUIevent& e) {}
	virtual void onMouseMove(glm::ivec2& mousePos);
	virtual void onMouseOff();
	virtual void onLeftClick();
	virtual void onRightClick() {}
	virtual void onMiddleClick() {}

	bool overlays(glm::i32vec2& pos);

	CguiBase* findDescendant(int childId);

	std::string name; 
};