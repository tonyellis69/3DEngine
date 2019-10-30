

#include "GUIbutton.h"

using namespace glm;

CGUIbutton::CGUIbutton(const std::string & text) {
	type = uiButton;

	label = add<CGUIlabel>(text, uiHcentred | uiVcentred);
	applyStyleSheet();

	label->setBorderOn(false);
	label->setTextAlignment(tcentred);

	fitToPresets = true;
	resizeToFit();
}

CGUIbutton::CGUIbutton(const std::string& text, unsigned int styleWord) :
	CGUIbutton(text) {
	setPositionStyles(styleWord);
}

void CGUIbutton::applyStyleSheet() {
	setBackColour1(styleSheet->defaultButtonBackColour1);
	setBackColour2(styleSheet->defaultButtonBackColour2);
	setBorderColour(styleSheet->defaultBorderColour);
	setBorderColourFocusColour(styleSheet->defaultBorderFocusColour);

	label->applyStyleSheet();
}

/*
CGUIbutton::CGUIbutton(int x, int y, int w, int h) : CGUIbase(x,y,w,h) {
	type = uiButton;

	label = new CGUIlabel2(5, 5, w - 10, h - 10);
	label->setText("Button");
	label->setHorizontalAlignment(tcentred);
	Add(label);
	setBackColour1(oldbackColour1);
	setBackColour2(oldbackColour2);
}*/

/** Resize this control to fit label text. */
void CGUIbutton::resizeToFit() {
	//base size on label:
	i32vec2 newSize;
	newSize.x = label->getWidth() + 2 * styleSheet->buttonLabelSurround;
	newSize.y = label->getHeight() + 2 * styleSheet->buttonLabelSurround;

	if (fitToPresets) {
		int currentWidth = newSize.x;
		for (auto preset : styleSheet->buttonPresets) {
			newSize = preset;
			if (preset.x > currentWidth) {
				break;
			}
		}
	}

	resize(newSize.x, newSize.y);

	//TO DO
	//And now somehow the label must be positioned in the centre
	//probably in the positionHint(control) method.
}

void CGUIbutton::DrawSelf() {
	if (MouseDown == this) {
		pDrawFuncs->drawRect2(drawBox, (glm::vec4&)backColour2, (glm::vec4&)backColour1);
	}
	else {
		pDrawFuncs->drawRect2(drawBox, (glm::vec4&)backColour1, (glm::vec4&)backColour2);
	}
	
	if (MouseOver == this)
		pDrawFuncs->drawBorder2(drawBox, borderFocusColour);
	else
		pDrawFuncs->drawBorder2(drawBox, (glm::vec4&)borderColour);
}


void CGUIbutton::OnClick(const int mouseX, const int mouseY) {
	CMessage msg;
	msg.Msg = uiClick;
	parent->message(this, msg);
	callbackObj->GUIcallback(this, msg);
}

void CGUIbutton::setText(const std::string & text) {
	label->setText(text);
}

void CGUIbutton::resize(int w, int h) {
	CGUIbase::resize(w, h);
	position(label);
}
