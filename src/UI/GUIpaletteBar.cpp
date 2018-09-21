#include "GUIpaletteBar.h"
#include "GUIroot.h"

CGUIpaletteBar::CGUIpaletteBar(int x, int y, int w, int h) : CGUIpanel( x,  y,  w,  h) {
	upperPadding = 20;
	barHeight = 30;
	tabSize = glm::i32vec2(15,20);
	paletteImage = new CGUIimage1D(upperPadding, upperPadding, w - (2 * upperPadding), barHeight);
	Add(paletteImage);
	
	colourPicker = new CGUIcolourPicker(600, 300, 500, 400);
	colourPicker->setGUIcallback(this);
	colourPicker->setVisible(false);
	rootUI->Add(colourPicker);

	colourGradient.insertColour(64, { 255,0,0,255 });
}

/*
void CGUIpaletteBar::DrawSelf() {

	//pDrawFuncs->drawCtrlRect(*this);

	//if (drawBorder) {
	//	pDrawFuncs->drawCtrlBorder(*this);
	//}
	CGUIpanel::DrawSelf();
}*/

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
				createTab(mouseX - paletteImage->localPos.x, mouseY - paletteImage->localPos.y);
			}
		}

	}
	CGUIbase::MouseMsg(Msg, mouseX, mouseY, key);
}

void CGUIpaletteBar::createTab(int mouseX, int mouseY) {
	CGUIpaletteTab* tab = new CGUIpaletteTab(mouseX + paletteImage->localPos.x - (tabSize.x / 2), paletteImage->localPos.y + barHeight,
		tabSize.x, tabSize.y);
	Add(tab);
	tab->position = paletteImage->drawBox.size.x / mouseX;
	//colourTabs.push_back({ tab, tab->position });
}

void CGUIpaletteBar::message(CGUIbase& sender, CMessage& msg) {
	if (msg.Msg == WM_RBUTTONUP) {
		delete &sender;
	}
	if (msg.Msg == MY_DOUBLECLICK) {
		changeTabColour((CGUIpaletteTab*) &sender);
	}
}

void CGUIpaletteBar::GUIcallback(CGUIbase* sender, CMessage& msg) {
	if (msg.Msg == uiClick) {
		if (sender->id = uiOKid)
			glm::i32vec4 pickedColour = colourPicker->getColour();
		colourPicker->setVisible(false);
		colourPicker->makeUnModal();
	}
}

void CGUIpaletteBar::changeTabColour(CGUIpaletteTab* sender) {
	colourChangeTab = sender;
	//display colour picker modally
	colourPicker->makeModal();
	colourPicker->setVisible(true);
}



void CGUIpaletteTab::onDoubleClick(const int mouseX, const int mouseY) {
	CMessage msg;
	msg.Msg = MY_DOUBLECLICK;
	parent->message(*this, msg);

}

void CGUIpaletteTab::onRMouseUp(const int mouseX, const int mouseY) {
	CMessage msg;
	msg.Msg = WM_RBUTTONUP;
	parent->message(*this, msg);
}


