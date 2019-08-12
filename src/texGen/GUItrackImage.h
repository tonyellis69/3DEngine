#pragma once

#include "..\UI\GUIimage.h"

/** A GUIimage control that reports mouse movement. */
class CGUItrackImage : public CGUIimage {
public:
	CGUItrackImage(int x, int y, int w, int h) : CGUIimage(x, y, w, h) {};
	void OnMouseMove(const  int mouseX, const  int mouseY, int key);
	bool onMouseOff(const  int mouseX, const  int mouseY, int key);
	void OnRMouseDown(const  int mouseX, const  int mouseY, int key);
	bool MouseWheelMsg(const  int mouseX, const  int mouseY, int wheelDelta, int key);
	void onDoubleClick(const int mouseX, const int mouseY, int key);

};
