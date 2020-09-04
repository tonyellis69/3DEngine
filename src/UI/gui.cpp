#include "gui.h"

#include "GUIroot.h"



CGUIlabel2* gui::addLabel(std::string text, int x, int y) {
	CGUIlabel2* label = new CGUIlabel2(x, y, style::labelPresets[0].x, style::labelPresets[0].y);
	label->setText(text);

	pGUIroot->Add(label);
	return label;
}

void gui::removeControl(CGUIbase* ctrl) {
	delete ctrl;
}
