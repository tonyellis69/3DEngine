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
	createTab(paletteImage->drawBox.size.x);
}


void CGUIpaletteBar::setGradientTexture(CBaseTexture & texture) {
	paletteImage->setTexture(texture);
	paletteImage->setTextureData(colourGradient.getData());
}

void CGUIpaletteBar::OnClick(const int mouseX, const int mouseY) {

}


void CGUIpaletteBar::MouseMsg(unsigned int Msg, int mouseX, int mouseY, int key) {
	if (IsOnControl(*paletteImage, mouseX, mouseY)) {
		if (Msg == MY_DOUBLECLICK) {
			if (MouseDown == paletteImage) {
				MouseDown = NULL;
				createTab(mouseX - paletteImage->localPos.x);
			}
		}

	}
	CGUIbase::MouseMsg(Msg, mouseX, mouseY, key);
}

void CGUIpaletteBar::createTab(int mouseX) {
	CGUIpaletteTab* tab = new CGUIpaletteTab(mouseX + paletteImage->localPos.x - (tabSize.x / 2), paletteImage->localPos.y + barHeight,
		tabSize.x, tabSize.y);
	Add(tab);
	float unitPos = mouseX / float(paletteImage->drawBox.size.x);
	tab->position = unitPos * 255.0f;
	glm::vec4 colour = glm::vec4(colourGradient.getColour(tab->position)) / 255.0f;
	tab->setBackColour1(colour);
	tab->setBackColour2(colour);
	updateAppearance();
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
}

void CGUIpaletteBar::deleteTab(CGUIpaletteTab* tab) {
	colourGradient.deleteTab(tab->position);
	delete tab;
	paletteImage->setTextureData(colourGradient.getData());
}

void CGUIpaletteBar::moveTab(CGUIpaletteTab* tab, int newPos) {
	if (newPos < paletteImageStartX - (tabSize.x / 2))
		newPos = paletteImageStartX - (tabSize.x / 2);
	if (newPos > paletteImageEndX - (tabSize.x / 2))
		newPos = paletteImageEndX - (tabSize.x / 2);
	tab->setPos(newPos, tab->localPos.y);
	float unitPos = float(newPos - paletteImageStartX + (tabSize.x / 2)) / paletteImage->drawBox.size.x;
	int tabNewPos = unitPos * 255;
	int tabOldPos = tab->position;
	tab->position = tabNewPos;
	colourGradient.moveTab(tabOldPos, tabNewPos);
	paletteImage->setTextureData(colourGradient.getData());

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
	paletteImage->setTextureData(colourGradient.getData());
	glm::vec4 floatColour = glm::vec4(newColour) / 255.0f;
	colourChangeTab->setBackColour1(floatColour);
	colourChangeTab->setBackColour2(floatColour);
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


