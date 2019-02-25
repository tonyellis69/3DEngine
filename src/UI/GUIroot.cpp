#include "GUIroot.h"
#include "GUImouse.h"

CGUIroot* CGUIbase::rootUI = NULL;
CDrawFuncs* CGUIdragDrop::pDrawFuncs = NULL;
 
CGUIroot::CGUIroot() {
	SetName("Root");
	type = root;
	rootUI = this;
	mouse = new CGUImouse(); //create default mouse.
	id = uiRootID;
	focusControl = NULL;
	borderWidth = 0;
}

/** Root overloads this method so it can send messages direct to any control that has trapped the
	mouse, keyboard, etc. */
void CGUIroot::MouseMsg(unsigned int Msg, int mouseX, int mouseY, int key) {
	if ((!visible) || (!enabled))
		return;

	
	if (scrollbarHasMouse) { //a scrollbar has captured the mouse, so pass the message straight there.
		if ((Msg == WM_MOUSEMOVE)) { //TO DO: pass all messages to the scrollbar instead?
			scrollbarHasMouse->OnMouseMove(mouseX , mouseY , key);
			return;
		}
	}

	if (modalControls.size() > 0 /*&& scrollbarHasMouse == NULL*/) {
		//modalControl->MouseMsg(Msg,mouseX -modalControl->screenPos.x ,mouseY -modalControl->screenPos.y,key);
		//TO DO: why did I ever do the above?
		modalControls.back()->MouseMsg(Msg, mouseX , mouseY , key);
		//modalControl->MouseMsg(Msg, mouseX , mouseY, key);
		return;
	}

	if ((Msg == WM_MOUSEMOVE)) {
		OnMouseMove(mouseX, mouseY, key);
	}


	if (focusControl)
		return focusControl->MouseMsg(Msg, mouseX, mouseY,key);


	CGUIbase::MouseMsg(Msg,mouseX,mouseY,key);
}

/** Root overloads this method so it can send messages direct to any control that has trapped the
	mouse, keyboard, etc. */
bool CGUIroot::MouseWheelMsg(const  int mouseX, const  int mouseY, int wheelDelta, int key) {
	if ((!visible) || (!enabled))
		return false;

	if (modalControls.size()) {
		return modalControls.back()->MouseWheelMsg(mouseX,mouseY,wheelDelta,key);
	}

	if (focusControl)
		return focusControl->MouseWheelMsg(mouseX, mouseY, wheelDelta, key);

	if (CGUIbase::MouseWheelMsg(mouseX,mouseY,wheelDelta,key))
		return true;

	

	//still here? mousewheel was on this control
	CMessage msg;
	msg.Msg = uiMouseWheel;
	msg.value = wheelDelta;
	pDrawFuncs->handleUImsg(*this,msg);

	return false;
}


void CGUIroot::setLocalDimensions(int x, int y, int w, int h) {
	setPos(x, y);
	setWidth(w);
	setHeight(h);
	drawBox.pos = glm::i32vec2(x, y);
	Clipbox.x =x;Clipbox.y =y; Clipbox.width = w; Clipbox.height = h;
	for (size_t i=0;i < controls.size();i++)
		controls[i]->updateAppearance();
}


/** Override the normal clipping method, as GUIroot has no parent to clip against. */
void CGUIroot::recalculateClipbox() {
	Clipbox.x = getLocalPos().x; Clipbox.y = getLocalPos().y; Clipbox.width = getWidth(); Clipbox.height = getHeight();
}

/** Receives a keystroke message, and passes it on to whichever control has captured the keyboard, if any. */
void CGUIroot::KeyboardMsg(unsigned int Key, long Mod) {
	if (KeyCapture != NULL)
		KeyCapture->onKeyPress(Key,Mod);
}

/** Receives a character entry message, and passes it on to whichever control has captured the keyboard, if any. */
void CGUIroot::CharEntryMsg(unsigned int Key, long Mod) {
	if (KeyCapture != NULL)
		KeyCapture->OnCharEntry(Key,Mod);
}

/** Set the pointer for the GUI drawing functions to the given drawFunc bundle. */
void CGUIroot::setDrawFuncs(CDrawFuncs* drawFunc) {
	pDrawFuncs = drawFunc;
	dragDropObj->pDrawFuncs = drawFunc;
}

/** Overload function to draw the mouse after drawing everything else, so it's on top. */
void CGUIroot::Draw() {
	//CGUIbase::Draw(); //draw every other child control of root first

	UIrect* parentClipbox = &parent->Clipbox;
	pDrawFuncs->setClip(Clipbox);

	//then draw each subcontrol
	for (size_t i = 0; i < controls.size(); i++) {
		if (!controls[i]->isModal)
			controls[i]->Draw();
	}

	//draw modal controls last
	for (auto modalCtrl : modalControls) {
		modalCtrl->Draw();
	}

	//draw mouse control if any
	pDrawFuncs->setClip(Clipbox);
	//mouse->DrawSelf();
	if (dragDropObj) {
		dragDropObj->drawSelf(mousePos.x, mousePos.y);
	}
}


/** Ensures the mouse drawing coordinates are always set to where the mouse is. */
void CGUIroot::OnMouseMove(const int mouseX, const int mouseY, int key) {
	//mouse->screenPos.x = mouseX; 
	//mouse->screenPos.y = mouseY;
	mousePos.x = mouseX;
	mousePos.y = mouseY;
	CMessage msg;
	msg.Msg = uiMsgMouseMove;
	msg.value = key;
	msg.x = mouseX;
	msg.y = mouseY;
	pDrawFuncs->handleUImsg(*this,msg);
}

/** Root response to a left-mouse down action - report it, in case it's of use to the user.*/
void CGUIroot::OnLMouseDown(const  int mouseX, const  int mouseY, int key) {
	CMessage msg;
	msg.Msg = uiMsgLMdown;
	msg.x = mouseX;
	msg.y = mouseY;
	msg.value = key;
	pDrawFuncs->handleUImsg(*this,msg);
}

void CGUIroot::onLMouseUp(const int mouseX, const int mouseY) {
	CMessage msg;
	msg.Msg = uiMsgLMouseUp;
	msg.x = mouseX;
	msg.y = mouseY;
	//msg.value = key;
	pDrawFuncs->handleUImsg(*this,msg);

	
}
/** Root response to a right-mouse down action - report it, in case it's of use to the user.*/
void CGUIroot::OnRMouseDown(const  int mouseX, const  int mouseY) {
	CMessage msg;
	msg.Msg = uiMsgRMdown;
	msg.x = mouseX;
	msg.y = mouseY;
	pDrawFuncs->handleUImsg(*this,msg);
}

/** Root response to being clicked. */
void CGUIroot::OnClick(const  int mouseX, const  int mouseY) {
	CMessage msg;
	msg.Msg = uiClick;
	msg.x = mouseX; msg.y = mouseY;
	pDrawFuncs->handleUImsg(*this,msg);
}

/* 05/01/15 This was root default behavour, making it base control behaviour. */
/* void CGUIroot::onRMouseUp(const int mouseX, const int mouseY) {
	CMessage msg;
	msg.Msg = uiMsgRMouseUp;
	msg.x = mouseX; msg.y = mouseY;
	pDrawFuncs->handleUImsg(*this,msg);
} */

/** Add this control as something that must be dismissed before the user can return to
	the rest of the UI. Normally used for modal dialogue boxes.*/
void CGUIroot::addModal(CGUIbase* control) {
	modalControls.push_back(control);
	control->isModal = true;
	Add(control);
}



/** Set the given control as the control with focus, to which input can be sent even if the mouse pointer
isn't over it. */
void CGUIroot::setFocus(CGUIbase * control) {
	focusControl = control;
}






CGUIroot::~CGUIroot() {


	for (unsigned int i = 0; i < controls.size(); i++)
		if (controls[i] != NULL)
			delete controls[i];
	
	
	//TO DO: ancient, get rid of
	delete mouse;

}
