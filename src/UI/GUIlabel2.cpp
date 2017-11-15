#include "GUIlabel2.h"

using namespace glm;

CGUIlabel2::CGUIlabel2(int x, int y, int w, int h) {
	xPos = x; yPos = y; width = w; height = h;
	drawBox.pos = i32vec2(x, y); drawBox.size = i32vec2(w, h);
	textBuf.setFont(defaultFont);
	textBuf.setSize(w, h);

	type = uiLabel;
	setTextColour(0, 0, 0, 1);

	drawBorder = false;
	mousePassthru = true;
	pDrawFuncs->registerControl(*this);
}

void CGUIlabel2::setFont(CFont& font) {
	textBuf.setFont(font);
	textBuf.renderText();
}

void CGUIlabel2::setText(std::string newText) {
	textBuf.setText(newText);
}

void CGUIlabel2::setTextColour(float r, float g, float b, float a) {
	textColour.r = r; textColour.g = g; textColour.b = b; textColour.a = a;
	textBuf.setTextColour(textColour);
	textBuf.renderText();
}

void CGUIlabel2::setTextColour(UIcolour  colour) {
	setTextColour(colour.r, colour.g, colour.b, colour.a);
}

void CGUIlabel2::setMultiLine(bool onOff) {
	textBuf.setMultiLine(onOff);
}

void CGUIlabel2::setHorizontalAlignment(TTextAlign align) {
	textBuf.setHorizontalAlignment(align);
	textBuf.renderText();
}

TTextAlign CGUIlabel2::getJustification() {
	return textBuf.getJustification();
}


void CGUIlabel2::DrawSelf() {
	pDrawFuncs->drawTexture(*this, textBuf.textTexture);


	if (drawBorder) {
		pDrawFuncs->drawCtrlBorder(*this);
	}


}

void CGUIlabel2::borderOn(bool onOff) {
	drawBorder = onOff;
}

/** Set the dimensions and relative position of the control. */
void CGUIlabel2::SetPos(int x, int y, int w, int h) {
	xPos = x; yPos = y; width = w; height = h;
	drawBox.pos = glm::i32vec2(x, y);
	drawBox.size = glm::i32vec2(w, h);
	textBuf.setSize(w, h);
	//textBuf.renderText();
	updateAppearance();
}
