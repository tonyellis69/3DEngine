#pragma once

#include "GUIbase.h"
#include "GUIpanel.h"
#include "GUIlabel2.h"

#include <initializer_list>

enum TMenuHighlight { menuHighlightBar, menuHighlightBox, menuHighlightText };

/** A graphical control for presenting a simple menu. */
class CGUImenu : public CGUIpanel {
public:
	CGUImenu(int x, int y, int w, int h);
	void setFont(CFont* newFont);
	void setTextColour(UIcolour colour);
	void setSelectedColour(glm::vec4 colour);
	void DrawSelf();
	void addItem( std::initializer_list<std::string> itemTexts);
	void resizeToFit();
	void onMouseOff(const int mouseX, const int mouseY, int key);
	bool MouseWheelMsg(const  int mouseX, const  int mouseY, int wheelDelta, int key);
	void OnMouseMove(const int mouseX, const int mouseY, int key);
	void OnLMouseDown(const  int mouseX, const  int mouseY, int key);
	void OnClick(const int mouseX, const int mouseY);
	void clear();
	std::string& getItemName(int itemNo);
	
	std::vector<CGUIlabel2*> items;
	int nItems;
	CFont* itemFont;
	int itemWidth;
	int maxTextWidth;
	int itemHeight;
	int nextItemPos;
	int vItemPad; ///<Vertical space between items.
	int hItemPad; ///<Horizontal  space between items and menu edge.
	int hItemInteriorPadding; ///<Horizontal space between item text and label boundry.
	UIcolour textColour;
	glm::vec4  selectedColour;
	int selected;
	TTextAlign itemTextAlignment;
	int leftAlignIndent;
	TMenuHighlight highlightStyle;
	unsigned int minItemsShown; ///<Adjusts height on resizeToFit.
	unsigned int maxItemsShown; ///<Adjusts height on resizeToFit.
	bool resizeHorizontal;
};
