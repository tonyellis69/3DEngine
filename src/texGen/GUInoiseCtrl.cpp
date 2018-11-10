#include "GUInoiseCtrl.h"

#include "..\UI\GUIlabel2.h"

using namespace glm;

CGUInoiseCtrl::CGUInoiseCtrl(int x, int y, int w, int h) : CGUIpanel(x, y, w, h) {
	int CtrlStartY = 20;
	CGUIlabel2* lbl = new CGUIlabel2(20, CtrlStartY, 50, 20);
	lbl->setText("Octaves");
	Add(lbl);

	int CtrlCol2X = 190;
	lbl = new CGUIlabel2(CtrlCol2X, CtrlStartY, 50, 20);
	lbl->setText("Power");
	Add(lbl);

	lbl = new CGUIlabel2(20, CtrlStartY + 30, 80, 20);
	lbl->setText("Frequency");
	Add(lbl);

	lbl = new CGUIlabel2(CtrlCol2X, CtrlStartY + 30, 80, 20);
	lbl->setText("Persistance");
	Add(lbl);

	lbl = new CGUIlabel2(20, CtrlStartY + 60, 80, 20);
	lbl->setText("Scale");
	Add(lbl);

	lbl = new CGUIlabel2(CtrlCol2X, CtrlStartY + 60, 80, 20);
	lbl->setText("Bias");
	Add(lbl);

	lbl = new CGUIlabel2(20, CtrlStartY + 90, 80, 20);
	lbl->setText("Distance");
	Add(lbl);

	lbl = new CGUIlabel2(CtrlCol2X, CtrlStartY + 90, 80, 20);
	lbl->setText("Random hue");
	Add(lbl);


	int ctrlX = 100;
	octaveCtrl = new CGUInumeric2(ctrlX, CtrlStartY, 70, 18);
	octaveCtrl->setValue(1);
	octaveCtrl->setMinMax(0, 32);
	Add(octaveCtrl);

	powerCtrl = new CGUInumeric2(CtrlCol2X + 90, CtrlStartY, 70, 18);
	powerCtrl->setValue(0.1f); powerCtrl->setIncrement(0.1f);
	powerCtrl->setMinMax(0, 32);
	Add(powerCtrl);

	freqCtrl = new CGUInumeric2(ctrlX, CtrlStartY+30, 70, 18);
	freqCtrl->setValue(1);
	freqCtrl->setMinMax(0, 100);
	Add(freqCtrl);

	persistCtrl = new CGUInumeric2(CtrlCol2X + 90, CtrlStartY + 30, 70, 18);
	persistCtrl->setValue(0.5f); persistCtrl->setIncrement(0.1f);
	persistCtrl->setMinMax(0, 100);
	Add(persistCtrl);



	scaleCtrl = new CGUInumeric2(ctrlX, CtrlStartY + 60, 70, 18);
	scaleCtrl->setValue(1); scaleCtrl->setIncrement(0.1f);
	scaleCtrl->setMinMax(-100, 100);
	Add(scaleCtrl);

	biasCtrl = new CGUInumeric2(CtrlCol2X + 90, CtrlStartY + 60, 70, 18);
	biasCtrl->setValue(0.0f); biasCtrl->setIncrement(0.1f);
	biasCtrl->setMinMax(-32, 32);
	Add(biasCtrl);

	distCtrl = new CGUIcheckButton(ctrlX, CtrlStartY + 90, 18, 18);
	distCtrl->Set = true;
	distCtrl->setText("xxxx");
	Add(distCtrl);

	randHueCtrl = new CGUIcheckButton(CtrlCol2X + 90, CtrlStartY + 90, 18, 18);
	randHueCtrl->Set = false;
	randHueCtrl->setText("xxxx");
	Add(randHueCtrl);



	int saveButtonY = 330;
	saveButton = new CGUIbutton2(ctrlX, saveButtonY, 70, 20);
	saveButton->setText("Save");
	Add(saveButton);



	//position controls
	int posStartY = 150;
	lbl = new CGUIlabel2(20, posStartY, 80, 20);
	lbl->setText("Rotation");
	Add(lbl);

	for (int axis = 0; axis < 3; axis++) {
		angleCtrl[axis] = new CGUInumeric2(ctrlX + axis * 80, posStartY, 70, 18);
		angleCtrl[axis]->setValue(0); angleCtrl[axis]->setMinMax(-360, 360);
		Add(angleCtrl[axis]);
	}

	lbl = new CGUIlabel2(20, posStartY +30 , 80, 20);
	lbl->setText("Translation");
	Add(lbl);

	for (int axis = 0; axis < 3; axis++) {
		posCtrl[axis] = new CGUInumeric2(ctrlX + axis * 80, posStartY + 30, 70, 18);
		posCtrl[axis]->setValue(0); posCtrl[axis]->setMinMax(-100, 100);
		posCtrl[axis]->setIncrement(0.1f);
		Add(posCtrl[axis]);
	}

	lbl = new CGUIlabel2(20, posStartY + 60, 80, 20);
	lbl->setText("Scale point");
	Add(lbl);

	for (int axis = 0; axis < 3; axis++) {
		scalePtCtrl[axis] = new CGUInumeric2(ctrlX + axis * 80, posStartY + 60, 70, 18);
		scalePtCtrl[axis]->setValue(1); scalePtCtrl[axis]->setMinMax(-100, 100);
		scalePtCtrl[axis]->setIncrement(0.1f);
		Add(scalePtCtrl[axis]);
	}

	lbl = new CGUIlabel2(20, posStartY + 90, 80, 20);
	lbl->setText("Lower");
	Add(lbl);

	lowerCtrl = new CGUInumeric2(ctrlX, posStartY + 90, 70, 18);
	lowerCtrl->setValue(0); lowerCtrl->setMinMax(0, 1); lowerCtrl->setIncrement(0.1f);
	Add(lowerCtrl);

	lbl = new CGUIlabel2(CtrlCol2X, posStartY + 90, 80, 20);
	lbl->setText("Upper");
	Add(lbl);

	upperCtrl = new CGUInumeric2(CtrlCol2X + 70, posStartY + 90, 70, 18);
	upperCtrl->setValue(1); upperCtrl->setMinMax(0, 1); upperCtrl->setIncrement(0.1f);
	Add(upperCtrl);

	lbl = new CGUIlabel2(20, posStartY + 120, 80, 20);
	lbl->setText("Falloff");
	Add(lbl);

	falloffCtrl = new CGUInumeric2(ctrlX, posStartY + 120, 70, 18);
	falloffCtrl->setValue(0); falloffCtrl->setMinMax(0, 1); falloffCtrl->setIncrement(0.1f);
	Add(falloffCtrl);

	lbl = new CGUIlabel2(20, posStartY + 150, 80, 20);
	lbl->setText("Samp. width");
	Add(lbl);

	widthCtrl = new CGUInumeric2(ctrlX, posStartY + 150, 70, 18);
	widthCtrl->setValue(2); widthCtrl->setMinMax(0, 10);
	Add(widthCtrl);

	lbl = new CGUIlabel2(CtrlCol2X, posStartY + 150, 80, 20);
	lbl->setText("height");
	Add(lbl);

	heightCtrl = new CGUInumeric2(CtrlCol2X + 70, posStartY + 150, 70, 18);
	heightCtrl->setValue(2); heightCtrl->setMinMax(0, 10);
	Add(heightCtrl);


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
	vec3 vecAngle = vec3(angleCtrl[0]->value, angleCtrl[1] ->value, angleCtrl[2]->value);
	return vecAngle;
}

glm::vec3 CGUInoiseCtrl::getFullPosition() {
	vec3 vecPos = vec3(posCtrl[0]->value, posCtrl[1]->value, posCtrl[2]->value);
	return vecPos;
}

glm::vec3 CGUInoiseCtrl::getScalePt() {
	vec3 vecScale = vec3(scalePtCtrl[0]->value, scalePtCtrl[1]->value, scalePtCtrl[2]->value);
	return vecScale;
}
