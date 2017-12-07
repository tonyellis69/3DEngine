#pragma once
#include "..\renderer\texManager.h"
#include "GUIbase.h"

/** Control for displaying images. Textures and maybe others. */
class CGUIimage : public CGUIbase {
public:
	CGUIimage(int x, int y, int w, int h);
	void DrawSelf();
	void setTexture(CBaseTexture& imageTexture);
	

	CBaseTexture* texture;


};