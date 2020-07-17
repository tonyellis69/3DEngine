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
	const glm::vec4 gameReservedColour = { 0,1,0,0.75f };

	const std::map<std::string, TtextStyle> gameNormalStyles = {
		{ {"default"}, { "default", "main", darkGray } },
		{ {"mainBody"}, { "mainBody", "main", darkGray } },
		{ {"mainHeader"}, { "mainHeader","mainHeader",darkGray } },
		{ {"hot"}, { "hot", "main", hottextColour } },
		{ {"hotSelected"}, { "hotSelected", "main", hottextSelectedColour } },
		{ {"choice"}, { "choice", "main", choiceColour } }
	};

	const std::map<std::string, TtextStyle> smallNormalStyles = {
		{ {"default"}, { "default","smallFnt",uiWhite } },
		{ {"reserved"}, { "reserved","smallFnt",gameReservedColour } },
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
		{ {"hotSelected"}, { "hotSelected","defaultFont",glm::vec4(0,0,1.0,1) } },
		{ {"mainBody"}, { "mainBody", "smallFnt", uiWhite } },
		{ {"mainHeader"}, { "mainHeader","smallHeaderFnt",uiWhite } }
	};





	const TtextTheme gameTheme = { "gameTheme", { gameDefaultStyles} };

	const glm::vec4 gameBackColour = { 0,0,0,1 };
	const glm::vec4 gameBorderColour = { 1,1,1,1 };




	const std::map<std::string, TtextTheme> themes2 = {
		{ {"defaultTheme"}, {defaultTheme} },
		{ {"gameNormal"}, {gameNormalTheme} },
		{ {"smallNormal"}, {smallNormalTheme} },
		{ {"gameTheme"}, {gameTheme} }
	};

	const int mainWinCtrlBorder = 20; ///<Indent around main window for placing controls

	const int defPopupW = 300; ///<Defence details popup width;
	const int defPopupH = 125; ///<Defence details popup width;

	const glm::i32vec2 gameWinCtrBorder = { 10,10 };

	const glm::i32vec2 powerQueueWinSize = { 120,500 };
	const glm::i32vec2 combatLogWinSize = { 180,500 };


}

/* A text style specifies what pre-rendered font to use (ie, typeset and size), 
	what colour to draw it in, and what this style is called. 

	A theme bundles together a collection of text styles. 
	
	A richText control is set to a theme. When it is asked to render 
	text in a named style, it looks up that style in the current theme.
	If it can find the style it will use it, if not it continues with
	the current style.
		In this way, if a user specifies 'bold' or 'hot text' and that
	style is supported by the theme, their text will rendered in the 
	appropriate typeface.
		For this reason, it is important that themes include a style
	called 'default', because after a theme change this is the style
	the richText control will attempt to render in. If it does not find
	it, it will keep rendering in the default style of the previous theme.
	
	
	*/













