#pragma once

#include "GUIpanel.h"
#include "GUItextbox2.h"
#include "GUInumeric2.h"
#include "GUIbutton2.h"
#include "GUIscrollbar.h"

//typedef void(*msgCallback)(CGUIbase*, CMessage& );

class CGUIcolourPicker : public CGUIpanel {
public:
	CGUIcolourPicker(int x, int y, int w, int h) ;
	void setColour(glm::i32vec4 newColour);
	void MouseMsg(unsigned int Msg, int mouseX, int mouseY, int key);
	void OnClick(const  int mouseX, const  int mouseY);
	void message(CGUIbase* sender, CMessage& msg);
	//void setMsgCallback(msgCallback callbackFn);
	void updateColour();
	glm::i32vec4 getColour();

	CGUIpanel* colourPanel;

	CGUInumeric2* numeric[4];
	

	CGUIsysScrollbar* slider[4];

	CGUIbutton2* OKbutton;

	glm::i32vec4 colour255;

};