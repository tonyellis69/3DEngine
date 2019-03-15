#include "GUIrichTextPanel.h"

CGUIrichTextPanel::CGUIrichTextPanel(int x, int y, int w, int h) {
	setLocalPos(x, y);
	drawBorder = false;
	setWidth(w);
	setHeight(h);
	type = uiRichTextPanel;

	setBackColour1(oldbackColour1);
	setBackColour2(oldbackColour2);

	inset = 10;
	richText = new CGUIrichText(inset, inset, w - inset*2, h - inset);
	richText->setMultiLine(true);
	richText->anchorRight = inset;
	richText->anchorLeft = inset;
	richText->anchorBottom = inset;
	Add(richText);

}

void CGUIrichTextPanel::setInset(int newInset) {
	inset = newInset;
	richText->anchorRight = inset;
	richText->anchorLeft = inset;
	richText->anchorBottom = inset;
	needsUpdate = true;
}

void CGUIrichTextPanel::setFont(CFont * newFont) {
	richText->setFont(newFont);
}

CFont * CGUIrichTextPanel::getFont() {
	return richText->getFont();
}

void CGUIrichTextPanel::setTextColour(float r, float g, float b, float a) {
	richText->setTextColour(r, g, b, a);
}

void CGUIrichTextPanel::setTextColour(UIcolour colour) {
	richText->setTextColour(colour);
}


void CGUIrichTextPanel::setText(std::string newText) {
	richText->setText(newText);
}

void CGUIrichTextPanel::appendText(std::string newText) {
	richText->appendText(newText);
}

void CGUIrichTextPanel::clear() {
	richText->clear();
}



void CGUIrichTextPanel::setResizeMode(TResizeMode mode){
	richText->setResizeMode(mode);
}



void CGUIrichTextPanel::setTextStyles(std::vector<TtextStyle>* styles) {
	richText->setTextStyles(styles);
}

void CGUIrichTextPanel::setTextStyle(std::string styleName) {
	richText->setTextStyle(styleName);
}

std::vector<unsigned int> CGUIrichTextPanel::purgeHotText(unsigned int id){
	return richText->purgeHotText(id);
}

void CGUIrichTextPanel::update(float dT) {
	richText->update(dT);
}

void CGUIrichTextPanel::appendMarkedUpText(string text) {
	richText->appendMarkedUpText(text);
}

unsigned int CGUIrichTextPanel::getRichTextID() {
	return richText->getID();
}



bool CGUIrichTextPanel::MouseWheelMsg(const  int mouseX, const  int mouseY, int wheelDelta, int key) {
	return richText->MouseWheelMsg(mouseX, mouseY, wheelDelta, key);
}

void CGUIrichTextPanel::OnLMouseDown(const int mouseX, const int mouseY, int key) {
	if (!IsOnControl((CGUIbase&)*this,mouseX, mouseY)) {
		CMessage msg;
		msg.Msg = uiClickOutside;
		msg.x = mouseX; msg.y = mouseY;
		pDrawFuncs->handleUImsg(*this->richText, msg);

	}
}

void CGUIrichTextPanel::resizeToFit() {
	richText->resizeToFit();
}

void CGUIrichTextPanel::message(CGUIbase* sender, CMessage& msg) {
	if (msg.Msg == uiMsgChildResize) {
		setWidth(richText->getWidth() + (2 * inset));
		setHeight(richText->getHeight() + (2 * inset));
	}
}




