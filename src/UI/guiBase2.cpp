#include "guiBase2.h"

#include "gui2.h"

#include "listen/listen.h"


void CguiBase::add(CGUIbase* child) {
	CGUIbase::add(child);
	gui.addToIndex((CguiBase*)child);
}

void CguiBase::onGuiMsg(CguiMsg& msg) {
	switch (msg.type) {
	case gMouseOff: onMouseOff(); break;
	case gMouseMove: onMouseMove(msg.mousePos); break;

	}
}

void CguiBase::onMouseMove(glm::ivec2& mousePos) {

}

void CguiBase::onMouseOff() {
	CEvent e;
	e.type = eMouseOff;
	e.guiID = uniqueID;
	lis::event(e);
}


/** Returns true if pos is inside screen position.*/
bool CguiBase::overlays(glm::i32vec2& pos) {
	if (!this->visible)
		return false;

	return glm::all(glm::lessThan(this->drawBox.pos, pos)) &&
		glm::all(glm::greaterThan(this->drawBox.pos + this->drawBox.size, pos));
}

CguiBase* CguiBase::findDescendant(int childId) {
	CguiBase* result = nullptr;
	if (uniqueID == childId)
		return this;
	for (auto& control : controls) {
		result = ((CguiBase*)control)->findDescendant(childId);
		if (result != NULL)
			break;
	}
	return result;
}
