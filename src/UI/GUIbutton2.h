#pragma once

#include "GUIlabel2.h"
#include "GUIlabel.h"
#include "GUIbetterBase.h"

class CGUIbutton2 : public CGUIbetterBase {
public:
	CGUIbutton2(int x, int y, int w, int h);
	void DrawSelf();
	void OnClick(const int mouseX, const int mouseY);
	void setText(const std::string& text);

	CGUIlabel2* label;

};