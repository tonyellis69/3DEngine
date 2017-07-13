#include "GUIbetterBase.h"

unsigned int UIuniqueIDgen = 0;

CGUIbetterBase::CGUIbetterBase() {
	backColour1 =  UIlightGrey;
	backColour2 = UIdarkGrey;
	borderColour = UIdarkGrey;
	drawBorder = true;
	uniqueID = UIuniqueIDgen++;
}
