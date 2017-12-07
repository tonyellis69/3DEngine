#include "GUIpanel.h"

CGUIpanel::CGUIpanel(int x, int y, int w, int h) {
	localPos = glm::i32vec2(x, y); 
	//width = w; height = h;

	drawBorder = true;

	drawBox.pos = glm::i32vec2(x, y); 
	drawBox.size = glm::i32vec2(w, h);

	type = uiPanel;

	pDrawFuncs->registerControl(*this);

	setBackColour1(oldbackColour1);
	setBackColour2(oldbackColour2);
}


void CGUIpanel::DrawSelf() {
	pDrawFuncs->drawCtrlRect(*this);

	setBorderColour(UIdarkGrey);
	pDrawFuncs->drawCtrlBorder(*this);
}
