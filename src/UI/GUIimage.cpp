#include "GUIimage.h"

using namespace glm;

CGUIimage::CGUIimage(int x, int y, int w, int h) {
	xPos = x; yPos = y; width = w; height = h;
	drawBox.pos = i32vec2(x, y); drawBox.size = i32vec2(w, h);

	type = uiImage;

	pDrawFuncs->registerControl(*this);
}

void CGUIimage::DrawSelf() {

	//just a boring rectangle to draw.
	pDrawFuncs->setDrawColours(backColour1, backColour2);
//	pDrawFuncs->drawRect(screenPos, width, height);
	if (drawBorder) {
		pDrawFuncs->setDrawColours(borderColour, borderColour);
		pDrawFuncs->drawBorder(screenPos.x, screenPos.y, width, height);
	}

	pDrawFuncs->drawCtrlRect(*this);

}
