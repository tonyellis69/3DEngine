#include "GUIimage.h"

using namespace glm;

CGUIimage::CGUIimage(int x, int y, int w, int h) {
	xPos = x; yPos = y; width = w; height = h;
	drawBox.pos = i32vec2(x, y); drawBox.size = i32vec2(w, h);

	type = uiImage;

	textureHandle = NULL; 

	pDrawFuncs->registerControl(*this);
}

void CGUIimage::DrawSelf() {
	pDrawFuncs->drawCtrlRect(*this);
	if (drawBorder) {
		pDrawFuncs->drawCtrlBorder(*this);
	}

	

}

/** Get the texture handle for this texture. */
void CGUIimage::setTexture(std::string & textureName) {
	textureHandle = pDrawFuncs->getTextureHandle(textureName);
}
