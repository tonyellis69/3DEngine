#include "GUIbetterBase.h"

unsigned int UIuniqueIDgen = 0;

CGUIbetterBase::CGUIbetterBase() {
	backColour1 =  UIlightGrey;
	backColour2 = UIdarkGrey;
	borderColour = UIdarkGrey;
	drawBorder = true;
	uniqueID = UIuniqueIDgen++;
	visible = true;
}

void CGUIbetterBase::setBorderColour(UIcolour colour) {
	borderColour = colour;
}

/** Recursively update this control's position, dimensions and clipping, and those of its children. */
void CGUIbetterBase::updateAppearance() {

	needsUpdate = false;
	//1. Recalculate x,y,w,h if necessary due to justification or spanning
	if (anchorRight != NONE)
		width = parent->width - xPos - anchorRight;
	else {
		switch (hFormat) {
		case hCentre: {
			xPos = (parent->width - width) / 2; break; }
		case hRight: {
			xPos = parent->width - (width + parent->borderWidth); break; }
		case hLeft: {
			xPos = parent->borderWidth; break; }
		case hSpan: {
			xPos = 0; width = parent->width; break; }
		}
	}

	if (anchorBottom != NONE)
		height = parent->height - yPos - anchorBottom;
	else {
		switch (vFormat) {
		case vCentre: {
			yPos = (parent->height - height) / 2; break; }
		case vTop: {
			yPos = parent->borderWidth; break; }
		case vBottom: {
			yPos = parent->height - height - parent->borderWidth; break; }
		case vSpan: {
			yPos = 0; height = parent->height; break; }
		}
	}



	//2. Recalculate clipping
	CalculateClipbox();
	//3. Recalculate screen position
	screenPos.x = parent->screenPos.x + xPos; screenPos.y = parent->screenPos.y + yPos;
	//TO DO: above is legacy, to be replaced with:
	drawBox.pos = glm::i32vec2(parent->screenPos.x, parent->screenPos.y) + glm::i32vec2(xPos, yPos);
	//...but screenPos becomes drawBox
	pDrawFuncs->updateScreenDimensions(*this);

	//repeat through children
	for (size_t i = 0; i < Control.size(); i++)
		Control[i]->updateAppearance();
}