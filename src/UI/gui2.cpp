#include "gui2.h"

#include "listen/listen.h"

#include "utils/log.h"

CguiRoot2 gui;

CguiBase* findControlUnderMouse(CguiBase* ctrl, glm::i32vec2& mousePos);

void CguiRoot2::init(CguiBase* root) {
	pRoot = root;
	hotControl = root;
	lis::subscribe(this);
}

void CguiRoot2::addToIndex(CguiBase* ctrl) {
	ctrlIndex.push_back(ctrl);
}

void CguiRoot2::onEvent(CEvent& e) {
	if (e.type == eMouseMove) {
		oldFocusControl = hotControl;
		hotControl = findControlUnderMouse(pRoot, e.pos);
		if (hotControl != oldFocusControl && oldFocusControl) {
			CguiMsg msg;
			msg.type = gMouseOff;
			oldFocusControl->onGuiMsg(msg);
		}


		if (hotControl == nullptr)
			return;
		CguiMsg msg;
		msg.type = gMouseMove;
		msg.mousePos = e.pos;
		hotControl->onGuiMsg(msg);
	}
	else if (e.type == eMouseExitWindow) {
		if (oldFocusControl) {
			CguiMsg msg;
			msg.type = gMouseOff;
			oldFocusControl->onGuiMsg(msg);
		}

	}

}

CguiBase* CguiRoot2::getControl(int id) {
	CguiBase* result = nullptr;
	if (uniqueID == id)
		return this;
	for (auto& control : controls) {
		result = ((CguiBase*)control)->findDescendant(id);
		if (result != NULL)
			break;
	}
	return result;
}

CguiBase* CguiRoot2::getParent(int id) {
	CguiBase* ctrl = pRoot->findDescendant(id);
	return (CguiBase*)ctrl->parent;
}

CguiBase* CguiRoot2::findControl(const std::string& targetName) {
	for (auto& ctrl : ctrlIndex)
		if (ctrl->name == targetName)
			return ctrl;
	return nullptr;
}

bool CguiRoot2::mouseIn(const std::string& name) {
	CguiBase* ctrl = findControl(name);
	return ctrl->isMouseOver();
}

bool CguiRoot2::mouseNotIn(const std::string& name) {
	return !mouseIn(name);
}



/** Return the visible control under the mouse pointer that has
	no children also under the mouse pointer. */
CguiBase* findControlUnderMouse(CguiBase* ctrl, glm::i32vec2& mousePos) {
	if (ctrl->overlays(mousePos) == false  || ctrl->visible == false)
		return nullptr;

	CguiBase* result;
	for (auto* child : ctrl->controls) {
		result = findControlUnderMouse((CguiBase*)child, mousePos);
		if (result)
			return result;
	}

	return ctrl;
}
