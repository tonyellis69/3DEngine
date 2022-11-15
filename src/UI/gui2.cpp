#include "gui2.h"

#include "listen/listen.h"

#include "guiEvent.h"

#include "win/win.h"

CguiRoot2 UI; ///<Global object.

CguiBase* findControlUnderMouse(CguiBase* ctrl, glm::i32vec2& mousePos);

void CguiRoot2::init(CguiBase* root) {
	pRoot = root;
	hotControl = root;
}

void CguiRoot2::addToIndex(CguiBase* ctrl) {
	ctrlIndex.push_back(ctrl);
}

void CguiRoot2::onMouseButton(int button, int action, int mod) {
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		if (action == GLFW_RELEASE && lMouseDown)
			hotControl->onLeftClick();
		lMouseDown = action;
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
		if (action == GLFW_RELEASE && rMouseDown)
			hotControl->onRightClick();
		rMouseDown = action;
	}
	else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
		if (action == GLFW_RELEASE && mMouseDown)
			hotControl->onMiddleClick();
		mMouseDown = action;
	}
}

void CguiRoot2::onKey(int key, int action, int mod) {
	CGUIevent e;
	e.i1 = key; e.i2 = mod;
	if (action == GLFW_PRESS) {
		e.type = eKeyDown;
	}
	else if (action == GLFW_RELEASE) {
		e.type = eKeyUp;
	}
	else if (action == GLFW_REPEAT) { //NB: OK for text-entry, not game-smooth realtime.
		e.type = eKeyRepeat;
	}
	lis::event(e);
}


void CguiRoot2::onMouseMove(double x, double y) {
	glm::i32vec2 pos = { x,y };
	oldHotControl = hotControl;
	hotControl = findControlUnderMouse(pRoot, pos);
	if (hotControl != oldHotControl && oldHotControl) {
		oldHotControl->onMouseOff();
	}

	if (hotControl == nullptr)
		return;
	hotControl->onMouseMove(pos);
}

void CguiRoot2::onWinEnter(int entered) {	
	if (entered == false && oldHotControl) {
		oldHotControl->onMouseOff();
	}

}

//CguiBase* CguiRoot2::getControl(int id) {
//	CguiBase* result = nullptr;
//	//if (uniqueID == id)
//	//	return this;
//	//for (auto& control : controls) {
//	//	result = ((CguiBase*)control)->findDescendant(id);
//	//	if (result != NULL)
//	//		break;
//	//}
//	return result;
//}

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
	glm::i32vec2 p;
	CWin::getMousePos(p.x, p.y);
	return ctrl->overlays(p);
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
