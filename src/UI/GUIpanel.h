#pragma once
#include "GUIbase.h"

class CGUIpanel : public CGUIbase {
public:
	CGUIpanel() {};
	CGUIpanel(int x, int y, int w, int h);
	void OnMouseMove(const int mouseX, const int mouseY, int key) {};
	void DrawSelf();


	
};