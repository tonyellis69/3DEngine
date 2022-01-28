#pragma once

#include "GUIbase.h"

#include <string>

#include "listen/event.h"
#include "guiMsg.h"

class CguiBase : public CGUIbase {
public:
	CguiBase() : CGUIbase() {}
	CguiBase(int x, int y, int w, int h) : CGUIbase(x, y, w, h) {}

	void add(CGUIbase* child);
	virtual void onEvent(CEvent& e) {}
	virtual void onGuiMsg(CguiMsg& msg);
	virtual void onMouseMove(glm::ivec2& mousePos);
	virtual void onMouseOff();

	bool overlays(glm::i32vec2& pos);

	CguiBase* findDescendant(int childId);

	std::string name; 
};