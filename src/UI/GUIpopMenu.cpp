#include "GUIpopMenu.h"

CGUIpopMenu::CGUIpopMenu(int x, int y, int w, int h) : CGUImenu(x,y,w,h) {
	lastY = 0;
}


void CGUIpopMenu::DrawSelf() {
	pDrawFuncs->drawCtrlRect(*this);

//	if (drawBorder) {
//		pDrawFuncs->drawCtrlBorder(*this);
//	}

}

/** Left mouse released, so a selection has been made. */
void CGUIpopMenu::OnLMouseUp(const  int mouseX, const  int mouseY) {
	CMessage msg;
	msg.Msg = uiMsgLMouseUp;
	msg.value = selected;
	pDrawFuncs->handleUImsg(*this, msg);
}

/** Use mouse movement to change selection. */
void CGUIpopMenu::OnMouseMove(const  int mouseX, const  int mouseY, int key) {
	if (lastY == 0) {
		lastY = mouseY;
		return;
	}
	int yDiff = mouseY - lastY;
	lastY = mouseY;
	yDisplacement += yDiff;
	if (abs(yDisplacement) > itemHeight) {
		items[selected]->setTextColour(textColour);
		selected += yDisplacement / abs(yDisplacement);
		if (selected >= nItems)
				selected = nItems-1;
		if (selected < 0)
			selected = 0;
		items[selected]->setTextColour(selectedColour);
		yDisplacement = 0;
	}
};

void CGUIpopMenu::setVisible(bool onOff) {
	visible = onOff;
	if (visible) {
		lastY = 0;
		yDisplacement = 0;
	}
}

