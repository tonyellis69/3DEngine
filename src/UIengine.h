#pragma once
#include "Engine.h"
#include "UI\GUIbase.h"

const float disabledAlpha = 0.2f;
class CGUIengine  {
public:
	void setFont(int font);
	void setDrawColours(const UIcolour& colour1,const UIcolour& colour2 );
	int getTextWidth(char* text) ;
	void drawIcon(int icon, int x, int y);
	void drawRect(UIcoord pos, int width, int height);
	void DrawBorder(int x, int y, int width, int height);
	void setClip(UIrect& clipbox);
	void drawText(int x, int y, int w,char* Text);
	void drawTextMultiLine(int x, int y, int w,char* Text);
	void drawLine(int x, int y, int x2, int y2);
	void setIconset(int iconset);
	void drawTile(int tileNo, UIrect& tileRect);
	void drawImage(int imageNo, UIrect& tileRect);
	void setCursor(int iCursorPos);
	void setDrawColoursConditional( UIcolour backColour1, UIcolour backColour2,bool enabled );
	void drawDottedRect(int x, int y, int width, int height);
	void setScale(float x, float y);
	CEngine* pEngine;

	
};