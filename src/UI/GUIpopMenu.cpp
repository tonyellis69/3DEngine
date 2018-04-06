#include "GUIpopMenu.h"

CGUIpopMenu::CGUIpopMenu(int x, int y, int w, int h) : CGUImenu(x,y,w,h) {

}


void CGUIpopMenu::DrawSelf() {
	pDrawFuncs->drawCtrlRect(*this);

//	if (drawBorder) {
//		pDrawFuncs->drawCtrlBorder(*this);
//	}

}


void CGUIpopMenu::OnLMouseUp(const  int mouseX, const  int mouseY) {
	CMessage msg;
	msg.Msg = uiMsgLMouseUp;
	msg.value = selected;
	pDrawFuncs->handleUImsg(*this, msg);
}
