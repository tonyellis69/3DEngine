#pragma once


#include "GUIpanel.h"
#include "GUIimage1D.h"
#include "GUIcolourPicker.h"
#include "GUIdropdownMenu.h"

#include "..\colourGradient.h"

struct colourTab { //wraps the colour slider tabs
	CGUIpanel* gui;
	float position;
};

class CGUIpaletteTab;
class CGUIpaletteBar : public CGUIpanel  {
public:
	CGUIpaletteBar(int x, int y, int w, int h);
	
	void setGradientTexture(CBaseTexture& texture);
	void setGradient(ColourGradient& gradient);
	bool MouseMsg(unsigned int Msg, int mouseX, int mouseY, int key);

	void createTab(int mouseX);
	CGUIpaletteTab * addTab(int tabIndex);
	void message(CGUIbase* sender, CMessage& msg);
	void deleteTab(CGUIpaletteTab * tab);
	void moveTab(CGUIpaletteTab * tab, int newPos);
	void GUIcallback(CGUIbase* sender, CMessage& msg);
	void editTabColour(CGUIpaletteTab* sender);
	void changeTabColour(glm::i32vec4& newColour);
	void loadPalette();
	void clearTabControls();
	void* getPixels();
	void updatePalette();
	void addControls();
	void save();
	void setIndicatorPosition(int indexPos);
	void DrawSelf();
	void createTabAtIndicator(float falloff);
	void onDrop(const  int mouseX, const  int mouseY);


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

	CGUIsysScrollbar* hueSlider;
	float oldHueRotation;


	CGUIdropdownMenu* restoreMenu;
	CGUIbutton2* saveButton;
	CGUItextbox2* fileNameBox;

	CGUIlabel2* tabPosLbl; //<Label showing index positionHint of a dragged tab
	int indicator;
};

class CGUIpaletteTab : public CGUIpanel {
public:
	CGUIpaletteTab(int x, int y, int w, int h) : CGUIpanel(x, y, w, h) {
		setBorderOn(false); 
		type = uiPaletteTab;
	};
	bool onDoubleClick(const int mouseX, const int mouseY, int key);
	bool OnLMouseDown(const  int mouseX, const  int mouseY, int key);
	bool onRMouseUp(const int mouseX, const int mouseY);
	bool OnMouseMove(int mouseX, int mouseY, int key);
	void onDrop(const  int mouseX, const  int mouseY);
	bool MouseWheelMsg(const  int mouseX, const  int mouseY, int wheelDelta, int key);


	int position;
	glm::i32vec4 colour;
	int lastMouseX;
};

