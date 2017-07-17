#include "GUIimage.h"

using namespace glm;

CGUIimage::CGUIimage(int x, int y, int w, int h) {
	xPos = x; yPos = y; width = w; height = h;
	drawBox.pos = i32vec2(x, y); drawBox.size = i32vec2(w, h);

	type = uiImage;

	texture = NULL;

	pDrawFuncs->registerControl(*this);
}

void CGUIimage::DrawSelf() {
	if (texture) { //let's draw a texture!
		pDrawFuncs->drawTexture(*this,*texture);
	} else
		pDrawFuncs->drawCtrlRect(*this);
	if (drawBorder) {
		pDrawFuncs->drawCtrlBorder(*this);
	}

	

}

/** Set the texture we're using as a source for this image. */
void CGUIimage::setTexture(CBaseTexture& imageTexture) {
	texture = &imageTexture;
}
