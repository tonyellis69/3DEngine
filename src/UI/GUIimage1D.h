#pragma once

#include "GUIbase.h"
#include "GUIimage.h"


/** controls for displaying images. Textures and maybe others. */
class CGUIimage1D : public CGUIimage {
public:
	CGUIimage1D(int x, int y, int w, int h);
	void DrawSelf();
	void setTexture(CBaseTexture& imageTexture);
	void setTextureData(void* pixels);

	CBaseTexture* texture;


};