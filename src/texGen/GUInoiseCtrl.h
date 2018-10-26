#pragma once

#include "..\UI\GUIpanel.h"
#include "..\UI\GUInumeric2.h"

/** A developer-only class for playing with noise functions, live. */
class CGUInoiseCtrl : public CGUIpanel {
public:
	CGUInoiseCtrl(int x, int y, int w, int h);
	void message(CGUIbase * sender, CMessage & msg);
	bool MouseWheelMsg(const int mouseX, const int mouseY, int wheelDelta, int key);
	glm::vec3 getFullRotation();

	CGUInumeric2* octaveCtrl;
	CGUInumeric2* freqCtrl;
	CGUInumeric2* powerCtrl;
	CGUInumeric2* scaleCtrl;
	CGUInumeric2* biasCtrl;

	CGUInumeric2* angle[3];
};