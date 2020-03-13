#include "GUIpaletteBar.h"
#include "GUIroot.h"
#include "GUIswatchContainer.h"
#include "..\colour.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/color_space.hpp>
#include <glm/gtx/component_wise.hpp>

CGUIpaletteBar::CGUIpaletteBar(int x, int y, int w, int h) : CGUIpanel(x, y, w, h) {
	type = uiPaletteBar;
	upperPadding = 20;
	barHeight = 30;
	tabSize = glm::i32vec2(15, 20);
	paletteImageStartX = upperPadding;
	paletteImageEndX = w - (2 * upperPadding) + paletteImageStartX;
	paletteImage = new CGUIimage1D(paletteImageStartX, upperPadding, w - (2 * upperPadding), barHeight);
	paletteImage->setBorderOn(false);
	Add(paletteImage);

	colourPicker = new CGUIcolourPicker(600, 300, 500, 400);
	colourPicker->setGUIcallback(this);
	colourPicker->setVisible(false);
	rootUI->Add(colourPicker);

	tabPosLbl = new CGUIlabel2(0, 0, 50, 20);
	tabPosLbl->setBorderOn(false);
	tabPosLbl->setVisible(false);
	Add(tabPosLbl);

	createTab(0);
	createTab(255);

	addControls();
	autoArrangeRows(0, 80);

	paletteTexture.resize(256, 0);
	setGradientTexture(paletteTexture);

	oldHueRotation = 0;
	indicator = -1;
}


void CGUIpaletteBar::setGradientTexture(CBaseTexture & texture) {
	paletteImage->setTexture(texture);
	updatePalette();
}

/** Set the colour gradient we're going to use, updating the palette texture and tabs accordingly. */
void CGUIpaletteBar::setGradient(ColourGradient & gradient) {
	colourGradient = gradient;
	paletteTexture.setPixelData(gradient.getData());
	clearTabControls();
	for (auto tab : colourGradient.getTabs()) {
		createTab(tab.first);
	}

}




bool CGUIpaletteBar::MouseMsg(unsigned int Msg, int mouseX, int mouseY, int key) {
	if (IsOnControl(*paletteImage, mouseX, mouseY)) {
		if (Msg == MY_DOUBLECLICK) {
			if (MouseDown == paletteImage) {
				MouseDown = NULL;
				glm::i32vec2 localMouse = screenToLocalCoords(mouseX, mouseY);
				float unitPos = (localMouse.x - paletteImageStartX) / float(paletteImage->getWidth());
				createTab(unitPos * 255.0f);
			}
		}
	}

	//Catch user dragging a tab
	if (Msg == WM_LBUTTONDOWN) {
		int zPos = -1;
		CGUIbase* topTab = NULL;
		for (auto ctrl : controls) { //make sure it's the top one if they're stacked
			if (ctrl->type == uiPaletteTab && IsOnControl(*ctrl, mouseX, mouseY)) {
				if (ctrl->getLocalPos().x > zPos) {
					zPos = ctrl->getLocalPos().x;
					topTab = ctrl;
				}

			}
		}
		if (topTab) {
			topTab->OnLMouseDown(mouseX, mouseY, key);
			return true;
		}
	}

	if (Msg == WM_LBUTTONUP) {
		if (IsOnControl(*paletteImage, mouseX, mouseY) && dragDropObj != NULL) {
			onDrop(mouseX, mouseY);
			return true;
		}
		tabPosLbl->setVisible(false);

	}


	return CGUIbase::MouseMsg(Msg, mouseX, mouseY, key);
}

void CGUIpaletteBar::createTab(int tabIndex) {
	CGUIpaletteTab* tab = addTab(tabIndex);
	glm::i32vec4 colour = glm::vec4(colourGradient.getColour(tab->position)); 
	colourGradient.changeColour(tab->position, colour); //TO DO: why do I do this?
	tab->colour = colour;
	glm::vec4 realColour = glm::vec4(colour) / 255.0f;
	tab->setBackColour1(realColour);
	tab->setBackColour2(realColour);

	//ensure tabPosLbl is the last thing we draw
	auto tabPosPos = std::find(controls.begin(),controls.end(), tabPosLbl);
	std::iter_swap(tabPosPos, controls.end() -1);
}

CGUIpaletteTab* CGUIpaletteBar::addTab(int tabIndex) {
	int paletteImageProportion = paletteImage->getWidth() *  (tabIndex / 255.0f);
	CGUIpaletteTab* tab = new CGUIpaletteTab(paletteImageProportion + paletteImage->getLocalPos().x - (tabSize.x / 2),
		paletteImage->getLocalPos().y + barHeight, tabSize.x, tabSize.y);
	Add(tab);

	tab->position = tabIndex;
	return tab;
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
	if (msg.Msg == uiMsgSlide && sender == hueSlider) {
		float dHueRotation =   msg.value - oldHueRotation;
		oldHueRotation = msg.value;
		colourGradient.rotateTabHues(dHueRotation);
		for (auto ctrl : controls) {
			if (ctrl->type == uiPaletteTab) {
				glm::vec4 floatColour = glm::vec4(colourGradient.getColour(((CGUIpaletteTab*)ctrl)->position)) / 255.0f;
				ctrl->setBackColour1(floatColour);
				ctrl->setBackColour2(floatColour);
			}
		}
		updatePalette();
		if (callbackObj) {
			CMessage msg;
			msg.Msg = uiMsgUpdate;
			callbackObj->GUIcallback(this, msg);
		}
	}

	//User dropped a swatch onto a tab
	if (sender->type == uiPaletteTab && msg.Msg == uiMsgUpdate) {
		CGUIpaletteTab* tab = static_cast<CGUIpaletteTab*>(sender);
		colourGradient.changeColour(tab->position, tab->colour );
		updatePalette();
	}
}

void CGUIpaletteBar::deleteTab(CGUIpaletteTab* tab) {
	colourGradient.deleteTab(tab->position);
	delete tab;
	updatePalette();
}

void CGUIpaletteBar::moveTab(CGUIpaletteTab* tab, int newPos) {
	int direction = newPos > tab->getLocalPos().x ? 1 : -1;
	int tabOffset = tabSize.x / 2;
	//ensure tabs can travel to the edge of the palette and no further;
	if (newPos < paletteImageStartX - tabOffset )
		newPos = paletteImageStartX - tabOffset ;
	else if (newPos > paletteImageEndX - tabOffset)
		newPos = paletteImageEndX - tabOffset;

	//ensure our tab can't move through another:
	int ctrlPos; 
	int lowerPos = std::min(tab->getLocalPos().x, newPos);
	int higherPos = std::max(tab->getLocalPos().x, newPos);
	for (auto ctrl : controls) {
		if (ctrl->type == uiPaletteTab && ctrl != tab) { //for each compared tab...
			ctrlPos = ctrl->getLocalPos().x;
			//if it lies between this tab and the proposed new positionHint
			//make the new positionHint one less than the compared tab
			if (ctrlPos >= lowerPos && ctrlPos <= higherPos) {
				newPos = ctrlPos - direction;
				break;
			}
		}
	} 

	float unitPos = float( (newPos + tabOffset) - paletteImageStartX ) / paletteImage->getWidth();
	float tabNewPosf = unitPos * 255.0f;
	int tabNewPos = round(tabNewPosf);

	tab->setLocalPos(newPos, tab->getLocalPos().y);
	int tabOldPos = tab->position;
	if (tabOldPos != tabNewPos)
		tab->position = colourGradient.moveTab(tabOldPos, tabNewPos);
	updatePalette();

	//display positionHint
	tabPosLbl->setText(std::to_string(tabNewPos));
	tabPosLbl->setLocalPos(tab->getLocalPos().x + tabSize.x, tab->getLocalPos().y);
	tabPosLbl->setVisible(true);
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



void CGUIpaletteBar::loadPalette() {
	updatePalette();
	clearTabControls();
	for (auto tab : colourGradient.getTabs()) {
		createTab(tab.first);
	}
	updatePalette();
}

/** Delete tab controls only, not touching the colour gradient.*/
void CGUIpaletteBar::clearTabControls() {
	for (auto iter = controls.begin(); iter != controls.end(); ) {
		if ((*iter)->type == uiPaletteTab)
			iter = controls.erase(iter);
		else
			++iter;
	}
}

/** Return an array of every pixel in this colour gradient. */
void * CGUIpaletteBar::getPixels() {
	return colourGradient.getData();
}

void CGUIpaletteBar::updatePalette() {
	paletteTexture.setPixelData(colourGradient.getData());

	CMessage msg;
	msg.Msg = uiMsgUpdate;
	if (callbackObj)
		callbackObj->GUIcallback(this, msg);
}

void CGUIpaletteBar::addControls() {
	hueSlider = new CGUIsysScrollbar(horizontal, paletteImageStartX, 0, 250);
	hueSlider->setMin(0); hueSlider->setMax(360);
	addToRow("hue", { hueSlider });

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

/** Indicate this index positionHint graphically. */
void CGUIpaletteBar::setIndicatorPosition(int indexPos) {
	indicator = indexPos;
}

void CGUIpaletteBar::DrawSelf() {
	if (indicator > -1) {
		guiRect indicatorRect;
		int paletteImageProportion = paletteImage->getWidth() * (indicator / 255.0);
		indicatorRect.pos = { paletteImage->drawBox.pos.x + paletteImageProportion,paletteImage->getHeight() - 10 };
		indicatorRect.size = { 2,10 };

		pDrawFuncs->drawRect2(indicatorRect,style::uiBlack,style::uiBlack);
	}
}

void CGUIpaletteBar::createTabAtIndicator(float falloff) {
	createTab(indicator);
	if (falloff > 0.05) {
		int lowerBound = std::max(0, indicator - int(255 * falloff));
		int upperBound = std::min(255,indicator + int(255 * falloff));

		for (auto ctrl : controls) { //ensure upper/lower tabs don't overlap an existing tab
			if (ctrl->type == uiPaletteTab) {
				int ctrlPos = static_cast<CGUIpaletteTab*>(ctrl)->position;
				if (ctrlPos >= lowerBound && ctrlPos < indicator)
					lowerBound = -1;
				if (ctrlPos <= upperBound && ctrlPos > indicator)
					upperBound = -1;
			}
		}
		if (lowerBound != -1)
			createTab(lowerBound);
		if (upperBound != -1)
			createTab(upperBound);
	}
}

/** Respond to a swatch being dropped on the gradient bar itself, by creating a new tab.*/
void CGUIpaletteBar::onDrop(const int mouseX, const int mouseY) {
	if (IsOnControl(*paletteImage, mouseX, mouseY) && dragDropObj->isDropType(this) ) {
		int tabIndex = (float(mouseX - paletteImage->drawBox.pos.x) / (paletteImage->getWidth()) ) * 255;
		CGUIpaletteTab* tab = addTab(tabIndex);
		colourChangeTab = tab;
		changeTabColour(static_cast<CGUIdragDropSwatch*>(dragDropObj)->colour);
	}
	deleteDragDropObj();
}








bool CGUIpaletteTab::onDoubleClick(const int mouseX, const int mouseY, int key) {
	scrollbarHasMouse = NULL;
	pDrawFuncs->mouseCaptured(false);

	CMessage msg;
	msg.Msg = MY_DOUBLECLICK;
	parent->message(this, msg);
	return true;
}

bool CGUIpaletteTab::OnLMouseDown(const  int mouseX, const  int mouseY, int key) {
	MouseDown = this;
	int mousePos;

	scrollbarHasMouse = this;
	pDrawFuncs->mouseCaptured(true);
	lastMouseX = mouseX; 
	return true;
}

bool CGUIpaletteTab::onRMouseUp(const int mouseX, const int mouseY) {
	CMessage msg;
	msg.Msg = WM_RBUTTONUP;
	parent->message(this, msg);
	return true;
}

bool CGUIpaletteTab::OnMouseMove(int mouseX, int mouseY, int key) {
	if ((MouseDown == this) && (scrollbarHasMouse == this)) {
		CMessage msg;
		msg.Msg = WM_MOUSEMOVE;
		msg.x = mouseX  - lastMouseX + getLocalPos().x; // localToScreenCoords(mouseX, mouseY).x - lastMouseX + localPos.x;
		lastMouseX = mouseX;// localToScreenCoords(mouseX, mouseY).x;
		parent->message(this, msg);

	}
	return true;
}

/** Catch an attempt to drop a swatch. */
void CGUIpaletteTab::onDrop(const int mouseX, const int mouseY) {
	if (dragDropObj->isDropType(this)) {
		glm::i32vec4 swatchColour = glm::vec4(static_cast<CGUIdragDropSwatch*>(dragDropObj)->colour);
		glm::vec4 realColour = glm::vec4(swatchColour) / 255.0f;
		setBackColour1(realColour);
		setBackColour2(realColour);
		colour = swatchColour;
		CMessage msg;
		msg.Msg = uiMsgUpdate;
		parent->message(this, msg);
	}
	deleteDragDropObj();
}

/** Catch mousewheeling to adjust brightness (saturation). */
bool CGUIpaletteTab::MouseWheelMsg(const int mouseX, const int mouseY, int wheelDelta, int key) {
	if ( glm::compMin(glm::vec3(colour)) == glm::compMax(glm::vec3(colour)) )
		return true;

	float increment = float(wheelDelta) * 0.1f;
	glm::vec3 rgb = col::byteToFloat(colour);
	glm::vec3 hsv = glm::hsvColor(rgb);
	hsv.y += increment;
	hsv.y = glm::clamp(hsv.y,0.01f, 1.0f);
	rgb = glm::rgbColor(hsv);
	colour = glm::i32vec4(rgb * 255.0f, colour.a);
	glm::vec4 realColour = glm::vec4(colour) / 255.0f;
	setBackColour1(realColour);
	setBackColour2(realColour);
	CMessage msg;
	msg.Msg = uiMsgUpdate;
	parent->message(this, msg);
	return true;
}



