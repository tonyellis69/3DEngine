#include "GUIbase.h"
#include <string>
#include <sstream>

#include "GUIradio.h"
#include "GUIscrollbar.h"
#include "GUIgroup.h"
#include "GUImouse.h"
#include "GUIpopMenu.h"
#include "GUIroot.h"

// TO DO: get rid of 'Name' and 'Count'. It's messy and I don't use it. 

unsigned int UIuniqueIDgen = 0;

CGUIdragDrop* CGUIbase::dragDropObj = NULL;


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
	//width = 30; height = 20;
	setWidth(30);
	setHeight(20);

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
	anchorLeft = false;
	anchorTop = false;
	setGUIcallback(this);
}

CGUIbase::CGUIbase(int x, int y, int w, int h) : CGUIbase(){
	
	setWidth(w);
	setHeight(h);
	setPos(x, y);
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
	auto it = parent->controls.begin();
	for (; it != parent->controls.end(); it++) {
		if (*it == this) {
			parent->controls.erase(it);
			break;
		}
	}

	for (unsigned int i = 0; i < controls.size(); i++)
		if (controls[i] != NULL)
			delete controls[i];
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
	for (size_t i = 0; i < controls.size(); i++) {
		if (IsOnControl(*controls[i], mouseX, mouseY)) {		
			controls[i]->MouseMsg(Msg, mouseX, mouseY, key);
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
		if (MouseDown == this) { //looks like we're dragging
			onDrag(mouseX, mouseY);
			//return;
		}
		OnMouseMove(mouseX, mouseY, key); 
		return; }
	case WM_LBUTTONDOWN: {
		uncaptureKeyboard();
		MouseDown = this;
		OnLMouseDown(mouseX, mouseY, key);
		return; }
	case WM_RBUTTONDOWN: {
		uncaptureKeyboard();
		MouseDown = this;
		OnRMouseDown(mouseX, mouseY, key);
		return; }
	case WM_LBUTTONUP: {
		if (dragDropObj != NULL) {
			onDrop(mouseX, mouseY);
			//return;
		}
		else {
			OnLMouseUp(mouseX, mouseY);
			if (MouseDown == this) //we've been clicked on!
				OnClick(mouseX, mouseY);
		}
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
		onDoubleClick(mouseX, mouseY, key);
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

	for (size_t i=0;i < controls.size();i++)
		if ( IsOnControl( *controls[i],mouseX, mouseY) ) {
			if (controls[i]->MouseWheelMsg(mouseX , mouseY ,wheelDelta,key))
				return true;
		}
	return false;
}


void CGUIbase::onDrop(const int mouseX, const int mouseY) {
	if (dragDropObj) {
		deleteDragDropObj();
	}
}

/** Convenience function for setting all dimensions at once. */
void CGUIbase::setLocalDimensions(int x, int y, int w, int h) {
	setPos(x, y);
	resize(w, h);
}

void CGUIbase::setPos(int x, int y) {
	localPos = glm::i32vec2(x, y);
	//NB: wait until updateAppearance to update drawBox.pos, as it depends on position of parent control
	needsUpdate = true;

}

void CGUIbase::setPosX(int x) {
	setPos(x, localPos.y);
}

void CGUIbase::setPosY(int y) {
	setPos(localPos.x, y);
}





/** Add a child UI element to this UI element. */
void CGUIbase::Add(CGUIbase* child) {
	controls.push_back(child);
	child->parent = this;
	child->needsUpdate = true;
	//TO DO: aboves turns out to be redundant - why?
}


/**	Handle any recent change to this control's size or position to ensure it draws correctly. 
	Eg recalculate drawbox and clipbox.
	This method Recursively calls itself on all child controls, which may now also need to update. */
void CGUIbase::updateAppearance() {
	needsUpdate = false;

	recalculateDiminsions();
	recalculateClipbox();

	for (auto child : controls) //because we've potentially repositioned all these
		child->updateAppearance();
}

/** Recalculate position and size according to the current local position and size of this control, its 
	positional adjustment settings (eg, anchoring), and the position and size of its parent.*/
void CGUIbase::recalculateDiminsions() {
	drawBox.pos = parent->drawBox.pos + localPos;

	//TO DO: probably don't need all these right/centre/etc justifications if I go for smart positioning
	
	//1. Recalculate x,y,w,h *if necessary* due to justification or spanning
	if (anchorRight != NONE) {
		if (anchorLeft)
			setWidth(min(getWidth(), parent->getWidth() - (anchorRight + getLocalPos().x)));
		else
			setPosX(parent->getWidth() - getWidth() - anchorRight);
	}
	else {
		switch (hFormat) {
		case hCentre: {
			setPosX((parent->getWidth() - getWidth()) / 2); break; }
		case hRight: {
			setPosX(parent->getWidth() - (getWidth() + parent->borderWidth)); break; }
		case hLeft: {
			setPosX(parent->borderWidth); break; }
		case hSpan: {
			setPosX(0); setWidth(parent->getWidth()); break; }
		}
	}

	if (anchorBottom != NONE)
		if (anchorLeft)
			setHeight(min(getHeight(), parent->getHeight() - (anchorBottom + getLocalPos().y)));
		else
			setPosY(parent->getHeight() - getHeight() - anchorBottom);
	else {
		switch (vFormat) {
		case vCentre: {
			setPosY((parent->getHeight() - getHeight()) / 2); break; }
		case vTop: {
			setPosY(parent->borderWidth); break; }
		case vBottom: {
			setPosY(parent->getHeight() - getHeight() - parent->borderWidth); break; }
		case vSpan: {
			setPosY(0); setHeight(parent->getHeight()); break; }
		}
	}

}



/** Draw this UI element. In practice this means recursively drawing all its child items too.*/
void CGUIbase::Draw() {
	if (!visible)
		return;
	if (needsUpdate)
		updateAppearance();
	
	//declare a box beyond which we do not draw
	pDrawFuncs->setClip(Clipbox);

	DrawSelf(); 

	//then draw each subcontrol
	for (auto control : controls) {
		if (!control->isModal) //modal controls go on top of everything else
			control->Draw();
	}
}


/** Fire off the necessary drawing instructions to draw this control. */
void CGUIbase::DrawSelf() {
}


/** Calculate the drawable area of this control, and clip it by the drawable area of its parent conntrol. */
void CGUIbase::recalculateClipbox() {
	UIrect parentClipbox = parent->Clipbox;
	parentClipbox.x += parent->borderWidth;	parentClipbox.y += parent->borderWidth;
	parentClipbox.width -= parent->borderWidth * 2; parentClipbox.height -= parent->borderWidth * 2;

	Clipbox.width = getWidth(); //drawable width of this control
	Clipbox.x = parent->drawBox.pos.x + getLocalPos().x; //screen x position where drawing of control starts
	if (Clipbox.x <parentClipbox.x) { //is it outside the clipbox of the parent control?
		Clipbox.width -= (parentClipbox.x - Clipbox.x); //clip it to the parent clipbox start
		Clipbox.x = parentClipbox.x;
	}


	Clipbox.height = getHeight(); //repeat process for top of drawing area
	Clipbox.y = parent->drawBox.pos.y + getLocalPos().y;
	if (Clipbox.y < parentClipbox.y) {
		Clipbox.height -= (parentClipbox.y - Clipbox.y);
		if (Clipbox.height < 0)
			Clipbox.height = 0;
		Clipbox.y = parentClipbox.y;
	}

	//now width and height
	if ((Clipbox.x + Clipbox.width) >(parentClipbox.x + parentClipbox.width))
		Clipbox.width -= (Clipbox.x + Clipbox.width) - (parentClipbox.x + parentClipbox.width);

	if ((Clipbox.y + Clipbox.height) >(parentClipbox.y + parentClipbox.height))
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
	Name = NameStr;
}

void CGUIbase::setDefaultFont(CFont* font) {
	defaultFont = font;
}

/** Convert these local coordinate to absolute screen coordinates. */
glm::i32vec2 CGUIbase::localToScreenCoords(int x, int y) {
	return glm::i32vec2(drawBox.pos.x + x, drawBox.pos.y + y);
}

glm::i32vec2 CGUIbase::screenToLocalCoords(int x, int y) {
	return glm::i32vec2( x - drawBox.pos.x, y - drawBox.pos.y);
}

glm::i32vec2& CGUIbase::getLocalPos() {
	return localPos;
}

int CGUIbase::getWidth() {
	return drawBox.size.x;
}

int CGUIbase::getHeight() {
	return drawBox.size.y;
}

void CGUIbase::setWidth(int w) {
	drawBox.size.x = w;
	needsUpdate = true;
}

void CGUIbase::setHeight(int h) {
	drawBox.size.y = h;
	needsUpdate = true;
}

const glm::i32vec2 CGUIbase::getScreenPos(){
	return drawBox.pos;
}

void CGUIbase::resize(int w, int h) {
	if (w < 1)
		w = 1;
	if (h < 1)
		h = 1;
	setWidth(w);
	setHeight(h);
}

void CGUIbase::setGUIcallback(Icallback * callbackInstance) {
	callbackObj = callbackInstance;
}

/** Default callback for GUI messages. Controls' callbackObj is set to this on initialisation
	to avoid calling on NULL. */
void CGUIbase::GUIcallback(CGUIbase * sender, CMessage & msg) {
	cerr << "\nObject with unique id " << sender->uniqueID << " called message " << msg.Msg
		<< " value " << msg.value;
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
				control->setPos(control->getLocalPos().x + offsetX, rowY);
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

void CGUIbase::deleteDragDropObj() {
	delete dragDropObj;
	dragDropObj = NULL;
}

/** Launch a handy popup menu. */
void CGUIbase::popupMenu(std::initializer_list<std::string> options,  IPopupMenu* callback) {
	CGUIpopMenu* popupMenu = new CGUIpopMenu(rootUI->mousePos.x, rootUI->mousePos.y, 200, 50);
	popupMenu->setMenuCallback(callback);
	popupMenu->resizeHorizontal = true;
	popupMenu->maxItemsShown = options.size();

	popupMenu->addItem(options);
	Add(popupMenu);
	popupMenu->makeModal();
}



/** Return a pointer to the child control of this control with the given id number. */
CGUIbase* CGUIbase::getChild(int childID) {
	for (auto control : controls){
		if ( control->id == childID )
			return control;
	}
	return NULL;
}

/** Recursively search this control and its children for the control with the given id number. */
CGUIbase* CGUIbase::findControl(int childID) {
	CGUIbase* result = NULL;
	if (id == childID )
		return this;
	for (auto control : controls){
		result = control->findControl(childID);
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
	for (auto control : controls){
		result = control->findControl(child);
		if (result != NULL) 
			break;
	}
	return result;	
}


void CGUIbase::OnLMouseUp(const int mouseX, const int mouseY) {
	
}

void CGUIbase::onRMouseUp(const int mouseX, const int mouseY) {

}

/** Returns the position of this control in the child-list of its parent. */
int CGUIbase::getControlNo() {
	int x=0;
	while (parent->controls[x] != this) {
		x++;
	}
	return x;
}

	
/** Disable or enable this contol. */
void CGUIbase::setEnabled(bool onOff) {
	enabled = onOff;
}

bool CGUIbase::getEnabled() {
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

void CGUIbase::setBorderOn(bool onOff) {
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



