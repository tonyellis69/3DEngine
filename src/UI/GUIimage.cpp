#include "GUIimage.h"

#include "UI/uiRender.h"

using namespace glm;

CGUIimage::CGUIimage(int x, int y, int w, int h) {
	setLocalPos(x, y);
	setWidth(w);
	setHeight(h);

	type = uiImage;

	texture = NULL;
}

void CGUIimage::DrawSelf() {
	if (texture) { //let's draw a texture!
		uiDraw::drawTexture(drawBox,(CRenderTexture&)texture);
	} else
		//pDrawFuncs->drawCtrlRect(*this);
		uiDraw::drawRect(drawBox, (vec4&)backColour1, (vec4&) backColour2);

	if (drawBorder) {
		//pDrawFuncs->drawCtrlBorder(*this);
		uiDraw::drawBorder(drawBox, (vec4&)borderColour);
	}

}

/** Set the texture we're using as a source for this image. */
void CGUIimage::setTexture(CBaseTexture& imageTexture) {
	texture = &imageTexture;
}


