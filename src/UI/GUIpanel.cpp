#include "GUIpanel.h"

CGUIpanel::CGUIpanel(int x, int y, int w, int h) : CGUIbase(x,y,w,h){

	//setLocalPos(x, y);
	drawBorder = true;
	setBorderColour(UIdarkGrey);

	//setWidth(w);
	//setHeight(h);

	type = uiPanel;
	//pDrawFuncs->registerControl(*this);

	setBackColour1(oldbackColour1);
	setBackColour2(oldbackColour2);
}


void CGUIpanel::DrawSelf() {
	//pDrawFuncs->drawCtrlRect(*this);
	pDrawFuncs->drawRect2(drawBox, (glm::vec4&)backColour1, (glm::vec4&) backColour2);


	if (drawBorder) {
		//pDrawFuncs->drawCtrlBorder(*this);
		pDrawFuncs->drawBorder2(drawBox, (glm::vec4&)borderColour);
	}

}
