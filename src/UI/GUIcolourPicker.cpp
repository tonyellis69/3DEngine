#include "GUIcolourPicker.h"
#include "..\colour.h"

CGUIcolourPicker::CGUIcolourPicker(int x, int y, int w, int h) : CGUIpanel(x, y, w, h) {
	int padding = 30;
	int panelSize = 90;
	colourPanel = new CGUIpanel(padding, padding, panelSize, panelSize);
	Add(colourPanel);
	int numericRow = padding + panelSize + padding;
	int numericWidth = 75; int numericHeight = 20; int sliderX = 140;
	int ySpace = numericRow; 
	for (int component = 0; component < 4; component++) {
		numeric[component] = new CGUInumeric2(padding, ySpace, numericWidth, numericHeight);
		numeric[component]->setMinMax(0, 255);
		numeric[component]->id = component;
		Add(numeric[component]);
		ySpace = numeric[component]->localPos.y + numeric[component]->drawBox.size.y + padding;

		slider[component] = new CGUIsysScrollbar(horizontal, sliderX, numeric[component]->localPos.y, 300);
		slider[component]->setMax(255); slider[component]->setMin(0);
		slider[component]->id = component;
		Add(slider[component]);

	}



	setColour(glm::i32vec4(255));

	CGUIbutton2* OKbutton = new CGUIbutton2(padding, numeric[3]->localPos.y + numeric[3]->drawBox.size.y + padding,
		numericWidth, numericHeight);
	OKbutton->setText("OK");
	OKbutton->id = uiOKid;
	Add(OKbutton);
}

/** Set the current selected colour. */
void CGUIcolourPicker::setColour(glm::i32vec4 newColour) {
	colour255 = newColour;
	glm::vec4 floatColour = col::byteToFloat(newColour);
	colourPanel->setBackColour1(floatColour);
	colourPanel->setBackColour2(floatColour);
	numeric[0]->setValue(newColour.r);
	numeric[1]->setValue(newColour.g);
	numeric[2]->setValue(newColour.b);
	numeric[3]->setValue(newColour.a);

	slider[0]->setValue(newColour.r);
	slider[1]->setValue(newColour.g);
	slider[2]->setValue(newColour.b);
	slider[3]->setValue(newColour.a);
}


void CGUIcolourPicker::MouseMsg(unsigned int Msg, int mouseX, int mouseY, int key) {
/*	if (IsOnControl(*paletteImage, mouseX, mouseY)) {
		if (Msg == MY_DOUBLECLICK) {
			if (MouseDown == paletteImage) {
				MouseDown = NULL;
				createTab(mouseX - paletteImage->localPos.x, mouseY - paletteImage->localPos.y);
			}
		}

	}*/
	CGUIbase::MouseMsg(Msg, mouseX, mouseY, key);
}

void CGUIcolourPicker::OnClick(const  int mouseX, const  int mouseY) {
	if (IsOnControl(*numeric[0], mouseX, mouseY)) {
		int b = 0;

	}
}

void CGUIcolourPicker::message(CGUIbase & sender, CMessage & msg) {
	if (msg.Msg == uiSpin || msg.Msg == uiDataEntered) {
		updateColour();
	}
	if (msg.Msg == uiClick) {
		if (callbackObj)
			callbackObj->GUIcallback(&sender,msg);
	}
	if (msg.Msg == uiMsgSlide) {
		int component = sender.id;
		colour255[component] = slider[component]->Value;
		setColour(colour255);
	}
}



/** Update current colour according to the state of the controls. */
void CGUIcolourPicker::updateColour() {
	colour255.r = (int)numeric[0]->value;
	colour255.g = (int)numeric[1]->value;
	colour255.b = (int)numeric[2]->value;
	colour255.a = (int)numeric[3]->value;
	//glm::vec4 floatColour = col::byteToFloat(colour255);
	//colourPanel->setBackColour1(floatColour);
	//colourPanel->setBackColour2(floatColour);
	setColour(colour255);
}

glm::i32vec4 CGUIcolourPicker::getColour() {
	return colour255;
}
