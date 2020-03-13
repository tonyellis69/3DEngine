#pragma once


#include "text.h" //for TtextTheme
#include "renderer/renderer.h"



namespace style {
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


	const glm::vec4 defaultBackgroundColour = uiWhite;

	extern CFont* defaultFont;
	const glm::vec4 defaultFontColour = uiBlack;

	const glm::vec4 defaultButtonBackColour1 = uiVeryLightGrey;
	const glm::vec4 defaultButtonBackColour2 = uiLightishGrey;
	const glm::vec4 defaultBorderColour = uiBlue;// uiDarkGrey,;
	const glm::vec4 defaultBorderFocusColour = uiLightishGrey;
	const glm::vec4 defaultScrollbarBackColour = uiLightishGrey;


	const glm::i32vec2 resizeMin = { 40,10 }; ///<Minimum size resizeToFit is allowed to go to.
	const glm::i32vec2 resizeMax = { 200, 30 }; ///<Maximum size resizeToFit is allowed to go to.

	const std::vector<glm::i32vec2> buttonPresets = { {60,24}, {80,28}, {100,32} }; ///<Preset sizes for buttons.

	const int controlBorder = 10; ///<Size of border surrounding child control area
	const int controlSpacing = 10; ///<Space between one control and the next.
	const int labelTextSurround = 2; ///<Spacing around label text
	const int buttonLabelSurround = 4; ///<Spacing around a button's label
	const int scrollbarLength = 80;
	const int scrollbarWidth = 12;
	const int scrollbarSliderSize = 30;



	const std::map<std::string, TtextStyle> defaultStyles = { 
		{ {"default"}, { "default","defaultFont",uiBlack } },
		{ {"hot"}, { "hot","defaultFont",glm::vec4(0,0,0.5,1) } },
		{ {"hotSelected"}, { "hotSelected","defaultFont",glm::vec4(0,0,1.0,1) } }

	};
	const TtextTheme defaultTheme = { "defaultTheme", { defaultStyles} };

/////////////old game additions
	const glm::vec4 darkGray = { 0.25, 0.25, 0.25, 1 };
	const glm::vec4 choiceColour = { 0.5, 0.5, 0.5, 1 };
	const glm::vec4 white = { 1,1,1,1 };
	const glm::vec4 hottextColour = { 0.28, 0.28, 0.47, 1 };
	const glm::vec4 hottextSelectedColour = { 1, 0.547, 0.0, 1 };

	const std::map<std::string, TtextStyle> gameNormalStyles = {
		{ {"mainBody"}, { "mainBody", "main", darkGray } },
		{ {"mainHeader"}, { "mainHeader","mainHeader",darkGray } },
		{ {"hot"}, { "hot", "main", hottextColour } },
		{ {"hotSelected"}, { "hotSelected", "main", hottextSelectedColour } },
		{ {"choice"}, { "choice", "main", choiceColour } }
	};

	const std::map<std::string, TtextStyle> smallNormalStyles = {
		{ {"small"}, { "small","small",darkGray } },
		{ {"smallHeader"}, { "smallHeader","smallHeader",darkGray } },
		{ {"hot"}, { "hot", "small", hottextColour } },
		{ {"hotSelected"}, { "hotSelected", "small", hottextSelectedColour } }
	};

	const TtextTheme gameNormalTheme = { "gameNormal", { gameNormalStyles} };
	const TtextTheme smallNormalTheme = { "smallNormal", { smallNormalStyles} };

	//////////////new game additions
	const std::map<std::string, TtextStyle> gameDefaultStyles = {
		{ {"default"}, { "default","defaultFont",uiWhite } },
		{ {"hot"}, { "hot","defaultFont",glm::vec4(0,0,0.5,1) } },
		{ {"hotSelected"}, { "hotSelected","defaultFont",glm::vec4(0,0,1.0,1) } }
	};
	const TtextTheme gameTheme = { "gameTheme", { gameDefaultStyles} };

	const glm::vec4 gameBackColour = { 0,0,0,1 };
	const glm::vec4 gameBorderColour = { 1,1,1,1 };
	const glm::i32vec2 richTextInset = { 10,10 };



	const std::map<std::string, TtextTheme> themes2 = {
		{ {"defaultTheme"}, {defaultTheme} },
		{ {"gameNormal"}, {gameNormalTheme} },
		{ {"smallNormal"}, {smallNormalTheme} },
		{ {"gameTheme"}, {gameTheme} }
	};



}














