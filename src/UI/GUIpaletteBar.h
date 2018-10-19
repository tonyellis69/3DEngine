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
class CGUIpaletteBar : public CGUIpanel  {
public:
	CGUIpaletteBar(int x, int y, int w, int h);
	//void DrawSelf();
	void setGradientTexture(CBaseTexture& texture);
	void setGradient(ColourGradient& gradient);
	void OnClick(const  int mouseX, const  int mouseY);
	void MouseMsg(unsigned int Msg, int mouseX, int mouseY, int key);

	void createTab(int mouseX);
	void message(CGUIbase* sender, CMessage& msg);
	void deleteTab(CGUIpaletteTab * tab);
	void moveTab(CGUIpaletteTab * tab, int newPos);
	void GUIcallback(CGUIbase* sender, CMessage& msg);
	void editTabColour(CGUIpaletteTab* sender);
	void changeTabColour(glm::i32vec4& newColour);
	void logPalette();
	void loadPalette();
	void clearTabControls();
	void* getPixels();
	void updatePalette();

	int paletteImageStartX;
	int paletteImageEndX;

	CGUIimage1D* paletteImage;
	int upperPadding;
	int barHeight;
	glm::i32vec2 tabSize;


	CGUIpaletteTab* colourChangeTab;

	CGUIcolourPicker* colourPicker;

	ColourGradient colourGradient;
	CRenderTexture paletteTexture;

	int logButtonID;
};

class CGUIpaletteTab : public CGUIpanel {
public:
	CGUIpaletteTab(int x, int y, int w, int h) : CGUIpanel(x, y, w, h) {
		borderOn(false); 
		type = uiPaletteTab;
	};
	void onDoubleClick(const int mouseX, const int mouseY);
	void OnLMouseDown(const  int mouseX, const  int mouseY, int key);
	void onRMouseUp(const int mouseX, const int mouseY);
	void OnMouseMove(int mouseX, int mouseY, int key);
	int position;
	glm::vec4 colour;
	int lastMouseX;
};

