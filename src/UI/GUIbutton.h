#pragma once

#include "GUIlabel2.h"
#include "GUIlabel.h"
#include "GUIbase.h"

class CGUIbutton : public CGUIbase {
public:
	CGUIbutton(const std::string & text);
	CGUIbutton(const std::string& text, unsigned int styleWord);
	void resizeToFit();
	//CGUIbutton(int x, int y, int w, int h);
	void DrawSelf();
	void OnClick(const int mouseX, const int mouseY);
	void setText(const std::string& text);
	void resize(int w, int h);

	CGUIlabel* label;
	bool fitToPresets; ///<If true, use preset sizes
};