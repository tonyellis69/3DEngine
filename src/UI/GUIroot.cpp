#include "GUIroot.h"
#include "GUImouse.h"

CGUIroot* CGUIbase::rootUI;
 
CGUIroot::CGUIroot() {
	SetName("Root");
	type = root;
	rootUI = this;
	mouse = new CGUImouse(); //create default mouse.
	id = uiRootID;
	modalControl = NULL;
	killModal = false;
}

/** We overload this method so that we can send messages direct to any control that has trapped the
	mouse, keyboard, etc. */
void CGUIroot::MouseMsg(unsigned int Msg, int mouseX, int mouseY, int key) {
	if ((!visible) || (!enabled))
		return;

	if (modalControl != NULL) {
		modalControl->MouseMsg(Msg,mouseX -modalControl->screenPos.x ,mouseY -modalControl->screenPos.y,key);
		if (killModal) 
			deleteModal();
		return;
	}
	if (( Msg == WM_MOUSEMOVE )) { 
		if (scrollbarHasMouse) { //a scrollbar has captured the mouse, so pass the message straight there.
				scrollbarHasMouse->OnMouseMove(mouseX - scrollbarHasMouse->screenPos.x,mouseY - scrollbarHasMouse->screenPos.y, key);
				return;
		}
		OnMouseMove(mouseX,mouseY,key);
	}


	CGUIbase::MouseMsg(Msg,mouseX,mouseY,key);
}

/** We overload this method so that we can send messages direct to any control that has trapped the
	mouse, keyboard, etc. */
bool CGUIroot::MouseWheelMsg(const  int mouseX, const  int mouseY, int wheelDelta, int key) {
	if ((!visible) || (!enabled))
		return false;

	if (modalControl != NULL) {
		return modalControl->MouseWheelMsg(mouseX -modalControl->screenPos.x ,mouseY -modalControl->screenPos.y,wheelDelta,key);
	}

	if (CGUIbase::MouseWheelMsg(mouseX,mouseY,wheelDelta,key))
		return true;

	//still here? mousewheel was on this control
	CMessage msg;
	msg.Msg = uiMouseWheel;
	msg.value = wheelDelta;
	pDrawFuncs->handleUImsg(*this,msg);

	return false;
}


void CGUIroot::SetPos(int x, int y, int w, int h) {
	xPos = x; yPos = y; width = w; height = h; screenPos.x = x; screenPos.y = y;
	Clipbox.x =x;Clipbox.y =y; Clipbox.width = w; Clipbox.height = h;
	for (size_t i=0;i < Control.size();i++)
		Control[i]->updateAppearance();
}


/** Override the normal clipping method, as GUIroot has no parent to clip against. */
void CGUIroot::CalculateClipbox() {
	Clipbox.x = xPos; Clipbox.y = yPos; Clipbox.width = width; Clipbox.height = height;
}

/** Receives a keystroke message, and passes it on to whichever control has captured the keyboard, if any. */
void CGUIroot::KeyboardMsg(unsigned int Key, long Mod) {
	if (KeyCapture != NULL)
		KeyCapture->OnKeyPress(Key,Mod);
}

/** Receives a character entry message, and passes it on to whichever control has captured the keyboard, if any. */
void CGUIroot::CharEntryMsg(unsigned int Key, long Mod) {
	if (KeyCapture != NULL)
		KeyCapture->OnCharEntry(Key,Mod);
}

/** Set the pointer for the GUI drawing functions to the given drawFunc bundle. */
void CGUIroot::setDrawFuncs(CDrawFuncs* drawFunc) {
	pDrawFuncs = drawFunc;
}

/** Overload function to draw the mouse after drawing everything else, so it's on top. */
void CGUIroot::Draw() {
	CGUIbase::Draw(); //draw every other child control of root first
	
	//draw mouse control if any
	pDrawFuncs->setClip(Clipbox);
	mouse->DrawSelf();
}


/** Ensures the mouse drawing coordinates are always set to where the mouse is. */
void CGUIroot::OnMouseMove(const int mouseX, const int mouseY, int key) {
	mouse->screenPos.x = mouseX; 
	mouse->screenPos.y = mouseY;
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
	modalControl = control;
	Add(control);
}

/** Schedule the modal control for deletion. We don't delete it now because it
	may still get accessed. */
void CGUIroot::removeModal() {
	killModal = true;
	updateAppearance();
}


//TO DO: this runs the risk of a method of the modal control being called
//after modalControl has been deleted.
/** Delete the current modal control, if any.*/
void CGUIroot::deleteModal() {
	if (modalControl != NULL) {
		for (size_t c=0;c<Control.size();c++) {
			if (Control[c] == modalControl)
				Control.erase(Control.begin()+c);
		}
		delete modalControl;
	}
	modalControl = NULL;
	killModal = false;
}


CGUIroot::~CGUIroot() {
	delete mouse;

}
