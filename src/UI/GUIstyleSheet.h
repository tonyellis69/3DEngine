#pragma once

struct TtextStyleDef {
	std::string name;
	std::string font;
	glm::vec4 colour;
};


//colour constants - add to, but don't change
const glm::vec4 uiWhite = { 1,1,1,1 };
const glm::vec4 uiRed = { 1,0,0,1 };
const glm::vec4 uiBlue = { 0,0,0.7f,1 };
const glm::vec4 uiBlack = { 0,0,0.0,1 };
const glm::vec4 uialmostBlack = { 0.1,0.1,0.1,1 };
//const glm::vec4 uiDarkGrey = { 0.25,0.25,0.25,1 };
const glm::vec4 uiDarkGrey = { 0.49f,0.49f,0.49f,1 };
const glm::vec4 uiLightishGrey = { 0.6f,0.6f,0.6f,1 };
const glm::vec4 uiLightGrey = { 0.75f,0.75f,0.75f,1 };
const glm::vec4 uiVeryLightGrey = { 0.88f,0.88f,0.88f,1 };

const glm::vec4 uiLightBlue = { 0.6f,0.6f,0.9f,1 };

const glm::vec4 uiOldbackColour1 = { 0.99f,0.99f, 0.99f,1.0f };
const glm::vec4 uiOldbackColour2 = { 0.75f, 0.75f, 0.75f,1.0f };





/** Defines the structure for storing the style settings of the GUI, and initialises
	it with default values.*/
class CGUIstyleSheet {
public:
	CGUIstyleSheet() : defaultBackgroundColour(uiWhite),
		resizeMin(40, 10), resizeMax(200, 30),
		controlBorder(10),
		controlSpacing(10),
		defaultFontColour(uiRed),
		richtextStyle{ "default","defaultFont",uiBlack },
		richtextHotStyle{ "hot","defaultFont",glm::vec4(0,0,0.5,1) },
		richtextSelectedStyle{ "hotSelected","defaultFont",glm::vec4(0,0,1.0,1) },

		labelTextSurround(2),
		buttonLabelSurround(4),
		buttonPresets{ {60,24}, {80,28}, {100,32},
			//defaultButtonBackColour1 = glm::vec4(uiLightGrey),
			//defaultButtonBackColour2 = glm::vec4(uiDarkGrey),
			defaultButtonBackColour1 = uiVeryLightGrey,
			defaultButtonBackColour2 = uiLightishGrey,

		defaultBorderColour = uiRed,// uiDarkGrey,
		defaultBorderFocusColour = uiLightishGrey
		},
		scrollbarLength(80),
		scrollbarWidth(12),
		scrollbarSliderSize(30),
		defaultScrollbarBackColour{ uiLightishGrey }
	{

	
	}

	glm::vec4 defaultBackgroundColour; 

	CFont* defaultFont;
	glm::vec4 defaultFontColour;

	glm::vec4 defaultButtonBackColour1;
	glm::vec4 defaultButtonBackColour2;
	glm::vec4 defaultBorderColour;
	glm::vec4 defaultBorderFocusColour;
	glm::vec4 defaultScrollbarBackColour;


	glm::i32vec2 resizeMin; ///<Minimum size resizeToFit is allowed to go to.
	glm::i32vec2 resizeMax; ///<Maximum size resizeToFit is allowed to go to.

	std::vector<glm::i32vec2> buttonPresets; ///<Preset sizes for buttons.

	int controlBorder; ///<Size of border surrounding child control area
	int controlSpacing; ///<Space between one control and the next.
	int labelTextSurround; ///<Spacing around label text
	int buttonLabelSurround; ///<Spacing around a button's label
	int scrollbarLength; 
	int scrollbarWidth;
	int scrollbarSliderSize;

	TtextStyleDef richtextStyle; ///<The default richtext styles
	TtextStyleDef richtextHotStyle;
	TtextStyleDef richtextSelectedStyle;
};

