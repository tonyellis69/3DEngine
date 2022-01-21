#include "GUIpanel.h"

#include "UI/uiRender.h"

CGUIpanel::CGUIpanel(int x, int y, int w, int h) : CguiBase(x,y,w,h){
	drawBorder = true;
	setBorderColour(UIdarkGrey);

	type = uiPanel;


	setBackColour1(oldbackColour1);
	setBackColour2(oldbackColour2);
}


void CGUIpanel::DrawSelf() {
	uiDraw::drawRect(drawBox, (glm::vec4&)backColour1, (glm::vec4&)backColour2); 

	if (drawBorder) {
		uiDraw::drawBorder(drawBox, borderColour);
	}

}
