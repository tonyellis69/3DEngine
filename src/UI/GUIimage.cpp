#include "GUIimage.h"

using namespace glm;

CGUIimage::CGUIimage(int x, int y, int w, int h) {
	localPos = glm::i32vec2(x, y);
	width = w; height = h;
	drawBox.pos = i32vec2(x, y); drawBox.size = i32vec2(w, h);

	type = uiImage;

	texture = NULL;

	//pDrawFuncs->registerControl(*this);
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


