#include "GUIpaletteBar.h"
#include "GUIroot.h"

CGUIpaletteBar::CGUIpaletteBar(int x, int y, int w, int h) : CGUIpanel(x, y, w, h) {
	upperPadding = 20;
	barHeight = 30;
	tabSize = glm::i32vec2(15, 20);
	paletteImageStartX = upperPadding;
	paletteImageEndX = w - (2 * upperPadding) + paletteImageStartX;
	paletteImage = new CGUIimage1D(paletteImageStartX, upperPadding, w - (2 * upperPadding), barHeight);
	paletteImage->borderOn(false);
	Add(paletteImage);

	colourPicker = new CGUIcolourPicker(600, 300, 500, 400);
	colourPicker->setGUIcallback(this);
	colourPicker->setVisible(false);
	rootUI->Add(colourPicker);

	createTab(0);
	createTab(255);

	CGUIbutton2* button = new CGUIbutton2(paletteImageStartX, 100, 70, 25);
	button->setText("Log");
	logButtonID = button->getID();
	Add(button);

	paletteTexture.resize(256, 0);
	setGradientTexture(paletteTexture);
}


void CGUIpaletteBar::setGradientTexture(CBaseTexture & texture) {
	paletteImage->setTexture(texture);
	updatePalette();
}

void CGUIpaletteBar::OnClick(const int mouseX, const int mouseY) {

}


void CGUIpaletteBar::MouseMsg(unsigned int Msg, int mouseX, int mouseY, int key) {
	if (IsOnControl(*paletteImage, mouseX, mouseY)) {
		if (Msg == MY_DOUBLECLICK) {
			if (MouseDown == paletteImage) {
				MouseDown = NULL;
				float unitPos = ( mouseX - paletteImageStartX) / float(paletteImage->drawBox.size.x);
				createTab(unitPos * 255.0f);
			}
		}

	}
	CGUIbase::MouseMsg(Msg, mouseX, mouseY, key);
}

void CGUIpaletteBar::createTab(int mouseX) {
	int paletteImagePosX = paletteImage->drawBox.size.x *  (mouseX / 255.0f);
	CGUIpaletteTab* tab = new CGUIpaletteTab(paletteImagePosX + paletteImage->localPos.x - (tabSize.x / 2), 
		paletteImage->localPos.y + barHeight, tabSize.x, tabSize.y);
	Add(tab);
	//float unitPos = mouseX / float(paletteImage->drawBox.size.x);
	//tab->position = unitPos * 255.0f;
	tab->position = mouseX;
	
	glm::vec4 colour = glm::vec4(colourGradient.getColour(tab->position)); 
	colourGradient.changeColour(tab->position, glm::i32vec4(colour));
	colour = colour / 255.0f;
	tab->setBackColour1(colour);
	tab->setBackColour2(colour);
	//updateAppearance();
	//updatePalette();
}

void CGUIpaletteBar::message(CGUIbase* sender, CMessage& msg) {
	if (msg.Msg == WM_RBUTTONUP) {
		deleteTab((CGUIpaletteTab*)sender);
	}
	if (msg.Msg == MY_DOUBLECLICK) {
		editTabColour((CGUIpaletteTab*)sender);
	}
	if (msg.Msg == WM_MOUSEMOVE) {
		moveTab((CGUIpaletteTab*)sender, msg.x);
	}
	if (msg.Msg == uiClick && sender->getID() == logButtonID) {
		logPalette();
	}
}

void CGUIpaletteBar::deleteTab(CGUIpaletteTab* tab) {
	colourGradient.deleteTab(tab->position);
	delete tab;
	updatePalette();
}

void CGUIpaletteBar::moveTab(CGUIpaletteTab* tab, int newPos) {
	if (newPos < paletteImageStartX - (tabSize.x / 2))
		newPos = paletteImageStartX - (tabSize.x / 2);
	else if (newPos > paletteImageEndX - (tabSize.x / 2))
		newPos = paletteImageEndX - (tabSize.x / 2);


	int ctrlPos; 
	int minPos = min(tab->localPos.x, newPos);
	int maxPos = max(tab->localPos.x, newPos);
	for (auto ctrl : Control) {
		if (ctrl->type == uiPaletteTab && ctrl != tab) {
			ctrlPos = ctrl->localPos.x; 
			if (ctrlPos >= minPos && ctrlPos <= maxPos) {
				if (tab->localPos.x == maxPos)
					newPos = ctrlPos + 1;
				else
					newPos = ctrlPos - 1;
				break;
			}
		}
	}

	float unitPos = float(newPos - paletteImageStartX + (tabSize.x / 2)) / paletteImage->drawBox.size.x;
	int tabNewPos = unitPos * 255.0f;
	tab->setPos(newPos, tab->localPos.y);
	int tabOldPos = tab->position;
	if (tabOldPos != tabNewPos)
		tab->position = colourGradient.moveTab(tabOldPos, tabNewPos);
	//paletteImage->setTextureData(colourGradient.getData());
	updatePalette();

}

void CGUIpaletteBar::GUIcallback(CGUIbase* sender, CMessage& msg) {
	if (msg.Msg == uiClick) {
		if (sender->id = uiOKid) {
			changeTabColour(colourPicker->getColour());
		}
		colourChangeTab = NULL;
		colourPicker->setVisible(false);
		colourPicker->makeUnModal();
	}
}

void CGUIpaletteBar::editTabColour(CGUIpaletteTab* sender) {
	colourChangeTab = sender;
	//display colour picker modally
	colourPicker->makeModal();
	colourPicker->setVisible(true);
}

void CGUIpaletteBar::changeTabColour(glm::i32vec4& newColour) {
	int position = colourChangeTab->position;
	colourGradient.changeColour(position, newColour);
	//paletteImage->setTextureData(colourGradient.getData());
	updatePalette();
	glm::vec4 floatColour = glm::vec4(newColour) / 255.0f;
	colourChangeTab->setBackColour1(floatColour);
	colourChangeTab->setBackColour2(floatColour);
}

void CGUIpaletteBar::logPalette() {
	for (auto tab : colourGradient.getTabs()) {
		glm::i32vec4 colour = tab.second; 
		std::cerr << "\ncolourGradient.changeColour(" << tab.first << ", glm::i32vec4(" << colour.r << ", " << colour.g <<
			", " << colour.b << ", " << colour.a << "));";
	}
}

void CGUIpaletteBar::loadPalette() {
/*	colourGradient.changeColour(0, glm::i32vec4(0, 0, 238, 255));
	colourGradient.changeColour(10, glm::i32vec4(0, 0, 238, 255));
	colourGradient.changeColour(130, glm::i32vec4(67, 69, 245, 255));
	colourGradient.changeColour(132, glm::i32vec4(231, 175, 0, 255));
	colourGradient.changeColour(136, glm::i32vec4(42, 175, 0, 255));
	colourGradient.changeColour(200, glm::i32vec4(42, 117, 0, 255));
	colourGradient.changeColour(246, glm::i32vec4(225, 221, 227, 255));
	colourGradient.changeColour(255, glm::i32vec4(225, 221, 227, 255));
	*/

	/*
	//Jade
	colourGradient.changeColour(0, glm::i32vec4(24, 146, 102, 255));
	colourGradient.changeColour(128, glm::i32vec4(78, 154, 115, 255));
	colourGradient.changeColour(159, glm::i32vec4(128, 204, 165, 255));
	colourGradient.changeColour(175, glm::i32vec4(78, 154, 115, 255));
	colourGradient.changeColour(255, glm::i32vec4(29, 135, 102, 255));
	*/

	/**
	//wood
	colourGradient.changeColour(0, glm::i32vec4(189, 94, 4, 255));
	colourGradient.changeColour(191, glm::i32vec4(144, 48, 6, 255));
	colourGradient.changeColour(255, glm::i32vec4(60, 10, 8, 255));
	*/

	colourGradient.changeColour(0, glm::i32vec4(0, 0, 0, 255));
	colourGradient.changeColour(8, glm::i32vec4(0, 0, 0, 255));
	colourGradient.changeColour(16, glm::i32vec4(216, 216, 242, 255));
	colourGradient.changeColour(128, glm::i32vec4(191, 191, 191, 255));
	colourGradient.changeColour(191, glm::i32vec4(210, 116, 125, 255));
	colourGradient.changeColour(223, glm::i32vec4(210, 113, 98, 255));
	colourGradient.changeColour(255, glm::i32vec4(255, 176, 192, 255));


	updatePalette();
	clearTabControls();
	for (auto tab : colourGradient.getTabs()) {
		createTab(tab.first);
	}
	updatePalette();
}

/** Delete tab controls only, not touching the colour gradient.*/
void CGUIpaletteBar::clearTabControls() {
	for (auto iter = Control.begin(); iter != Control.end(); ) {
		if ((*iter)->type == uiPaletteTab)
			iter = Control.erase(iter);
		else
			++iter;
	}
}

/** Return an array of every pixel in this colour gradient. */
void * CGUIpaletteBar::getPixels() {
	return colourGradient.getData();
}

void CGUIpaletteBar::updatePalette() {
//	paletteImage->setTextureData(colourGradient.getData());
	paletteTexture.setData(colourGradient.getData());

	CMessage msg;
	msg.Msg = uiMsgUpdate;
	if (callbackObj)
		callbackObj->GUIcallback(this, msg);
}



void CGUIpaletteTab::onDoubleClick(const int mouseX, const int mouseY) {
	scrollbarHasMouse = NULL;
	pDrawFuncs->mouseCaptured(false);

	CMessage msg;
	msg.Msg = MY_DOUBLECLICK;
	parent->message(this, msg);
}

void CGUIpaletteTab::OnLMouseDown(const  int mouseX, const  int mouseY, int key) {
	//return;
	MouseDown = this;
	int mousePos;

	scrollbarHasMouse = this;
	pDrawFuncs->mouseCaptured(true);
	lastMouseX = getScreenCoords(mouseX, mouseY).x;
}

void CGUIpaletteTab::onRMouseUp(const int mouseX, const int mouseY) {
	CMessage msg;
	msg.Msg = WM_RBUTTONUP;
	parent->message(this, msg);
}

void CGUIpaletteTab::OnMouseMove(int mouseX, int mouseY, int key) {
	if ((MouseDown == this) && (scrollbarHasMouse == this)) {
		CMessage msg;
		msg.Msg = WM_MOUSEMOVE;
		msg.x = getScreenCoords(mouseX, mouseY).x - lastMouseX + localPos.x;
		lastMouseX = getScreenCoords(mouseX, mouseY).x;
		parent->message(this, msg);

	}
}


