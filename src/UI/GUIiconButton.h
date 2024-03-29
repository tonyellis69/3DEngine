#pragma once
#include "GUIbase.h"
#include "GUIicons.h"

/** A class for creating buttons that display an icon, not text. */
class CGUIsysIconButton : public CGUIbase {
public:
	CGUIsysIconButton(int x, int y);
	CGUIsysIconButton(int x, int y, int w, int h);
	void DrawSelf();

	int icon; ///<References the icon for this button.
	int iconset; ///<Identifies what tileset icons will be taken from.

};


class CGUIiconButton : public CGUIsysIconButton {
public:
	CGUIiconButton(int x, int y);
	CGUIiconButton(int x, int y, int w, int h) : CGUIsysIconButton(x, y, w, h) {};
	bool OnClick(const  int mouseX, const  int mouseY);


	static int Count;
};