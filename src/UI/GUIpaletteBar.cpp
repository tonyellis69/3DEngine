#include "GUIpaletteBar.h"

CGUIpaletteBar::CGUIpaletteBar(int x, int y, int w, int h) : CGUIpanel( x,  y,  w,  h) {
	upperPadding = 20;
	barHeight = 30;
	paletteImage = new CGUIimage(upperPadding, upperPadding, w - (2 * upperPadding), barHeight);
	Add(paletteImage);

}

void CGUIpaletteBar::DrawSelf() {

	pDrawFuncs->drawCtrlRect(*this);

	if (drawBorder) {
		pDrawFuncs->drawCtrlBorder(*this);
	}
}
