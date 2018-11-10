#pragma once

#include "..\UI\GUIpanel.h"
#include "..\UI\GUInumeric2.h"
#include "..\UI\GUIbutton2.h"
#include "..\UI\GUIcheckButton.h"

/** A developer-only class for playing with noise functions, live. */
class CGUInoiseCtrl : public CGUIpanel {
public:
	CGUInoiseCtrl(int x, int y, int w, int h);
	void message(CGUIbase * sender, CMessage & msg);
	bool MouseWheelMsg(const int mouseX, const int mouseY, int wheelDelta, int key);
	glm::vec3 getFullRotation();
	glm::vec3 getFullPosition();
	glm::vec3 getScalePt();

	CGUInumeric2* octaveCtrl;
	CGUInumeric2* freqCtrl;
	CGUInumeric2* powerCtrl;
	CGUInumeric2* persistCtrl;
	CGUInumeric2* scaleCtrl;
	CGUInumeric2* biasCtrl;

	CGUIcheckButton* distCtrl;
	CGUIcheckButton* randHueCtrl;

	CGUInumeric2* angleCtrl[3];
	CGUInumeric2* posCtrl[3];
	CGUInumeric2* scalePtCtrl[3];

	CGUInumeric2* lowerCtrl;
	CGUInumeric2* upperCtrl;
	CGUInumeric2* falloffCtrl;

	CGUInumeric2* widthCtrl;
	CGUInumeric2* heightCtrl;

	CGUIbutton2* saveButton;
};