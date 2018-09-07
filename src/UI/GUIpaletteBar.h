#pragma once


#include "GUIpanel.h"
#include "GUIimage.h"

class CGUIpaletteBar : public CGUIpanel{
public:
	CGUIpaletteBar(int x, int y, int w, int h);
	void DrawSelf();


	CGUIimage* paletteImage;
	int upperPadding;
	int barHeight;
};