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

void CGUIrichTextPanel::setHotTextColour(float r, float g, float b, float a) {
	richText->setHotTextColour(r, g, b, a);
}

void CGUIrichTextPanel::setHotTextHighlightColour(float r, float g, float b, float a) {
	richText->setHotTextHighlightColour(r, g, b, a);
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

void CGUIrichTextPanel::appendMarkedUpText(string text) {
	richText->appendMarkedUpText(text);
}

unsigned int CGUIrichTextPanel::getRichTextID() {
	return richText->getID();
}
