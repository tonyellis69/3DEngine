#include "GUInoiseCtrl.h"

#include "..\UI\GUIlabel2.h"

using namespace glm;

CGUInoiseCtrl::CGUInoiseCtrl(int x, int y, int w, int h) : CGUIpanel(x, y, w, h) {
	int CtrlStartY = 20;
	CGUIlabel2* lbl = new CGUIlabel2(20, CtrlStartY, 50, 20);
	lbl->setText("Octaves");

	int ctrlX = 100;
	octaveCtrl = new CGUInumeric2(ctrlX, CtrlStartY, 70, 18);
	octaveCtrl->setValue(1);
	octaveCtrl->setMinMax(0, 32);


	int CtrlCol2X = 190;
	CGUIlabel2* lbl2 = new CGUIlabel2(CtrlCol2X, CtrlStartY, 50, 20);
	lbl2->setText("Power");

	powerCtrl = new CGUInumeric2(CtrlCol2X + 90, CtrlStartY, 70, 18);
	powerCtrl->setValue(0.1f); powerCtrl->setIncrement(0.1f);
	powerCtrl->setMinMax(0, 32);

	addToRow("octave", { lbl,octaveCtrl });
	addToRow("octavePower", { lbl, octaveCtrl, lbl2, powerCtrl });


	lbl = new CGUIlabel2(20, CtrlStartY + 30, 80, 20);
	lbl->setText("Frequency");

	freqCtrl = new CGUInumeric2(ctrlX, CtrlStartY + 30, 70, 18);
	freqCtrl->setValue(1);
	freqCtrl->setMinMax(0, 100);

	addToRow("freq", { lbl, freqCtrl });
	addToRow("freqPersist", { lbl, freqCtrl });


	lbl = new CGUIlabel2(CtrlCol2X, CtrlStartY + 30, 80, 20);
	lbl->setText("Persistance");

	persistCtrl = new CGUInumeric2(CtrlCol2X + 90, CtrlStartY + 30, 70, 18);
	persistCtrl->setValue(0.5f); persistCtrl->setIncrement(0.1f);
	persistCtrl->setMinMax(0, 100);

	addToRow("freqPersist", { lbl, persistCtrl });



	lbl = new CGUIlabel2(20, CtrlStartY + 60, 80, 20);
	lbl->setText("Scale");

	scaleCtrl = new CGUInumeric2(ctrlX, CtrlStartY + 60, 70, 18);
	scaleCtrl->setValue(1); scaleCtrl->setIncrement(0.1f);
	scaleCtrl->setMinMax(-100, 100);

	addToRow("scaleBias", { lbl,scaleCtrl });

	lbl = new CGUIlabel2(CtrlCol2X, CtrlStartY + 60, 80, 20);
	lbl->setText("Bias");

	biasCtrl = new CGUInumeric2(CtrlCol2X + 90, CtrlStartY + 60, 70, 18);
	biasCtrl->setValue(0.0f); biasCtrl->setIncrement(0.1f);
	biasCtrl->setMinMax(-32, 32);

	addToRow("scaleBias", { lbl,biasCtrl });

	distCtrl = new CGUIcheckButton(20, CtrlStartY + 90, 96, 20);
	distCtrl->Set = true;
	distCtrl->setText("Distance");

	randHueCtrl = new CGUIcheckButton(CtrlCol2X, CtrlStartY + 90, 108, 20);
	randHueCtrl->Set = false;
	randHueCtrl->setText("Random hue");

	addToRow("distHue", { distCtrl,randHueCtrl });

	int saveButtonY = 330;

	//position controls
	int posStartY = 150;
	lbl = new CGUIlabel2(20, posStartY, 80, 20);
	lbl->setText("Rotation");
	addToRow("rotation", { lbl });

	for (int axis = 0; axis < 3; axis++) {
		angleCtrl[axis] = new CGUInumeric2(ctrlX + axis * 80, posStartY, 70, 18);
		angleCtrl[axis]->setValue(0); angleCtrl[axis]->setMinMax(-360, 360);
		addToRow("rotation", { angleCtrl[axis] });
	}

	lbl = new CGUIlabel2(20, posStartY + 30, 80, 20);
	lbl->setText("Translation");
	addToRow("translation", { lbl });

	for (int axis = 0; axis < 3; axis++) {
		posCtrl[axis] = new CGUInumeric2(ctrlX + axis * 80, posStartY + 30, 70, 18);
		posCtrl[axis]->setValue(0); posCtrl[axis]->setMinMax(-100, 100);
		posCtrl[axis]->setIncrement(0.1f);
		addToRow("translation", { posCtrl[axis] });
	}

	lbl = new CGUIlabel2(20, posStartY + 60, 80, 20);
	lbl->setText("Scale point");
	addToRow("scalePoint", { lbl });
	for (int axis = 0; axis < 3; axis++) {
		scalePtCtrl[axis] = new CGUInumeric2(ctrlX + axis * 80, posStartY + 60, 70, 18);
		scalePtCtrl[axis]->setValue(1); scalePtCtrl[axis]->setMinMax(-100, 100);
		scalePtCtrl[axis]->setIncrement(0.1f);
		addToRow("scalePoint", { scalePtCtrl[axis] });
	}

	lbl = new CGUIlabel2(20, posStartY + 90, 80, 20);
	lbl->setText("Lower");

	lowerCtrl = new CGUInumeric2(ctrlX, posStartY + 90, 70, 18);
	lowerCtrl->setValue(0); lowerCtrl->setMinMax(0, 1); lowerCtrl->setIncrement(0.1f);
	addToRow("lowerUpper", { lbl,lowerCtrl });

	lbl = new CGUIlabel2(CtrlCol2X, posStartY + 90, 80, 20);
	lbl->setText("Upper");

	upperCtrl = new CGUInumeric2(CtrlCol2X + 70, posStartY + 90, 70, 18);
	upperCtrl->setValue(1); upperCtrl->setMinMax(0, 1); upperCtrl->setIncrement(0.1f);
	addToRow("lowerUpper", { lbl,upperCtrl });



	lbl = new CGUIlabel2(20, posStartY + 120, 80, 20);
	lbl->setText("Falloff");

	falloffCtrl = new CGUInumeric2(ctrlX, posStartY + 120, 70, 18);
	falloffCtrl->setValue(0.5f); falloffCtrl->setMinMax(0, 1); falloffCtrl->setIncrement(0.1f);
	addToRow("falloff", { lbl,falloffCtrl });

	lbl = new CGUIlabel2(20, posStartY + 150, 80, 20);
	lbl->setText("Samp. width");

	widthCtrl = new CGUInumeric2(ctrlX, posStartY + 150, 70, 18);
	widthCtrl->setValue(2); widthCtrl->setMinMax(0, 10);
	addToRow("sampWidthHeight", { lbl,widthCtrl });

	lbl = new CGUIlabel2(CtrlCol2X, posStartY + 150, 90, 20);
	lbl->setText("Samp. height");

	heightCtrl = new CGUInumeric2(CtrlCol2X + 90, posStartY + 150, 70, 18);
	heightCtrl->setValue(2); heightCtrl->setMinMax(0, 10);
	addToRow("sampWidthHeight", { lbl,heightCtrl });

	lbl = new CGUIlabel2(20, 0, 80, 20);
	lbl->setText("Source 1");

	src1Ctrl = new CGUIdropdownMenu(ctrlX, 0, 80, 20);
	src1Ctrl->setText("");
	src1Ctrl->setGUIcallback(this);
	addToRow("source1", { lbl, src1Ctrl });

	lbl2 = new CGUIlabel2(CtrlCol2X, 0, 80, 20);
	lbl2->setText("Source 2");

	src2Ctrl = new CGUIdropdownMenu(CtrlCol2X + 70, 0, 80, 20);
	src2Ctrl->setText("");
	src2Ctrl->setGUIcallback(this);
	addToRow("source1&2", { lbl, src1Ctrl, lbl2, src2Ctrl });

	lbl = new CGUIlabel2(20, 0, 80, 20);
	lbl->setText("Source 3");

	src3Ctrl = new CGUIdropdownMenu(ctrlX, 0, 80, 20);
	src3Ctrl->setText("");
	src3Ctrl->setGUIcallback(this);
	addToRow("source3", { lbl, src3Ctrl });

	paletteBar = new CGUIpaletteBar(20, 10, 300, 150);
	paletteBar->loadPalette();
	paletteBar->setGUIcallback(this);
	addToRow("paletteBar", { paletteBar });

	swatchCtrl = new CGUIswatchContainer(20, 10, 300, 130);
	addToRow("swatches", { swatchCtrl });


	lbl = new CGUIlabel2(20, posStartY + 150, 90, 20);
	lbl->setText("Size");
	gausSizeCtrl = new CGUInumeric2(ctrlX, posStartY + 150, 70, 18);
	gausSizeCtrl->setValue(5); gausSizeCtrl->setMinMax(1, 20);

	lbl2 = new CGUIlabel2(CtrlCol2X, 0, 80, 20);
	lbl2->setText("Sigma");
	gausSigmaCtrl = new CGUInumeric2(CtrlCol2X + 70, 0, 70, 18);
	gausSigmaCtrl->setValue(2); gausSigmaCtrl->setMinMax(1, 20);
	gausSigmaCtrl->setIncrement(0.1f);
	addToRow("gausSizeSigma", { lbl,gausSizeCtrl,lbl2,gausSigmaCtrl });

	lbl = new CGUIlabel2(CtrlCol2X, posStartY + 150, 90, 20);
	lbl->setText("Percent");
	percentageCtrl = new CGUInumeric2(CtrlCol2X + 70, posStartY + 150, 70, 18);
	percentageCtrl->setValue(10); percentageCtrl->setMinMax(1, 50);

	lbl2 = new CGUIlabel2(20, 0, 80, 20);
	lbl2->setText("Seam falloff");

	addToRow("seamPercentFalloff", {lbl2, falloffCtrl,lbl, percentageCtrl });


	lbl = new CGUIlabel2(20, posStartY + 150, 90, 20);
	lbl->setText("Depth");
	rectsDepthCtrl = new CGUInumeric2(ctrlX, posStartY + 150, 70, 18);
	rectsDepthCtrl->setValue(3); rectsDepthCtrl->setMinMax(1, 10);

	lbl2 = new CGUIlabel2(CtrlCol2X, 0, 80, 20);
	lbl2->setText("Variance");
	rectsVarianceCtrl = new CGUInumeric2(CtrlCol2X + 70, 0, 70, 18);
	rectsVarianceCtrl->setValue(0.15f); rectsVarianceCtrl->setMinMax(0, 1);
	rectsVarianceCtrl->setIncrement(0.01f);
	addToRow("rectsDepthVariance", { lbl,rectsDepthCtrl,lbl2,rectsVarianceCtrl });

	lbl = new CGUIlabel2(20, posStartY + 150, 90, 20);
	lbl->setText("Style");
	rectsStyleCtrl = new CGUInumeric2(ctrlX, posStartY + 150, 70, 18);
	rectsStyleCtrl->setValue(1); rectsStyleCtrl->setMinMax(0,2);

	lbl2 = new CGUIlabel2(CtrlCol2X, 0, 80, 20);
	lbl2->setText("Clusters");
	clustersCtrl = new CGUInumeric2(CtrlCol2X + 70, 0, 70, 18);
	clustersCtrl->setValue(0.0f); rectsVarianceCtrl->setMinMax(0, 1);
	clustersCtrl->setIncrement(0.01f);

	addToRow("styleClusters", { lbl,rectsStyleCtrl,lbl2, clustersCtrl });


	lbl = new CGUIlabel2(20, posStartY + 150, 90, 20);
	lbl->setText("Stages");
	blocksStagesCtrl = new CGUInumeric2(ctrlX, posStartY + 150, 70, 18);
	blocksStagesCtrl->setValue(2); blocksStagesCtrl->setMinMax(1, 10);

	lbl2 = new CGUIlabel2(CtrlCol2X, 0, 80, 20);
	lbl2->setText("Iterations");
	blocksIterationsCtrl = new CGUInumeric2(CtrlCol2X + 70, 0, 70, 18);
	blocksIterationsCtrl->setValue(3); blocksIterationsCtrl->setMinMax(1, 10);

	addToRow("stagesIterations", { lbl,blocksStagesCtrl,lbl2,blocksIterationsCtrl });

	lbl = new CGUIlabel2(20, posStartY + 150, 90, 20);
	lbl->setText("Density");
	blocksDensityCtrl = new CGUInumeric2(ctrlX, posStartY + 150, 70, 18);
	blocksDensityCtrl->setValue(1); blocksDensityCtrl->setMinMax(0, 10);

	lbl2 = new CGUIlabel2(CtrlCol2X, 0, 80, 20);
	lbl2->setText("Scale");
	blocksScaleCtrl = new CGUInumeric2(CtrlCol2X + 70, 0, 70, 18);
	blocksScaleCtrl->setValue(2); blocksScaleCtrl->setMinMax(1, 10);
	blocksScaleCtrl->setIncrement(0.5f);

	addToRow("densityScale", { lbl,blocksDensityCtrl,lbl2,blocksScaleCtrl });

	lbl = new CGUIlabel2(20, posStartY + 150, 90, 20);
	lbl->setText("Iterations");
	coverIterationsCtrl = new CGUInumeric2(ctrlX, posStartY + 150, 70, 18);
	coverIterationsCtrl->setValue(1); coverIterationsCtrl->setMinMax(1, 200);

	lbl2 = new CGUIlabel2(CtrlCol2X, 0, 80, 20);
	lbl2->setText("Scale");
	coverScaleCtrl = new CGUInumeric2(CtrlCol2X + 70, 0, 70, 18);
	coverScaleCtrl->setValue(1); coverScaleCtrl->setMinMax(0.05, 10);
	coverScaleCtrl->setIncrement(0.1f);

	addToRow("iterationsScale", { lbl,coverIterationsCtrl,lbl2,coverScaleCtrl });



}


void CGUInoiseCtrl::message(CGUIbase* sender, CMessage& msg) {
	if (callbackObj) {
		callbackObj->GUIcallback(sender, msg);
	}
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

void CGUInoiseCtrl::GUIcallback(CGUIbase * sender, CMessage & msg) {
	if ( callbackObj)
		callbackObj->GUIcallback(sender, msg);
}

/** Get paletteBar control to indicate this colour on the gradient. */
void CGUInoiseCtrl::setIndicatorPosition(int indexPos) {
	paletteBar->setIndicatorPosition(indexPos);
}
