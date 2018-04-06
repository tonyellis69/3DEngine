#pragma once

#include "GUImenu.h"

/** An extension of the basic menu class, providing a solid  background and more. */
class CGUIpopMenu : public CGUImenu {
public:
	CGUIpopMenu(int x, int y, int w, int h);
	void DrawSelf();
	//void MouseMsg(unsigned int Msg, int mouseX, int mouseY, int key);
	void OnLMouseUp(const  int mouseX, const  int mouseY);

};
