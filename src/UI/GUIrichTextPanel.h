#pragma once

#include "GUIpanel.h"
#include "GUIrichText.h"

/** A composite control that adds a rich text control to a panel, which
	can then be modified, added-to, etc. */

class CGUIrichTextPanel : public CGUIpanel {
public:	
	CGUIrichTextPanel(int x, int y, int w, int h);
	void setInset(int newInset);
	void setFont(CFont* newFont);
	CFont* getFont();
	void setTextColour(float r, float g, float b, float a);
	void setTextColour(UIcolour colour);
	void setHotTextColour(const glm::vec4& colour);
	void setHotTextHighlightColour(const glm::vec4& colour);
	void setText(std::string newText);
	void appendText(std::string newText);
	void clear();
	void setResizeMode(TResizeMode mode);
	void setTextStyle(TtextStyle & newStyle);
	void addStyle(TtextStyle & newStyle);
	void purgeHotText(int msgId, int objId);
	void update(float dT);

	void appendMarkedUpText(string text);
	unsigned int getRichTextID();

	void updateAppearance();

	bool MouseWheelMsg(const int mouseX, const int mouseY, int wheelDelta, int key);

	void OnLMouseDown(const int mouseX, const int mouseY, int key);

	void resizeToFit();

	void message(CGUIbase & sender, CMessage & msg);

	CGUIrichText* richText; ///<Rich text control.
	int inset; ///<Inset for rich text control within panel.
};