#pragma once

#include "GUIlabel2.h"
#include "GUImenu.h"

/** Class for a basic drop down menu, created out of existing controls.*/
class CGUIdropdownMenu : public CGUIbase {
public:
	CGUIdropdownMenu(int x, int y, int w, int h);
	void setText(const std::string& title);
	void DrawSelf();
	bool OnClick(const  int mouseX, const  int mouseY);
	void GUIcallback(CGUIbase * sender, CMessage & msg);
	void clear();
	std::string getItemName(int itemNo);

	void addItem(std::initializer_list<std::string> itemTexts);





	CGUIlabel2* menuButton; ///<What you click on to drop menu.
	CGUImenu* menu; ///<The actual menu.
};
