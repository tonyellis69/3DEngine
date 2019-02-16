

#include "GUIimage1D.h"

using namespace glm;


CGUIimage1D::CGUIimage1D(int x, int y, int w, int h) : CGUIimage(x, y, w, h) {

}

/** Set the texture we're using as a source for this image. */
void CGUIimage1D::setTexture(CBaseTexture& imageTexture) {
	texture = &imageTexture;
}

void CGUIimage1D::setTextureData(void * pixels) {
	texture->setPixelData(pixels);
}



void CGUIimage1D::DrawSelf() {
	if (texture) { //let's draw a texture!
		pDrawFuncs->drawTextureGradient(drawBox, *texture);
	}
	else
		pDrawFuncs->drawRect2(drawBox, (vec4&)backColour1, (vec4&)backColour2);

	if (drawBorder) {
		pDrawFuncs->drawBorder2(drawBox, (vec4&)borderColour);
	}
}


