#include "GUIimage.h"

using namespace glm;

CGUIimage::CGUIimage(int x, int y, int w, int h) {
	setPos(x, y);
	setWidth(w);
	setHeight(h);

	type = uiImage;

	texture = NULL;
}

void CGUIimage::DrawSelf() {
	if (texture) { //let's draw a texture!
		pDrawFuncs->drawTexture(drawBox,*texture);
	} else
		//pDrawFuncs->drawCtrlRect(*this);
		pDrawFuncs->drawRect2(drawBox, (vec4&)backColour1, (vec4&) backColour2);

	if (drawBorder) {
		//pDrawFuncs->drawCtrlBorder(*this);
		pDrawFuncs->drawBorder2(drawBox, (vec4&)borderColour);
	}

}

/** Set the texture we're using as a source for this image. */
void CGUIimage::setTexture(CBaseTexture& imageTexture) {
	texture = &imageTexture;
}


