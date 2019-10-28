#include "GUIbase.h"
#include <string>
#include <sstream>

#include "GUIradio.h"
#include "GUIscrollbar.h"
#include "GUIgroup.h"
#include "GUImouse.h"
#include "GUIpopMenu.h"
#include "GUIroot.h"
#include "GUIlabel.h"
#include "GUIbutton.h"

#include "..//3DEngine/src/utils/log.h"

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
	borderColour = (glm::vec4&)UIdarkGrey;
	drawBorder = true;
	uniqueID = UIuniqueIDgen++;
	visible = true;
	callbackObj = NULL;
	isModal = false;
	anchorLeft = false;
	anchorTop = false;
	setGUIcallback(this);
	if (rootUI) {
		setStyleSheet(rootUI->styleSheet);
		controlCursor.rowCol = glm::i32vec2(0); // styleSheet->controlBorder);

	}

	resizesForChildren = false;
	
}

CGUIbase::CGUIbase(int x, int y, int w, int h) : CGUIbase(){
	
	setWidth(w);
	setHeight(h);
	setLocalPos(x, y);
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

/** Create a child control of the requested type. */
CGUIbase * CGUIbase::add(UItype ctrlType, std::string text) {
	CGUIbase* ctrl;
	switch (ctrlType) {
	case uiLabel: ctrl = new CGUIlabel(text); break;
	case uiButton: ctrl = new CGUIbutton(text); break;
	default: return NULL;
	}

	//set position arbitarily for now
	ctrl->setLocalPos(10, 10);
	Add(ctrl);
	positionLogical(ctrl);
	glm::i32vec2 maxSize = layoutControlsCoarse();
	//resize parent if required

	//do fine layou
	layoutFine();

	return ctrl;
}

void CGUIbase::setStyleSheet(CGUIstyleSheet* styleSheet) {
	this->styleSheet = styleSheet;
	resizeMin = styleSheet->resizeMin;
	resizeMax = styleSheet->resizeMax;
}


/** Set the given control's abstract position as a child control, as defined by its
	requirements and by this, its parent control. */
void CGUIbase::positionLogical(CGUIbase * control) {
	control->positionHint.rowCol = controlCursor.rowCol;
	control->positionHint.layoutstyle = controlCursor.currentLayoutStyle;
	control->positionHint.centreRow = controlCursor.centreRowOn;
	controlCursor.advance();
}

/** La yout all child controls based on their row/column positions, ignoring alignment etc. 
	This tells us the space they require, maxSize, useful for sizing controls to children. */
glm::i32vec2 CGUIbase::layoutControlsCoarse() {
	glm::i32vec2 rowCol = glm::i32vec2(0);
	glm::i32vec2 maxSize = glm::i32vec2(0);
	glm::i32vec2 currentPos = glm::i32vec2(styleSheet->controlBorder);
	int currentRowHeight = styleSheet->controlBorder; 
	for (auto control : controls) {
		control->setLocalPos(currentPos.x, currentPos.y);
		currentRowHeight = std::max(currentRowHeight, currentPos.y + control->getSize().y);
		//is this control on the same row as the previous one?
		if (control->positionHint.rowCol.x < control->positionHint.layoutstyle.cols - 1) {
			currentPos.x += control->getSize().x;
			maxSize.x = std::max(maxSize.x, currentPos.x);
			currentPos.x += styleSheet->controlSpacing;
		}
		else { //no? Then place it at the start of a new row
			currentPos = glm::i32vec2(styleSheet->controlBorder, currentRowHeight + styleSheet->controlSpacing);
		}
	}
	maxSize = glm::i32vec2(maxSize.x, currentRowHeight) + glm::i32vec2(styleSheet->controlBorder);
	return maxSize;
}

/** Fine-adjust the position of all child controls according to alignment etc. */
void CGUIbase::layoutFine() {
	glm::i32vec4 cellSize; int vDiff, hDiff; int row = 0;
	std::vector<CGUIbase*> rowCtrls;
	for (auto control : controls) {

		if (control->positionHint.rowCol.y > row) {
			if (rowCtrls[0]->positionHint.centreRow &&
				std::find_if(rowCtrls.begin(), rowCtrls.end(), [](const CGUIbase* x) { return x->positionHint.expansive; }) == rowCtrls.end()) {
				centreCtrlRow(rowCtrls);
			}
			rowCtrls.clear();
			row++;
		}


		rowCtrls.push_back(control);
		cellSize = calcCellSize(control);
		if (control->positionHint.expansive)
			control->setWidth(cellSize.z);

		if (control->positionHint.hAlignment != uiHnone || control->positionHint.vAlignment != uiVnone) {
			vDiff = (cellSize.w - control->getSize().y) / 2;
			hDiff = (cellSize.z - control->getSize().x) / 2;
		}
		if (control->positionHint.vAlignment == uiVcentred) {
			control->setPosY(cellSize.y + vDiff);
		}




		if (control->positionHint.hAlignment == uiHcentred) {
			control->setPosX(cellSize.x + hDiff);
		}
		else if (control->positionHint.hAlignment == uiHleft) {
			control->setPosX(cellSize.x);
		}
		else if (control->positionHint.hAlignment == uiHright) {
			control->setPosX(cellSize.x + (cellSize.z - control->getWidth()) );
		}



	}
}

/** Calculate the width and height of the given cell, and its position, by analyising the size of the controls
	in its row and the preceeding rows. */
glm::i32vec4& CGUIbase::calcCellSize(CGUIbase* cellControl) {
	glm::i32vec2 size(0); glm::i32vec2 pos(0);

	//build a model of the row we're looking at:
	std::vector<CGUIbase*> rowControls;
	int ctrlsInRow = 0; int totalWidths = 0; int expansiveCtrls = 0;
	float availableParentWidth = (float)getSize().x - styleSheet->controlBorder * 2;

	for (auto control : controls) {
		if (control->positionHint.rowCol.y == cellControl->positionHint.rowCol.y) {
			rowControls.push_back(control);
			ctrlsInRow++;
			size.y = std::max(size.y, control->getHeight());
			if (control->positionHint.expansive)
				expansiveCtrls++;
			else
				totalWidths += control->getSize().x;
		}
	}

	availableParentWidth -= styleSheet->controlSpacing * (ctrlsInRow - 1);
	int defaultCellSize = availableParentWidth / ctrlsInRow;

	float cellToCtrlRatio; 
	if (expansiveCtrls) {
		availableParentWidth -= totalWidths;
		cellToCtrlRatio = 1.0f;
	}
	else {
		cellToCtrlRatio = availableParentWidth / totalWidths;
	}

	//build cell widths
	std::map<CGUIbase*,int> cellWidths;
	for (auto control : rowControls) {
		if (control->positionHint.expansive)
			cellWidths[control] = int( availableParentWidth / expansiveCtrls);
		else
			cellWidths[control] =  int(control->getWidth() * cellToCtrlRatio);
			//cellWidths[control] = defaultCellSize; //use for equal-sized cells
	}
	size.x = cellWidths[cellControl];

	//calc cell starting position based on preceding cell widths
	pos.x = styleSheet->controlBorder;
	for (auto control : rowControls) {
		if (control == cellControl)
			break;
		pos.x += cellWidths[control];
		pos.x += styleSheet->controlSpacing;
	}
	pos.y = cellControl->getLocalPos().y;
	return glm::i32vec4(pos, size);
}

/** Adjust the positions of these controls so that combined, they are centred. */
void CGUIbase::centreCtrlRow(std::vector<CGUIbase*>& rowCtrls) {
	//find combined width
	//find difference to parent width
	//find adjustment
	//apply to all controls.
	int combinedWidth = rowCtrls.back()->getLocalPos().x + rowCtrls.back()->getWidth()
		- rowCtrls[0]->getLocalPos().x;
	int startPos = int( (getWidth() - combinedWidth) * 0.5f);
	int adjust = rowCtrls[0]->getLocalPos().x - startPos;
	for (auto control : rowCtrls)
		control->setPosX(control->getLocalPos().x - adjust);

}

/*
glm::i32vec4& CGUIbase::calcCellSize(glm::i32vec2& cellRowCol) {
	glm::i32vec2 size(0);
	glm::i32vec2 pos(0);

	if (resizesForChildren) { //TO DO: not fully realised. Ideally, merge with the code below
		//find the height of the biggest control on this row
		//find the width of the widest control on this column
		for (auto control : controls) {
			if (control->positionHint.rowCol.y == cellRowCol.y)
				size.y = std::max(size.y, control->getSize().y);
			if (control->positionHint.rowCol.x == cellRowCol.x)
				size.x = std::max(size.x, control->getSize().x);
		}
		return glm::i32vec4(pos,size);
	}


	int totalWidths = 0;
	int ctrlWidth = 0; int ctrlsInRow = 0; int prevWidth = -1; int prevCells = 0;
	int rowY = 0;
	//redo this by building a temporary array of ctrls in this row which we then step through to make calculations
	//get working for non-expansive controls first.
	for (auto control : controls) {
		if (control->positionHint.rowCol.y == cellRowCol.y) {
			size.y = control->getSize().y;
			
			if (control->positionHint.rowCol.x == cellRowCol.x) {
				ctrlWidth = control->getSize().x;
				//if control expansive, treat width as minimal for now
				prevWidth = totalWidths;
				prevCells = ctrlsInRow;
				rowY = control->getLocalPos().y;
			}
			ctrlsInRow++;
			totalWidths += control->getSize().x;

		}
	}
	float availableParentWidth = (float)getSize().x - (styleSheet->controlSpacing * (ctrlsInRow-1)) - (styleSheet->controlBorder * 2);
	float ratio = availableParentWidth / totalWidths;
	size.x = int(ctrlWidth * ratio);

	//if *a* control in this row is expansive, but it's not this cell's, 
	//set this cell to the minimum width for the control. ratio becomes 1.
	//if the control in this cell is expansive
	//find the remaining parent width not used by the totalWidths and give it all to this cell

	//to find top left corner of cell, combine widths of previous cells x ratio,
	//then add borders
	pos.x = int((prevWidth) * ratio) + (prevCells  * styleSheet->controlSpacing) + styleSheet->controlBorder;
	pos.y = rowY;

	return glm::i32vec4(pos, size);
}
*/

/**	Position the given child control according to its requirements and this control's own
	situation. */
void CGUIbase::position(CGUIbase * control) {
	//control cursor points to the next free control space
	//control->setLocalPos(controlCursor.pos.x, controlCursor.pos.y);

	//determine the local layout space in case control wants to be centred etc in it
	//get the layout style from currentLayoutStyle
	//find the "working space" - the area we have to work within

	if (control->positionHint.free)
		return;


	if (control->positionHint.hAlignment == uiHcentred || control->positionHint.vAlignment == uiVcentred) {
		glm::i32vec2 childCentre = control->getSize() / 2;
		glm::i32vec2 parentCentre = getSize() / 2;
		glm::i32vec2 newPos = parentCentre - childCentre;
		if (control->positionHint.hAlignment == uiHcentred)
			control->setPosX(newPos.x);
		if (control->positionHint.vAlignment == uiVcentred)
			control->setPosY(newPos.y);
	}


}

glm::i32vec2& CGUIbase::getSize() {
	return drawBox.size;
}

/** Set the layout style to use for placing subsequent controls. */
void CGUIbase::setLayout(CGUIlayout & layout) {
	currentLayoutStyle = layout;
}


CMessage CGUIbase::Message;
CGUIbase* CGUIbase::MouseOver;
CGUIbase* CGUIbase::MouseDown;
CGUIbase* CGUIbase::KeyCapture;
CGUIbase*  CGUIbase::scrollbarHasMouse;
CDrawFuncs* CGUIbase::pDrawFuncs;
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
	//for (size_t i = 0; i < controls.size(); i++) {
	//TO DO: reverse order is a bodge for z order, which I need to implement
	for (int i = controls.size()-1; i >= 0; i--) {
		if (IsOnControl(*controls[i], mouseX, mouseY)) {		
			controls[i]->MouseMsg(Msg, mouseX, mouseY, key);
			return; //TO DO: should only return if MouseMsg returns this message as dealt with!
		}
	}
	//Still here? Message is for this control
	switch (Msg) {
	case WM_MOUSEMOVE: {
		bool mouseOffSuccess = true;
		if (MouseOver != this) { //mouse has just entered this control 
			MouseDown = NULL;	//so remove any mousedown effect the previous control was showing
			if (MouseOver)
				mouseOffSuccess = MouseOver->onMouseOff(mouseX, mouseY, key);
		}
		if (mouseOffSuccess)
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
	setLocalPos(x, y);
	resize(w, h);
}

void CGUIbase::setLocalPos(int x, int y) {
	localPos = glm::i32vec2(x, y);
	//NB: wait until updateAppearance to update drawBox.pos, as it depends on positionHint of parent control
	needsUpdate = true;
}

void CGUIbase::setPosX(int x) {
	setLocalPos(x, localPos.y);
}

void CGUIbase::setPosY(int y) {
	setLocalPos(localPos.x, y);
}





/** Add a child UI element to this UI element. */
void CGUIbase::Add(CGUIbase* child) {
	controls.push_back(child);
	child->parent = this;
	child->needsUpdate = true;
	//TO DO: aboves turns out to be redundant - why?
}


/**	Handle any recent change to this control's size or positionHint to ensure it draws correctly. 
	Eg recalculate drawbox and clipbox.
	This method Recursively calls itself on all child controls, which may now also need to update. */
void CGUIbase::updateAppearance() {
	needsUpdate = false;

	recalculateDiminsions();
	recalculateClipbox();

	for (auto child : controls) //because we've potentially repositioned all these
		child->updateAppearance();
}

/** Recalculate positionHint and size according to the current local positionHint and size of this control, its 
	positional adjustment settings (eg, anchoring), and the positionHint and size of its parent.*/
void CGUIbase::recalculateDiminsions() {
	guiRect origDimensions = { drawBox.pos.x,drawBox.pos.y,drawBox.size.x,drawBox.size.y };

	drawBox.pos = parent->drawBox.pos + localPos;

	//TO DO: probably don't need all these right/centre/etc justifications if I go for smart positioning
	
	//1. Recalculate x,y,w,h *if necessary* due to justification or spanning
	if (anchorRight != NONE) {
		if (anchorLeft)
			setWidth(std::min(getWidth(), parent->getWidth() - (anchorRight + getLocalPos().x)));
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
			setHeight(std::min(getHeight(), parent->getHeight() - (anchorBottom + getLocalPos().y)));
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

	drawBox.pos = parent->drawBox.pos + localPos;

	if (origDimensions.pos == drawBox.pos  && origDimensions.size == drawBox.size)
		needsUpdate = false;

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
	Clipbox.x = parent->drawBox.pos.x + getLocalPos().x; //screen x positionHint where drawing of control starts
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
	liveLog << "\nObject with unique id " << sender->uniqueID << " called message " << msg.Msg
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
				control->setLocalPos(control->getLocalPos().x + offsetX, rowY);
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

/*
CGUIlabel* CGUIbase::addLabel(const std::string& text, int style) {
	CGUIlabel* lbl = new CGUIlabel(text);	
	return (CGUIlabel*) addCtrl(lbl);
}*/





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

/** Returns the positionHint of this control in the child-list of its parent. */
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

unsigned int CGUIbase::getUniqueID() {
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
	borderColour = (glm::vec4&)colour;
}

void CGUIbase::setBorderColour(const glm::vec4& colour) {
	borderColour = (glm::vec4&)colour;
}

void CGUIbase::setBorderColourFocusColour(const glm::vec4 & colour) {
	borderFocusColour = colour;
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

/** Set this control's positioning parameters (centre, left justify, etc)
	accoding to the flags in styleWord. */
void CGUIbase::setPositionStyles(unsigned int styleWord) {
	if (styleWord & uiHleft)
		setHorizontalAlignment(uiHleft);
	else if (styleWord & uiHcentred)
		setHorizontalAlignment(uiHcentred);
	else if (styleWord & uiHright)
		setHorizontalAlignment(uiHright);

	if (styleWord & uiVcentred)
		setVerticalAlignment(uiVcentred);
	else if (styleWord & uiVtop)
		setVerticalAlignment(uiVtop);
	else if (styleWord & uiVbottom)
		setVerticalAlignment(uiVbottom);


}



