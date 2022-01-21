#include "guiBase2.h"

#include "listen/listen.h"


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
	e.id = uniqueID;
	lis::event(e);
}


/** Returns true if pos is inside screen position.*/
bool CguiBase::posInside(glm::i32vec2& pos) {
	if (!this->visible)
		return false;

	return glm::all(glm::lessThan(this->drawBox.pos, pos)) &&
		glm::all(glm::greaterThan(this->drawBox.pos + this->drawBox.size, pos));
}
