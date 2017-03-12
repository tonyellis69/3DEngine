#pragma once
#include "GUIbase.h"
#include "GUIlabel.h"



/** Simple class for grouping controls. */

class CGUIgroup :  public CGUIbase {
public:
//	CGUIgroup(void) {};
	CGUIgroup(int x, int y, int w, int h);
	void SetText(const std::string& TextStr);
	void setHeaderHeight(int h);
	void setDrawn(bool drawn);

	void DrawSelf();

	static int Count;
	CGUIsysLbl* header; ///<Text header indentifying this group.
	int headerWidth; ///<The width of the header text int pixels. Used when drawing the outline box.
	int headerHeight; ///<The height of the header text in pixels.
	int headerIndent; ///<Indent of the header label from the side of the control.

private:
	bool isDrawn; ///<False if this class functions without being drawn. Not the same thing as being not visible.

};

