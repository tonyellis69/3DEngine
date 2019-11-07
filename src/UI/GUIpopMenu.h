#pragma once

#include "GUImenu.h"


class IPopupMenu {
public:
	virtual void popupMenuCallback(int choice) {}

};


/** An extension of the basic menu class useful for context menus etc. */
class CGUIpopMenu : public CGUImenu {
public:
	CGUIpopMenu(int x, int y, int w, int h);
	void setMenuCallback(IPopupMenu* callback);
	bool OnClick(const  int mouseX, const  int mouseY);


	IPopupMenu* pMenuCallback;
};
