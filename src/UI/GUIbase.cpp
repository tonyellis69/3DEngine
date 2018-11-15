#include "GUIbase.h"
#include <string>
#include <sstream>

#include "GUIradio.h"
#include "GUIscrollbar.h"
#include "GUIgroup.h"
#include "GUImouse.h"

// TO DO: get rid of 'Name' and 'Count'. It's messy and I don't use it. 

unsigned int UIuniqueIDgen = 0;

CGUIbase::CGUIbase()  {
	backColour1 = UItransparent;
	backColour2 = UItransparent;
	foreColour1 = UIlightGrey;
	foreColour2 = UIlightGrey;
	parent = this; 
	Name = "Blank";
	hFormat = hNone;
	vFormat = vNone;
	MouseOver = NULL;
	MouseDown = NULL;
	KeyCapture = NULL;
	scrollbarHasMouse = NULL;
	id = 0;
	type = base;
	borderWidth = 0;
	needsUpdate = false;

	anchorBottom = NONE;	
	anchorRight = NONE;	


	localPos = glm::i32vec2(0); 
	width = 30; height = 20;

	enabled = true;

	mousePassthru = false;

	//BetterBase stuff, may overrule some of the above:
	backColour1 = UIlightGrey;
	backColour2 = UIdarkGrey;
	borderColour = UIdarkGrey;
	drawBorder = true;
	uniqueID = UIuniqueIDgen++;
	visible = true;
	callbackObj = NULL;
	isModal = false;
}

/** Delete all children recursively. Hopefully without any complicated memory errors.*/
CGUIbase::~CGUIbase(void) {
	if (pDrawFuncs)
		pDrawFuncs->deregisterControl(*this);
	if (MouseOver == this)
		MouseOver = NULL;
	for (auto control = modalControls.begin(); control != modalControls.end(); control++) {
		if (*control == this) {
			modalControls.erase(control);
			break;
		}
	}

	//remove from parent's controls if any
	auto it = parent->Control.begin();
	for (; it != parent->Control.end(); it++) {
		if (*it == this) {
			parent->Control.erase(it);
			break;
		}
	}

	for (unsigned int i = 0; i < Control.size(); i++)
		if (Control[i] != NULL)
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

std::vector<CGUIbase*> CGUIbase::modalControls;

/** Returns true if the given point is inside this control's area. */
bool CGUIbase::IsOnControl(const CGUIbase& Control, const  int mouseX, const  int mouseY) {
	if (!Control.visible || !Control.enabled || Control.mousePassthru)
		return false;
	//return ( (Control.localPos.x < mouseX) && ((Control.localPos.x + Control.width) > mouseX)
	//	&& (Control.localPos.y < mouseY) && ((Control.localPos.y + Control.height) > mouseY));
	return glm::all(glm::lessThan(Control.drawBox.pos, glm::i32vec2(mouseX, mouseY))) &&
		glm::all(glm::greaterThan(Control.drawBox.pos + Control.drawBox.size,
			glm::i32vec2(mouseX, mouseY)));

}


/** Handle any of several mouse messages from the OS. This may well involve recursively passing the message to this control's children. */
void CGUIbase::MouseMsg(unsigned int Msg, int mouseX, int mouseY, int key) {
	if ((!visible) || (!enabled))
		return;
	
	//otherwise, recursively test message against this control's child controls
	for (size_t i = 0; i < Control.size(); i++) {
		if (IsOnControl(*Control[i], mouseX, mouseY)) {		
			Control[i]->MouseMsg(Msg, mouseX, mouseY, key);
			return;
		}
	}
	//Still here? Message is for this control
	switch (Msg) {
	case WM_MOUSEMOVE: {
		if (MouseOver != this) { //mouse has just entered this control 
			MouseDown = NULL;	//so remove any mousedown effect the previous control was showing
			if (MouseOver)
				MouseOver->onMouseOff(mouseX, mouseY, key);
		}
		MouseOver = this;
		OnMouseMove(mouseX, mouseY, key); return; }
	case WM_LBUTTONDOWN: {
		uncaptureKeyboard();
		MouseDown = this;
		OnLMouseDown(mouseX, mouseY, key);
		return; }
	case WM_RBUTTONDOWN: {
		uncaptureKeyboard();
		MouseDown = this;
		OnRMouseDown(mouseX, mouseY);
		return; }
	case WM_LBUTTONUP: {
		OnLMouseUp(mouseX, mouseY);
		if (MouseDown == this) //we've been clicked on!
			OnClick(mouseX, mouseY);
		if (MouseOver != this)
			MouseOver = this;
		scrollbarHasMouse = NULL;
		pDrawFuncs->mouseCaptured(false);
		MouseDown = NULL;
		return; }
	case WM_RBUTTONUP: {
		onRMouseUp(mouseX, mouseY);
		return; }
	case LM_HELD_DOWN: {
		if ((MouseDown == MouseOver) && (scrollbarHasMouse == NULL)) {
			OnLMouseDown(mouseX, mouseY, key);
			return;
		}
	case MY_DOUBLECLICK: {
		onDoubleClick(mouseX, mouseY);
		return; }
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
			if (Control[i]->MouseWheelMsg(mouseX , mouseY ,wheelDelta,key))
				return true;
		}
	return false;
}


/** Set the dimensions and relative position of the control. */
/*
void CGUIbase::SetPos(int x, int y, int w, int h) {
	xPos = x; yPos = y; width = w; height = h;
	updateAppearance();
}
*/
void CGUIbase::SetPos(int x, int y, int w, int h) {
	localPos = glm::i32vec2(x, y);
	width = w; height = h;
	//drawBox.pos = glm::i32vec2(x, y);
	drawBox.size = glm::i32vec2(w, h);
	updateAppearance();
}

void CGUIbase::setPos(int x, int y) {
	localPos = glm::i32vec2(x, y);
	//drawBox.pos = glm::i32vec2(x, y);
	//updateAppearance();
	needsUpdate = true;

}





/** Add a child UI element to this UI element. */
void CGUIbase::Add(CGUIbase* child) {
	Control.push_back(child);
	child->parent = this;
	child->updateAppearance();
}

/** Recursively update this control's position, dimensions and clipping, and those of its children. */
/*
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
*/

/** Recursively update this control's position, dimensions and clipping, and those of its children. */
void CGUIbase::updateAppearance() {
	needsUpdate = false;
	//1. Recalculate x,y,w,h if necessary due to justification or spanning
	if (anchorRight != NONE)
		//drawBox.size.x = parent->drawBox.size.x - localPos.x - anchorRight;
		localPos.x = parent->drawBox.size.x - drawBox.size.x - anchorRight;
	else {
		switch (hFormat) {
		case hCentre: {
			localPos.x = (parent->drawBox.size.x - drawBox.size.x) / 2; break; }
		case hRight: {
			localPos.x = parent->drawBox.size.x - (drawBox.size.x + parent->borderWidth); break; }
		case hLeft: {
			localPos.x = parent->borderWidth; break; }
		case hSpan: {
			localPos.x = 0; drawBox.size.x = parent->drawBox.size.x; break; }
		}
	}

	if (anchorBottom != NONE)
		//drawBox.size.y = parent->drawBox.size.y - localPos.y - anchorBottom;
		localPos.y = parent->drawBox.size.y - drawBox.size.y - anchorBottom;
	else {
		switch (vFormat) {
		case vCentre: {
			localPos.y = (parent->drawBox.size.y - drawBox.size.y) / 2; break; }
		case vTop: {
			localPos.y = parent->borderWidth; break; }
		case vBottom: {
			localPos.y = parent->drawBox.size.y - drawBox.size.y - parent->borderWidth; break; }
		case vSpan: {
			localPos.y = 0; drawBox.size.y = parent->drawBox.size.y; break; }
		}
	}



	//2. Recalculate clipping
	CalculateClipbox();
	//3. Recalculate screen position
	screenPos.x = parent->screenPos.x + drawBox.pos.x; screenPos.y = parent->screenPos.y + drawBox.pos.y;
	//screenPos.x = parent->screenPos.x + xPos; screenPos.y = parent->screenPos.y + yPos;
	//TO DO: above is legacy, to be replaced with:
	//drawBox.pos = glm::i32vec2(parent->screenPos.x, parent->screenPos.y) + drawBox.pos;
	drawBox.pos = parent->drawBox.pos + localPos;
	//...but screenPos becomes drawBox
	pDrawFuncs->updateScreenDimensions(*this);

	//repeat through children
	for (size_t i = 0; i < Control.size(); i++)
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
	for (size_t i = 0; i < Control.size(); i++) {
		if (!Control[i]->isModal)
			Control[i]->Draw();
	}
}

/** Fire off the necessary drawing instructions to draw this control. */
void CGUIbase::DrawSelf() {
}


/** Calculate the drawable area of this control, and clip it to the drawable area of its parent conntrol. */
/*
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

*/

/** Calculate the drawable area of this control, and clip it to the drawable area of its parent conntrol. */
void CGUIbase::CalculateClipbox() {


	UIrect parentClipbox = parent->Clipbox;
	parentClipbox.x += parent->borderWidth;	parentClipbox.y += parent->borderWidth;
	parentClipbox.width -= parent->borderWidth * 2; parentClipbox.height -= parent->borderWidth * 2;

	Clipbox.width = drawBox.size.x; //drawable width of this control
	Clipbox.x = parent->drawBox.pos.x + localPos.x; //screen x position where drawing of control starts
	if (Clipbox.x <parentClipbox.x) { //is it outside the clipbox of the parent control?
		Clipbox.width -= (parentClipbox.x - Clipbox.x); //clip it to the parent clipbox start
		Clipbox.x = parentClipbox.x;
	}


	Clipbox.height = drawBox.size.y;
	Clipbox.y = parent->drawBox.pos.y + localPos.y;
	if (Clipbox.y < parentClipbox.y) {
		Clipbox.height -= (parentClipbox.y - Clipbox.y);
		if (Clipbox.height < 0)
			Clipbox.height = 0;
		Clipbox.y = parentClipbox.y;
	}

	if ((Clipbox.x + Clipbox.width) >(parentClipbox.x + parentClipbox.width))
		Clipbox.width -= (Clipbox.x + Clipbox.width) - (parentClipbox.x + parentClipbox.width);

	if ((Clipbox.y + Clipbox.height) >(parentClipbox.y + parentClipbox.height))
		Clipbox.height -= (Clipbox.y + Clipbox.height) - (parentClipbox.y + parentClipbox.height);

	//Clipbox.x -= 1; Clipbox.width += 1;
//	Clipbox.height += 1;
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
	Name = NameStr;
}

void CGUIbase::setDefaultFont(CFont* font) {
	defaultFont = font;
}

/** Convert these local coordinate to absolute screen coordinates. */
glm::i32vec2 CGUIbase::getScreenCoords(int x, int y) {
	return glm::i32vec2(drawBox.pos.x + x, drawBox.pos.y + y);
}

glm::i32vec2 CGUIbase::getLocalPos(int x, int y) {
	return glm::i32vec2( x - drawBox.pos.x, y - drawBox.pos.y);
}

int CGUIbase::getWidth() {
	return drawBox.size.x;
}

int CGUIbase::getHeight() {
	return drawBox.size.y;
}

void CGUIbase::resize(int w, int h) {
	width = w; height = h;
	drawBox.size = glm::i32vec2(w, h);
	updateAppearance();
}

void CGUIbase::setGUIcallback(Icallback * callbackInstance) {
	callbackObj = callbackInstance;
}

/** Add given controls to the named row, creating it if necessary. */
void CGUIbase::addToRow(const std::string & name, std::initializer_list<CGUIbase*> childControls) {
	auto row = std::find_if(rowObjects.begin(), rowObjects.end(), [&](CRowObject& obj) { return obj.name == name; });
	int index = row - rowObjects.begin();
	if (row == rowObjects.end()) {
		CRowObject newRowObj;
		newRowObj.setName(name);
		rowObjects.push_back(newRowObj);
		index = rowObjects.size() -1;
	}

	for (auto control : childControls) {
		if (!findControl(control)) //may have already been added
			Add(control); 
		control->setVisible(false);
		rowObjects[index].addControl(control);
	}
}

/** Position every control in every active row object into the next available space, working downwards. */
void CGUIbase::autoArrangeRows(int offsetX, int offsetY) {
	int rowY = offsetY;
	for (auto rowObj : rowObjects) {
		if (rowObj.isActive) {
			for (auto control : rowObj.controls) {
				control->setPos(control->localPos.x + offsetX, rowY);
				control->setVisible(true);
			}
			rowY += rowObj.rowHeight;
		}
		else {
			for (auto control : rowObj.controls) {
				// control->setVisible(false);
			}
		}
	}
}

/** Make all controls in all rows invisible. */
void CGUIbase::hideAllRows() {
	for (auto& rowObj : rowObjects) {
		rowObj.isActive = false;
		for (auto control : rowObj.controls) {
			 control->setVisible(false);
		}
	}

}

/** Activate the given rows. */
void CGUIbase::activateRows( std::initializer_list< std::string> rowNames) {
	for (auto rowName : rowNames) {
		auto row = std::find_if(rowObjects.begin(), rowObjects.end(), [&](CRowObject& obj) { return obj.name == rowName; });
		row->isActive = true;
	}
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
/*
void CGUIbase::message(CGUIbase* sender, CMessage& msg) {
	if (sender->type == radioButton) { //several controls can be radio parents, so catch it here
		//set the other radio buttons to 'off'.
		for (size_t i=0;i<Control.size();i++)
			if ( Control[i]->type == radioButton ) {
				if ( Control[i] != sender) {
					static_cast<CGUIradio*>(Control[i])->Set = false; 
				}
			} 
	}
}*/

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
		KeyCapture->message(this,msg);
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

void CGUIbase::setBackColour1(const UIcolour & colour) {
	backColour1 = colour;
}

void CGUIbase::setBackColour2(const UIcolour & colour) {
	backColour2 = colour;
}

void CGUIbase::setBackColour1(const glm::vec4 & colour) {
	backColour1 = (UIcolour&)colour;
}

void CGUIbase::setBackColour2(const glm::vec4 & colour) {
	backColour2 = (UIcolour&)colour;
}

void CGUIbase::setBorderColour(const UIcolour& colour) {
	borderColour = colour;
}

void CGUIbase::setBorderColour(const glm::vec4& colour) {
	borderColour = (UIcolour&)colour;
}

void CGUIbase::borderOn(bool onOff) {
	drawBorder = onOff;
}

void CGUIbase::makeModal() {
	modalControls.push_back(this);
	isModal = true;
}

void CGUIbase::makeUnModal() {
	for (unsigned int ctrl = 0; ctrl< modalControls.size(); ctrl++)
		if (modalControls[ctrl] == this) {
			modalControls.erase(modalControls.begin() + ctrl);
			isModal = false;
		}
}



