#pragma once

#include <vector>
#include <string>
#include <string_view>

#include "glm/glm.hpp"

struct TextStyle {
	bool operator == (const TextStyle& style2) const;
	std::string_view fontName;
	glm::vec4 colour = { 1,1,1,1 };
	bool bold = false;
	//more to follow
};

class CStyleBlock;

struct TFragment2 {
	TextStyle style;
	std::string text;
	int pixelHeight;
	bool causesNewline = false;
};

struct CRWhead {
public:
//	CRWhead()  {}
	int blockNo = 0;
	int charNo = 0;
};

/** Maintains a body of text that can incorporate multiple
	fonts, colours and other style features. */
class CRichText {
public:
	CRichText();
	void addText(std::string text);
	void setReadPos(int pos);
	int getReadPos();
	bool endOfText();
	bool styleChange();
	char readNextChar();
	TextStyle getReadStyle();
	std::string getStringAt(int pos, int length);

private:
	std::tuple<std::string, std::string, TextStyle>getNextText(const std::string& text);
	std::tuple<std::string, TextStyle>getNextStyle(const std::string& text);
	void appendText(const std::string& text);
	void appendStyle(TextStyle& newStyle);
	TextStyle getStyleAt(CRWhead& head);


public: //temp! for debugging
	std::vector<CStyleBlock> textObjs;
	std::vector<std::string> fontNames;

	CRWhead readHead;
	CRWhead writeHead;

	bool endOfTextFlag = false; ///<read to end of text flag
	bool styleChangeFlag = false;
};



class CStyleBlock {
public:
	CStyleBlock() {};
	TextStyle style;

	std::string text;
};


