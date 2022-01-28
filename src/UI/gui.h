#pragma once

#include "GUIlabel2.h"
#include "GUIpanel.h"
#include "GUIbutton2.h"
#include "GUIswatchContainer.h"
#include "GUIpaletteBar.h"

/** A header file for using the GUI controls. */

namespace gui {

	CGUIlabel2* addLabel(const std::string& text, int x, int y);
	CGUIpanel* addPanel(int x, int y, int w, int h);
	CGUIbutton2* addButton(const std::string& text, int x, int y);
	CGUIswatchContainer* addSwatchContainer(int x, int y);
	CGUIpaletteBar* addPaletteBar(int x, int y);


	void removeControl(CGUIbase* ctrl);




}