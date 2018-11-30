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

	tabPos = new CGUIlabel2(0, 0, 50, 20);
	tabPos->borderOn(false);
	tabPos->setVisible(false);
	Add(tabPos);

	createTab(0);
	createTab(255);

	addControls();
	autoArrangeRows(0, 80);

	paletteTexture.resize(256, 0);
	setGradientTexture(paletteTexture);


}


void CGUIpaletteBar::setGradientTexture(CBaseTexture & texture) {
	paletteImage->setTexture(texture);
	updatePalette();
}

/** Set the colour gradient we're going to use, updating the palette texture and tabs accordingly. */
void CGUIpaletteBar::setGradient(ColourGradient & gradient) {
	colourGradient = gradient;
	paletteTexture.setData(gradient.getData());
	clearTabControls();
	for (auto tab : colourGradient.getTabs()) {
		createTab(tab.first);
	}

}

void CGUIpaletteBar::OnClick(const int mouseX, const int mouseY) {

}


void CGUIpaletteBar::MouseMsg(unsigned int Msg, int mouseX, int mouseY, int key) {
	if (IsOnControl(*paletteImage, mouseX, mouseY)) {
		if (Msg == MY_DOUBLECLICK) {
			if (MouseDown == paletteImage) {
				MouseDown = NULL;
				glm::i32vec2 localMouse = getLocalPos(mouseX, mouseY);
				float unitPos = (localMouse.x - paletteImageStartX) / float(paletteImage->drawBox.size.x);
				createTab(unitPos * 255.0f);
			}
		}
	}

	//Catch user dragging a tab
	if (Msg == WM_LBUTTONDOWN) {
		int zPos = -1;
		CGUIbase* topTab = NULL;
		for (auto ctrl : Control) { //make sure it's the top one if they're stacked
			if (ctrl->type == uiPaletteTab && IsOnControl(*ctrl, mouseX, mouseY)) {
				if (ctrl->localPos.x > zPos) {
					zPos = ctrl->localPos.x;
					topTab = ctrl;
				}

			}
		}
		if (topTab) {
			topTab->OnLMouseDown(mouseX, mouseY, key);
			return;
		}
	}

	if (Msg == WM_LBUTTONUP) {
		tabPos->setVisible(false);

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
	tab->colour = colour;
	colour = colour / 255.0f;
	tab->setBackColour1(colour);
	tab->setBackColour2(colour);

	//ensure tabPos is the last thing we draw
	auto tabPosPos = std::find(Control.begin(),Control.end(), tabPos);
	std::iter_swap(tabPosPos, Control.end() -1);
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
//	if (msg.Msg == uiClick && sender->getID() == logButtonID) {
	//	logPalette();
	//}
}

void CGUIpaletteBar::deleteTab(CGUIpaletteTab* tab) {
	colourGradient.deleteTab(tab->position);
	delete tab;
	updatePalette();
}

void CGUIpaletteBar::moveTab(CGUIpaletteTab* tab, int newPos) {
	int direction = newPos > tab->localPos.x ? 1 : -1;
	int tabOffset = tabSize.x / 2;
	//ensure tabs can travel to the edge of the palette and no further;
	if (newPos < paletteImageStartX - tabOffset )
		newPos = paletteImageStartX - tabOffset ;
	else if (newPos > paletteImageEndX - tabOffset)
		newPos = paletteImageEndX - tabOffset;

	//ensure our tab can't move through another:
	int ctrlPos; 
	int lowerPos = min(tab->localPos.x, newPos);
	int higherPos = max(tab->localPos.x, newPos);
	for (auto ctrl : Control) {
		if (ctrl->type == uiPaletteTab && ctrl != tab) { //for each compared tab...
			ctrlPos = ctrl->localPos.x;
			//if it lies between this tab and the proposed new position
			//make the new position one less than the compared tab
			if (ctrlPos >= lowerPos && ctrlPos <= higherPos) {
				newPos = ctrlPos - direction;
				break;
			}

		}
	} 

	float unitPos = float( (newPos + tabOffset) - paletteImageStartX ) / paletteImage->drawBox.size.x;
	float tabNewPosf = unitPos * 255.0f;
	int tabNewPos = round(tabNewPosf);



	tab->setPos(newPos, tab->localPos.y);
	int tabOldPos = tab->position;
	if (tabOldPos != tabNewPos)
		tab->position = colourGradient.moveTab(tabOldPos, tabNewPos);
	updatePalette();

	//display position
	tabPos->setText(std::to_string(tabNewPos));
	tabPos->setPos(tab->localPos.x + tabSize.x, tab->localPos.y);
	tabPos->setVisible(true);
}

void CGUIpaletteBar::GUIcallback(CGUIbase* sender, CMessage& msg) {
	if (sender == colourPicker) {
		if (msg.Msg == uiOKid) {
			changeTabColour(colourPicker->getColour());
		}
		colourChangeTab = NULL;
		colourPicker->setVisible(false);
		colourPicker->makeUnModal();
	}
	if (sender == saveButton)
		save();
	if (sender == restoreMenu) {
		msg.Msg = uiMsgRestore;
		callbackObj->GUIcallback(this, msg);
	}
		
}

void CGUIpaletteBar::editTabColour(CGUIpaletteTab* sender) {
	colourChangeTab = sender;
	//display colour picker modally
	colourPicker->setColour(sender->colour);
	colourPicker->makeModal();
	colourPicker->setVisible(true);
}

void CGUIpaletteBar::changeTabColour(glm::i32vec4& newColour) {
	int position = colourChangeTab->position;
	colourGradient.changeColour(position, newColour);
	updatePalette();
	glm::vec4 floatColour = glm::vec4(newColour) / 255.0f;
	colourChangeTab->setBackColour1(floatColour);
	colourChangeTab->setBackColour2(floatColour);
	colourChangeTab->colour = newColour;
}

void CGUIpaletteBar::logPalette() {
	for (auto tab : colourGradient.getTabs()) {
		glm::i32vec4 colour = tab.second; 
		std::cerr << "\ncolourGradient.changeColour(" << tab.first << ", glm::i32vec4(" << colour.r << ", " << colour.g <<
			", " << colour.b << ", " << colour.a << "));";
	}
}

void CGUIpaletteBar::loadPalette() {
	/**
	//wood
	colourGradient.changeColour(0, glm::i32vec4(189, 94, 4, 255));
	colourGradient.changeColour(191, glm::i32vec4(144, 48, 6, 255));
	colourGradient.changeColour(255, glm::i32vec4(60, 10, 8, 255));
	*/

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

void CGUIpaletteBar::addControls() {
	CGUIlabel2* lbl = new CGUIlabel2(paletteImageStartX, 0, 70, 25);
	lbl->setText("Load file");
	restoreMenu = new CGUIdropdownMenu(paletteImageStartX + 80, 0, 80, 20);
	restoreMenu->setText("[select]");
	restoreMenu->setGUIcallback(this);
	addToRow("restore", { lbl, restoreMenu });

	saveButton = new CGUIbutton2(paletteImageStartX, 80, 70, 20);
	saveButton->setText("Save as");
	saveButton->setGUIcallback(this);
	fileNameBox = new CGUItextbox2(paletteImageStartX + 80, 80, 80, 20);
	fileNameBox->setText("Palette1");
	addToRow("save", { saveButton, fileNameBox });
}

/** Save the palette as a .pal file. */
void CGUIpaletteBar::save() {
	if (!callbackObj)
		return;
	CMessage msg;
	msg.Msg = uiMsgSave;
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
	lastMouseX = mouseX; //getScreenCoords(mouseX, mouseY).x;
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
		msg.x = mouseX  - lastMouseX + localPos.x; // getScreenCoords(mouseX, mouseY).x - lastMouseX + localPos.x;
		lastMouseX = mouseX;// getScreenCoords(mouseX, mouseY).x;
		parent->message(this, msg);

	}
}



