#pragma once

#include "GUIbase.h"
#include "GUIlabel2.h"

/** A graphical control for presenting a simple menu. */
class CGUImenu : public CGUIbase {
public:
	CGUImenu(int x, int y, int w, int h);
	void setFont(CFont* newFont);
	void setTextColour(UIcolour colour);
	void setSelectedColour(UIcolour colour);
	void DrawSelf();
	void addItem(std::string itemText);
	void resizeToFit();
	bool MouseWheelMsg(const  int mouseX, const  int mouseY, int wheelDelta, int key);
	void OnMouseMove(const int mouseX, const int mouseY, int key);
	void OnLMouseDown(const  int mouseX, const  int mouseY, int key);
	void OnClick(const int mouseX, const int mouseY);
	void clear();
	
	std::vector<CGUIlabel2*> items;
	int nItems;
	CFont* itemFont;
	int itemWidth;
	int maxItemWidth;
	int itemHeight;
	int nextItemPos;
	int vPad;
	UIcolour textColour;
	UIcolour selectedColour;
	int selected;
};
