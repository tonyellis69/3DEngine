#pragma once


#include "GUIpanel.h"
#include "GUIimage1D.h"
#include "GUIcolourPicker.h"

#include "..\colourGradient.h"

struct colourTab { //wraps the colour slider tabs
	CGUIpanel* gui;
	float position;

};

class CGUIpaletteTab;
class CGUIpaletteBar : public CGUIpanel , public Icallback {
public:
	CGUIpaletteBar(int x, int y, int w, int h);
	//void DrawSelf();
	void setGradientTexture(CBaseTexture& texture);
	void OnClick(const  int mouseX, const  int mouseY);
	void MouseMsg(unsigned int Msg, int mouseX, int mouseY, int key);

	void createTab(int mouseX, int mouseY);
	void message(CGUIbase& sender, CMessage& msg);
	void GUIcallback(CGUIbase* sender, CMessage& msg);
	void changeTabColour(CGUIpaletteTab* sender);

	CGUIimage1D* paletteImage;
	int upperPadding;
	int barHeight;
	glm::i32vec2 tabSize;

	std::vector<colourTab> colourTabs;
	CGUIpaletteTab* colourChangeTab;

	CGUIcolourPicker* colourPicker;

	ColourGradient colourGradient;
};

class CGUIpaletteTab : public CGUIpanel {
public:
	CGUIpaletteTab(int x, int y, int w, int h) : CGUIpanel(x, y, w, h) {};
	void onDoubleClick(const int mouseX, const int mouseY);
	void onRMouseUp(const int mouseX, const int mouseY);
	float position;
	glm::vec4 colour;

};