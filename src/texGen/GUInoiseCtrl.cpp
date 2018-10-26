#include "GUInoiseCtrl.h"

#include "..\UI\GUIlabel2.h"

using namespace glm;

CGUInoiseCtrl::CGUInoiseCtrl(int x, int y, int w, int h) : CGUIpanel(x, y, w, h) {
	int CtrlStartY = 20;
	CGUIlabel2* lbl = new CGUIlabel2(20, CtrlStartY, 50, 20);
	lbl->setText("Octaves");
	Add(lbl);

	int CtrlCol2X = 200;
	lbl = new CGUIlabel2(CtrlCol2X, CtrlStartY, 50, 20);
	lbl->setText("Power");
	Add(lbl);

	lbl = new CGUIlabel2(20, CtrlStartY + 30, 80, 20);
	lbl->setText("Frequency");
	Add(lbl);

	lbl = new CGUIlabel2(20, CtrlStartY + 60, 80, 20);
	lbl->setText("Scale");
	Add(lbl);

	lbl = new CGUIlabel2(CtrlCol2X, CtrlStartY + 60, 80, 20);
	lbl->setText("Bias");
	Add(lbl);


	int ctrlX = 100;
	octaveCtrl = new CGUInumeric2(ctrlX, CtrlStartY, 70, 18);
	octaveCtrl->setValue(1);
	octaveCtrl->setMinMax(0, 32);
	Add(octaveCtrl);

	powerCtrl = new CGUInumeric2(CtrlCol2X + 60, CtrlStartY, 70, 18);
	powerCtrl->setValue(0.1f); powerCtrl->setIncrement(0.1f);
	powerCtrl->setMinMax(0, 32);
	Add(powerCtrl);

	freqCtrl = new CGUInumeric2(ctrlX, CtrlStartY+30, 70, 18);
	freqCtrl->setValue(1);
	freqCtrl->setMinMax(0, 32);
	Add(freqCtrl);

	scaleCtrl = new CGUInumeric2(ctrlX, CtrlStartY + 60, 70, 18);
	scaleCtrl->setValue(1);
	scaleCtrl->setMinMax(-100, 100);
	Add(scaleCtrl);

	biasCtrl = new CGUInumeric2(CtrlCol2X + 60, CtrlStartY + 60, 70, 18);
	biasCtrl->setValue(0.0f); biasCtrl->setIncrement(0.1f);
	biasCtrl->setMinMax(0, 32);
	Add(biasCtrl);


	//position controls
	int posStartY = 150;
	lbl = new CGUIlabel2(20, posStartY, 80, 20);
	lbl->setText("Rotation");
	Add(lbl);

	for (int axis = 0; axis < 3; axis++) {
		angle[axis] = new CGUInumeric2(ctrlX + axis * 80, posStartY, 70, 18);
		angle[axis]->setValue(0); angle[axis]->setMinMax(-360, 360);
		Add(angle[axis]);
	}

}


void CGUInoiseCtrl::message(CGUIbase* sender, CMessage& msg) {
	if (callbackObj) {
		callbackObj->GUIcallback(sender, msg);
	}
}

bool CGUInoiseCtrl::MouseWheelMsg(const  int mouseX, const  int mouseY, int wheelDelta, int key) {
	if (CGUIbase::MouseWheelMsg(mouseX, mouseY, wheelDelta, key))
		return true;
	//rotate stack
	CMessage msg;
	msg.Msg = uiMouseWheel;
	msg.value = wheelDelta;
	message(this, msg);


}


glm::vec3 CGUInoiseCtrl::getFullRotation() {
	vec3 vecAngle = vec3(angle[0]->value, angle[1] ->value, angle[2]->value);
	return vecAngle;
}
