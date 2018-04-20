#pragma once

#include "GUIpanel.h"
#include "GUIrichText.h"

/** A composite control that adds a rich text control to a panel, which
	can then be modified, added-to, etc. */

class CGUIrichTextPanel : public CGUIpanel {
public:	
	CGUIrichTextPanel(int x, int y, int w, int h);
	void setFont(CFont* newFont);
	CFont* getFont();
	void setTextColour(float r, float g, float b, float a);
	void setTextColour(UIcolour colour);
	void setHotTextColour(float r, float g, float b, float a);
	void setHotTextHighlightColour(float r, float g, float b, float a);
	void setText(std::string newText);
	void appendText(std::string newText);
	void clear();

	void appendMarkedUpText(string text);
	unsigned int getRichTextID();

	CGUIrichText* richText; ///<Rich text control.
	int inset; ///<Inset for rich text control within panel.
};