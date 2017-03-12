#include "GUIpanel.h"


int CGUIpanel::Count = 1;

CGUIpanel::CGUIpanel(int x, int y, int w, int h) {
	xPos = x; yPos = y; width = w; height = h;
	borderColour = UIdarkGrey;
	GenName("Panel",Count++);
	type = panel;
	drawBorder = true;
}


void CGUIpanel::DrawSelf() {
	//just a boring rectangle to draw.
	pDrawFuncs->setDrawColours(backColour1, backColour2);
	pDrawFuncs->drawRect(screenPos,width,height);
	if (drawBorder) {
		pDrawFuncs->setDrawColours(borderColour, borderColour);
		pDrawFuncs->drawBorder(screenPos.x,screenPos.y,width,height);
	}

}
