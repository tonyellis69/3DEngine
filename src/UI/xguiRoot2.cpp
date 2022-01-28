#include "guiRoot2.h"

#include "listen/listen.h"

#include "utils/log.h"

CguiBase* findControlUnderMouse(CguiBase* ctrl, glm::i32vec2& mousePos);

void CguiRoot2::init(CguiBase* root) {
	pRoot = root;
	focusControl = root;
	lis::subscribe(this);
}

void CguiRoot2::onEvent(CEvent& e) {
	if (e.type == eMouseMove) {
		oldFocusControl = focusControl;
		focusControl = findControlUnderMouse(pRoot, e.pos);
		if (focusControl != oldFocusControl && oldFocusControl) {
			CguiMsg msg;
			msg.type = gMouseOff;
			oldFocusControl->onGuiMsg(msg);
		}


		if (focusControl == nullptr)
			return;
		CguiMsg msg;
		msg.type = gMouseMove;
		msg.mousePos = e.pos;
		focusControl->onGuiMsg(msg);
	}

}



CguiBase* findControlUnderMouse(CguiBase* ctrl, glm::i32vec2& mousePos) {
	if (ctrl->posInside(mousePos) == false)
		return nullptr;

	CguiBase* result;
	for (auto* child : ctrl->controls) {
		result = findControlUnderMouse((CguiBase*)child, mousePos);
		if (result)
			return result;
	}

	return ctrl;
}
