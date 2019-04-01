#pragma once

const glm::vec4 uiWhite = { 1,1,1,1 };
const glm::vec4 uiBlue = { 0,0,0.7f,1 };
const glm::vec4 uiBlack = { 0,0,0.0,1 };
const glm::vec4 uialmostBlack = { 0.1,0.1,0.1,1 };
//const glm::vec4 uiDarkGrey = { 0.25,0.25,0.25,1 };
const glm::vec4 uiDarkGrey = { 0.49f,0.49f,0.49f,1 };
const glm::vec4 uiLightishGrey = { 0.6f,0.6f,0.6f,1 };
const glm::vec4 uiLightGrey = { 0.75f,0.75f,0.75f,1 };
const glm::vec4 uiVeryLightGrey = { 0.88f,0.88f,0.88f,1 };

const glm::vec4 uiOldbackColour1 = { 0.99f,0.99f, 0.99f,1.0f };
const glm::vec4 uiOldbackColour2 = { 0.75f, 0.75f, 0.75f,1.0f };



/** Defines the structure for storing the style settings of the GUI.*/
class CGUIstyleSheet {
public:
	CGUIstyleSheet() : resizeMin(40, 10), resizeMax(200, 30),
		childSurround(10),
		labelTextSurround(2),
		buttonLabelSurround(4),
		buttonPresets{ {60,24}, {80,28},
		defaultButtonBackColour1 = glm::vec4(uiLightGrey),
		defaultButtonBackColour2 = glm::vec4(uiDarkGrey),
		defaultBorderColour = glm::vec4(uiDarkGrey),
		defaultBorderFocusColour = glm::vec4(uiLightishGrey)
	}
	
	{}
	CFont* defaultFont;
	glm::ivec4 defaultFontColour;

	glm::vec4 defaultButtonBackColour1;
	glm::vec4 defaultButtonBackColour2;
	glm::vec4 defaultBorderColour;
	glm::vec4 defaultBorderFocusColour;

	glm::i32vec2 resizeMin; ///<Minimum size resizeToFit is allowed to go to.
	glm::i32vec2 resizeMax; ///<Maximum size resizeToFit is allowed to go to.

	std::vector<glm::i32vec2> buttonPresets; ///<Preset sizes for buttons.

	int childSurround; ///<Spacing around child control and parent
	int labelTextSurround; ///<Spacing around label text
	int buttonLabelSurround; ///<Spacing around a button's label

};

