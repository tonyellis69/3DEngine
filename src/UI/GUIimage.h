#pragma once

#include "GUIbetterBase.h"

/** Control for displaying images. Textures and maybe others. */
class CGUIimage : public CGUIbetterBase {
public:
	CGUIimage(int x, int y, int w, int h);
	void DrawSelf();
	void setTexture(std::string& textureName);

	unsigned int textureHandle;
};