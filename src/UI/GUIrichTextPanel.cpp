#include "GUIrichTextPanel.h"

CGUIrichTextPanel::CGUIrichTextPanel(int x, int y, int w, int h) {
	localPos = glm::i32vec2(x, y);
	drawBorder = false;
	drawBox.pos = glm::i32vec2(x, y);
	drawBox.size = glm::i32vec2(w, h);
	type = uiRichTextPanel;
	pDrawFuncs->registerControl(*this);

	setBackColour1(oldbackColour1);
	setBackColour2(oldbackColour2);

	inset = 10;
	richText = new CGUIrichText(inset, inset, w - inset, h - inset);
	richText->setMultiLine(true);
	Add(richText);

}

void CGUIrichTextPanel::setInset(int newInset) {
	inset = newInset;
	updateAppearance();
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

void CGUIrichTextPanel::setHotTextColour(const glm::vec4& colour) {
	richText->setHotTextColour(colour);
}

void CGUIrichTextPanel::setHotTextHighlightColour(const glm::vec4& colour) {
	richText->setHotTextHighlightColour(colour);
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

void CGUIrichTextPanel::setTextStyle(TtextStyle & newStyle) {
	richText->setTextStyle(newStyle);
}

void CGUIrichTextPanel::addStyle(TtextStyle & newStyle) {
	richText->styles.push_back(newStyle);
}

void CGUIrichTextPanel::purgeHotText(int msgId, int objId){
	richText->purgeHotText(msgId, objId);
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

/** Hacking this because I want the panel to keep its rich text control central.*/
//TO DO: should be able to do this automatically with anchorRight and anchorBottom - investigate!
//Doesn't appear to do anything! Is that because I always spawn a new richtext now rather than resize the old one?
void CGUIrichTextPanel::updateAppearance() {
	CGUIbase::updateAppearance();
	richText->setPos(inset, inset);
	richText->resize(drawBox.size.x - (2*inset), drawBox.size.y - (2*inset));
	richText->updateAppearance();
}


bool CGUIrichTextPanel::MouseWheelMsg(const  int mouseX, const  int mouseY, int wheelDelta, int key) {
	return richText->MouseWheelMsg(mouseX, mouseY, wheelDelta, key);
}

void CGUIrichTextPanel::OnLMouseDown(const int mouseX, const int mouseY, int key) {
	if (!IsOnControl((CGUIbase&)*this,mouseX, mouseY)) {
		CMessage msg;
		msg.Msg = uiMsgHotTextClick;
		msg.x = mouseX; msg.y = mouseY;
		msg.value = -1;
		pDrawFuncs->handleUImsg(*this->richText, msg);

	}
}

void CGUIrichTextPanel::resizeToFit() {
	richText->resizeToFit();
}

void CGUIrichTextPanel::message(CGUIbase& sender, CMessage& msg) {
	if (msg.Msg == uiMsgChildResize) {
		drawBox.size.x = richText->drawBox.size.x + (2 * inset);
		drawBox.size.y = richText->drawBox.size.y + (2 * inset);
		updateAppearance();
	}

}




