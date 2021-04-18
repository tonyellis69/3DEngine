#include "gui.h"

#include "GUIroot.h"




CGUIlabel2* gui::addLabel(const std::string& text, int x, int y) {
	CGUIlabel2* label = new CGUIlabel2(x, y, style::labelPresets[0].x, style::labelPresets[0].y);
	label->setText(text);
	pGUIroot->add(label);
	return label;
}

CGUIpanel* gui::addPanel(int x, int y, int w, int h) {
	CGUIpanel* panel = new CGUIpanel(x, y, w, h);
	pGUIroot->add(panel);
	return panel;
}

CGUIbutton2* gui::addButton(const std::string& text, int x, int y) {
	CGUIbutton2* button = new CGUIbutton2(x, y, style::buttonPresets[1].x, style::buttonPresets[1].y);
	button->setText(text);
	pGUIroot->add(button);
	return button;
}

CGUIswatchContainer* gui::addSwatchContainer(int x, int y) {
	CGUIswatchContainer* swatch = new CGUIswatchContainer(x, y, 320, 300);
	pGUIroot->add(swatch);
	return swatch;
}

CGUIpaletteBar* gui::addPaletteBar(int x, int y) {
	CGUIpaletteBar* palette = new CGUIpaletteBar(x, y, 320, 300);
	pGUIroot->add(palette);
	return palette;
}



void gui::removeControl(CGUIbase* ctrl) {
	delete ctrl;
}
