#include "UIengine.h"



void CGUIengine::setDrawColours(const UIcolour& colour1,const UIcolour& colour2 ) {
	pEngine->setDrawColour((rgba&)colour1);
	pEngine->setSecondaryDrawColour((rgba&)colour2);
}

void CGUIengine::drawIcon(int icon, int x, int y) {
	pEngine->drawTile(icon,(float)x,pEngine->flipY((float)y));
}


void CGUIengine::drawRect(UIcoord pos, int width, int height) {
	pEngine->enableLineMode(true);
	pos.y = (int)pEngine->flipY((float)pos.y);
	pEngine->drawFilledRect((float)pos.x,(float)(pos.y-height),(float)width,(float)height);
	pEngine->enableLineMode(false);
}

void CGUIengine::DrawBorder(int x, int y, int width, int height) {
	/*We tweak the coords because the rounding error puts some of the rect's lines
	outside of the control's clipping area. Mathematically a vertical line at 0,0 is right
	*on* the zero knife-edge - half its width is actually in negative land, the other half 
	in the pixel-column we expect. The trouble is, the clipping area is in absolute integer
	pixels - a line that's only halfway in is out.
	TO DO: look for a more elegant solution? But if this is the only problem area, might not
	be worth it.*/

	float fx = x + 0.5f; 
	float fy = pEngine->flipY((float)y) - 0.5f;
	width -= 1;
	height -= 1;
	pEngine->enableLineMode(true);
	pEngine->drawOutlineRect(fx,fy,(float)width,(float)height);
	pEngine->enableLineMode(false);
}

void CGUIengine::setClip(UIrect& clipbox) {
		pEngine->setClip(clipbox.x, clipbox.y, clipbox.width, clipbox.height);
}


void CGUIengine::drawLine(int x, int y, int x2, int y2) {
	pEngine->enableLineMode(true);
	y = (int) pEngine->flipY((float)y);
	y2 = (int)pEngine->flipY((float)y2);
	pEngine->drawLine((float)x,(float)y,(float)x2,(float)y2);
	pEngine->enableLineMode(false);
}

void CGUIengine::setIconset(int iconset) {
	pEngine->setCurrentTileSheet(iconset);
}

void CGUIengine::drawTile(int tileNo, UIrect& tileRect) {
	float UIflip = (float)tileRect.width/2;
	float y = pEngine->flipY((float)tileRect.y);
	pEngine->drawTile(tileNo,(float)tileRect.x +UIflip ,  y - UIflip );	
}


void CGUIengine::drawImage(int imageNo, UIrect& tileRect) {
	pEngine->setCurrentTexture(imageNo);
	TRect image;
	image.originX = tileRect.width/2.0f;
	image.originY = tileRect.height/2.0f;
	image.width = (float)tileRect.width;
	image.height = (float)tileRect.height;

	pEngine->drawImage(&image,tileRect.x + image.originX,pEngine->flipY((float)tileRect.y) - image.originY);
}


void CGUIengine::setCursor(int iCursorPos) {
	pEngine->tmpCursorPos = iCursorPos;
}

/** Set the UI drawing colours when a control may be disabled. */
void CGUIengine::setDrawColoursConditional( UIcolour colour1, UIcolour colour2,bool enabled ) {
	if (!enabled) {
		colour1.a = disabledAlpha; colour2.a = disabledAlpha;
	}
	pEngine->setDrawColour((rgba&)colour1);
	pEngine->setSecondaryDrawColour((rgba&)colour2);
}

void CGUIengine::drawDottedRect(int x, int y, int width, int height) {
	pEngine->enableDottedLineMode(true);
	DrawBorder(x,y,width,height);
	pEngine->enableDottedLineMode(false);
}

void CGUIengine::setScale(float x, float y) {
	pEngine->setDrawScale(x,y);
}

