#include "GUIbase.h"
#include <string>
#include <sstream>

#include "GUIradio.h"
#include "GUIscrollbar.h"
#include "GUIgroup.h"
#include "GUImouse.h"

// TO DO: get rid of 'Name' and 'Count'. It's messy and I don't use it. 

CGUIbase::CGUIbase()  {
	backColour1 = UItransparent;
	backColour2 = UItransparent;
	foreColour1 = UIlightGrey;
	foreColour2 = UIlightGrey;
	parent = this; 
	Name = new char[6];
	strcpy(Name,"Blank");
	hFormat = hNone;
	vFormat = vNone;
	MouseOver = NULL;
	MouseDown = NULL;
	KeyCapture = NULL;
	scrollbarHasMouse = NULL;
	id = 0;
	type = base;
	visible = true;
	borderWidth = 0;
	needsUpdate = false;

	anchorBottom = NONE;	
	anchorRight = NONE;	


	xPos = 0; yPos = 0; width = 30; height = 20;

	enabled = true;

	mousePassthru = false;
}

/** Delete all children recursively. Hopefully without any complicated memory errors.*/
CGUIbase::~CGUIbase(void) {
	delete Name;
	for (size_t  i=0;i < Control.size();i++)
		delete Control[i];
}


CMessage CGUIbase::Message;
CGUIbase* CGUIbase::MouseOver;
CGUIbase* CGUIbase::MouseDown;
CGUIbase* CGUIbase::KeyCapture;
CGUIbase*  CGUIbase::scrollbarHasMouse;
CDrawFuncs* CGUIbase::pDrawFuncs;
CGUImouse* CGUIbase::mouse;
CGUIbaseEngine* CGUIbase::pGUIeng;

CFont* CGUIbase::defaultFont;

/** Returns true if the given point is inside this control's area. */
bool CGUIbase::IsOnControl(const CGUIbase& Control, const  int mouseX, const  int mouseY) {
	if (!Control.visible || !Control.enabled || Control.mousePassthru)
		return false;
	return ( (Control.xPos < mouseX) && ((Control.xPos + Control.width) > mouseX)
		&& (Control.yPos < mouseY) && ((Control.yPos + Control.height) > mouseY));
}


/** Handle any of several mouse messages from the OS. This may well involve recursively passing the message to this control's children. */
void CGUIbase::MouseMsg(unsigned int Msg, int mouseX, int mouseY, int key) {
	if ((!visible) || (!enabled))
		return;

	//otherwise, recursively test message against this control's child controls
	for (size_t i = 0; i < Control.size(); i++) {
		if (IsOnControl(*Control[i], mouseX, mouseY)) {
			Control[i]->MouseMsg(Msg, mouseX - Control[i]->xPos, mouseY - Control[i]->yPos, key);
			return;
		}
	}
	//Still here? Message is for this control
	switch (Msg) {
	case WM_MOUSEMOVE : {
		if (MouseOver != this){ //mouse has just entered this control 
			MouseDown = NULL;	//so remove any mousedown effect the previous control was showing
		}
		MouseOver = this; 
		OnMouseMove(mouseX, mouseY,key); return; }
	case WM_LBUTTONDOWN : {
		uncaptureKeyboard();
		MouseDown = this; 
		OnLMouseDown(mouseX,mouseY,key);
		return; }
  	case WM_RBUTTONDOWN : {
		uncaptureKeyboard();
		MouseDown = this; 
		OnRMouseDown(mouseX,mouseY);
		return; }
	case WM_LBUTTONUP : {
		OnLMouseUp(mouseX,mouseY);
		if (MouseDown == this) //we've been clicked on!
			OnClick(mouseX, mouseY);
		if (MouseOver != this)	
			MouseOver = this;
		scrollbarHasMouse = NULL;
		pDrawFuncs->mouseCaptured(false);
		MouseDown = NULL; 
		return;}
	case WM_RBUTTONUP : {
		onRMouseUp(mouseX,mouseY);
		return;}
	case LM_HELD_DOWN : {
		if (  (MouseDown == MouseOver) && (scrollbarHasMouse == NULL)) {
			OnLMouseDown(mouseX ,mouseY, key );
		return;
	}
						  }
	};
}



/** Respond to the user rolling the mouse wheel by recursively passing the message on to
	each control under the given mouseposition. Returns true if a control was found that
	handled the mousewheel event. */
bool CGUIbase::MouseWheelMsg(const  int mouseX, const  int mouseY, int wheelDelta, int key) {
	if ((!visible) || (!enabled))
		return false;

	for (size_t i=0;i < Control.size();i++)
		if ( IsOnControl( *Control[i],mouseX, mouseY) ) {
			if (Control[i]->MouseWheelMsg(mouseX - Control[i]->xPos, mouseY - Control[i]->yPos,wheelDelta,key))
				return true;
		}
	return false;
}


/** Set the dimensions and relative position of the control. */
void CGUIbase::SetPos(int x, int y, int w, int h) {
	xPos = x; yPos = y; width = w; height = h;
	updateAppearance();
}


/** Add a child UI element to this UI element. */
void CGUIbase::Add(CGUIbase* child) {
	Control.push_back(child);
	child->parent = this;
	child->updateAppearance();
}

/** Recursively update this control's position, dimensions and clipping, and those of its children. */
void CGUIbase::updateAppearance() {

	needsUpdate = false;
	//1. Recalculate x,y,w,h if necessary due to justification or spanning
	if (anchorRight != NONE)
		width = parent->width - xPos - anchorRight;
	else {
		switch (hFormat) {
		case hCentre : {
			xPos = (parent->width - width)/2; break;}
		case hRight : {
			xPos = parent->width - (width + parent->borderWidth); break; }
		case hLeft : {
			xPos = parent->borderWidth; break;}
		case hSpan : {
			xPos = 0; width = parent->width; break; }
		}
	}

	if (anchorBottom != NONE) 
		height = parent->height - yPos - anchorBottom;
	else {
		switch (vFormat) {
		case vCentre : {
			yPos = (parent->height - height)/2; break;}
		case vTop : {
			yPos = parent->borderWidth; break; }
		case vBottom : {
			yPos = parent->height - height - parent->borderWidth ; break;}
		case vSpan : {
			yPos = 0; height = parent->height; break; }
		}
	}

	

	//2. Recalculate clipping
	CalculateClipbox();
	//3. Recalculate screen position
	screenPos.x = parent->screenPos.x + xPos; screenPos.y = parent->screenPos.y + yPos;


	//repeat through children
	for (size_t i=0;i < Control.size();i++)
		Control[i]->updateAppearance();
}

/** Draw this UI element. In practice this means recursively drawing all its child items too.*/
void CGUIbase::Draw() {
	if (!visible)
		return;
	if (needsUpdate)
		updateAppearance();
	
	UIrect* parentClipbox = &parent->Clipbox;
	pDrawFuncs->setClip(Clipbox);

	DrawSelf(); 
	//then draw each subcontrol
	for (size_t i=0;i<Control.size();i++) 
		Control[i]->Draw();
}

/** Fire off the necessary drawing instructions to draw this control. */
void CGUIbase::DrawSelf() {
}


/** Calculate the drawable area of this control, and clip it to the drawable area of its parent conntrol. */
void CGUIbase::CalculateClipbox() {


	UIrect parentClipbox = parent->Clipbox;
	parentClipbox.x += parent->borderWidth;	parentClipbox.y +=parent-> borderWidth;
	parentClipbox.width -= parent->borderWidth*2; parentClipbox.height -= parent-> borderWidth*2;

	Clipbox.width = width;
	Clipbox.x = parent->screenPos.x + xPos;
	if (Clipbox.x <parentClipbox.x) {
		Clipbox.width -= (parentClipbox.x -Clipbox.x); 
		Clipbox.x = parentClipbox.x;
	}
	

	Clipbox.height = height;
	Clipbox.y = parent->screenPos.y + yPos;
	if (Clipbox.y < parentClipbox.y) {
		Clipbox.height -= (parentClipbox.y -Clipbox.y); 
		if (Clipbox.height < 0) 
			Clipbox.height=0;
		Clipbox.y = parentClipbox.y;
	}

	if ((Clipbox.x + Clipbox.width) > (parentClipbox.x + parentClipbox.width) ) 
		Clipbox.width -= (Clipbox.x + Clipbox.width) - (parentClipbox.x + parentClipbox.width);

	if ((Clipbox.y + Clipbox.height) > (parentClipbox.y + parentClipbox.height))
		Clipbox.height -= (Clipbox.y + Clipbox.height) - (parentClipbox.y + parentClipbox.height);
}

/** Generate a name for this control. */
void CGUIbase::GenName(char* NameStr, int iCount) {
	std::string strTmp(NameStr);
	std::ostringstream convert;
	convert << iCount;
	strTmp.append(convert.str());
	SetName((char*)strTmp.c_str());
}

void CGUIbase::SetName(char* NameStr) {
	int l = strlen(NameStr);
	delete Name;
	Name =  new char[l+1];
	strcpy(Name,NameStr);
}

void CGUIbase::setDefaultFont(CFont* font) {
	defaultFont = font;
}



/** Return a pointer to the child control of this control with the given id number. */
CGUIbase* CGUIbase::getChild(int childID) {
	for (size_t i=0;i<Control.size();i++){
		if ( Control[i]->id == childID )
			return Control[i];
	}
	return NULL;
}

/** Recursively search this control and its children for the control with the given id number. */
CGUIbase* CGUIbase::findControl(int childID) {
	CGUIbase* result = NULL;
	if (id == childID )
		return this;
	for (size_t i=0;i<Control.size();i++){
		result = Control[i]->findControl(childID);
		if (result != NULL) 
			break;
	}
	return result;	
}

/** Recursively search this control and its children for the given control. */
CGUIbase* CGUIbase::findControl(CGUIbase* child) {
	CGUIbase* result = NULL;
	if (this == child )
		return this;
	for (size_t i=0;i<Control.size();i++){
		result = Control[i]->findControl(child);
		if (result != NULL) 
			break;
	}
	return result;	
}


void CGUIbase::OnLMouseUp(const int mouseX, const int mouseY) {
	//do the default action for mouse up:
/*	CMessage msg;
	if (mouse->dragging) { //we're dragging something, so drop it.
	
		//Some code here to handle being dropped on

		msg.Msg = uiMsgDrop;
		msg.x = mouse->screenPos.x; msg.y = mouse->screenPos.y;
	//	Message.value = image;
		//then kill the mouse object
		pDrawFuncs->handleUImsg(*this,msg);
		//delete mouse;
	//mouse = new CGUImouse();
	
	}*/
	//TO DO: pretty sure we're not using the above any more.
	//also it's bad policy - dragging/dropping should be handled at a higher level,
	//not by the base class. By default, all we should do is alert user this event
	//has happened:
	CMessage msg2;
	msg2.Msg = uiMsgLMouseUp;
	msg2.x = mouse->screenPos.x; msg2.y = mouse->screenPos.y;
	pDrawFuncs->handleUImsg(*this,msg2);
} 

void CGUIbase::onRMouseUp(const int mouseX, const int mouseY) {
	CMessage msg;
	msg.Msg = uiMsgRMouseUp;
	msg.x = mouseX; msg.y = mouseY;
	pDrawFuncs->handleUImsg(*this,msg);
}

/** Returns the position of this control in the child-list of its parent. */
int CGUIbase::getControlNo() {
	int x=0;
	while (parent->Control[x] != this) {
		x++;
	}
	return x;
}

	
/** Handle a UI-internal message from another control.*/

void CGUIbase::message(CGUIbase& sender, CMessage& msg) {
	if (sender.type == radioButton) { //several controls can be radio parents, so catch it here
		//set the other radio buttons to 'off'.
		for (size_t i=0;i<Control.size();i++)
			if ( Control[i]->type == radioButton ) {
				if ( Control[i] != &sender) {
					static_cast<CGUIradio*>(Control[i])->Set = false; 
				}
			} 
	}
}

/** Disable or enable this contol. */
void CGUIbase::setEnabled(bool onOff) {
	enabled = onOff;
}

bool CGUIbase::getEnabled()
{
	return enabled;
}

/**Capture the keyboard for this control, forcing any other control holding
	it to lose it.*/
void CGUIbase::captureKeyboard() {
	uncaptureKeyboard();
	KeyCapture = this;
}

/** Release the keyboard from any control that may have captured ot. */
void CGUIbase::uncaptureKeyboard() {
	CMessage msg;
	msg.Msg  = uiLostKeyboard;
	if (KeyCapture != NULL)
		KeyCapture->message(*this,msg);
	KeyCapture = NULL;
}

void CGUIbase::setVisible(bool onOff) {
	visible = onOff;
}

bool CGUIbase::getVisible() {
	return visible;
}

unsigned int CGUIbase::getID() {
	return uniqueID;
}

