#include "GUIpopMenu.h"

CGUIpopMenu::CGUIpopMenu(int x, int y, int w, int h) : CGUImenu(x,y,w,h) {
	setBorderOn(true);
}

void CGUIpopMenu::setMenuCallback(IPopupMenu * callback) {
	pMenuCallback = callback;
}


bool CGUIpopMenu::OnClick(const int mouseX, const int mouseY) {
	pMenuCallback->popupMenuCallback(focusItem);
	CGUIpopMenu::~CGUIpopMenu();
	return true;
}

