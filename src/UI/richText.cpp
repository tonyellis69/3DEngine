#include "richText.h"

CRichText::CRichText() {
	initialise();
}

/** Add text at the position of the write head. */
void CRichText::addText(std::string text) {
	while (!text.empty()) {
		auto [txtToAppend, remainingText, nextStyle] = getNextText(text);
		appendText(txtToAppend);
		appendStyle(nextStyle);
		text = remainingText;
	}

}

/** Set the read head to this position in the text. */
void CRichText::setReadPos(int pos) {
	int charCount = 0;
	for (unsigned int blockNo = 0; blockNo < textObjs.size(); blockNo++) { 
		if (charCount + textObjs[blockNo].text.length() > pos) {
			readHead.blockNo = blockNo;
			readHead.charNo = pos - charCount;
			endOfTextFlag = false;
			return;
		}
		charCount += textObjs[blockNo].text.length();
	}

	readHead = {0,0};
	endOfTextFlag = true;
}

/** Return the read head position in characters from the start of the body of text. */
int CRichText::getReadPos() {
	int pos = 0;
	for (int block = 0; block < readHead.blockNo; block++)
		pos += textObjs[block].text.length();

	return pos + readHead.charNo;
}

/** Return flag for reading to end of text.*/
bool CRichText::endOfText() {
	return endOfTextFlag;
}

bool CRichText::styleChange() {
	return styleChangeFlag;
}

/** Return the next appendable text and/or style change. */
std::tuple<std::string, std::string, TextStyle> CRichText::getNextText(const std::string& text) {
	std::string remainingText;
	std::string txtToAppend;
	TextStyle nextStyle;

	unsigned int appendableChars = text.find_first_of('\\');
	txtToAppend = text.substr(0, appendableChars);
	remainingText = text.substr(txtToAppend.length(), std::string::npos);

	std::tie(remainingText, nextStyle) = getNextStyle(remainingText);

	return { txtToAppend,remainingText,nextStyle };
}

/** Return the next style in the given string and any remaining text. */
std::tuple<std::string, TextStyle> CRichText::getNextStyle(const std::string& text) {
	TextStyle nextStyle = getStyleAt(writeHead);
	if (text.empty())
		return { "",nextStyle };

	std::string remainingText;

	//find the style
	if (text[1] == 'b') {
		nextStyle.bold = !nextStyle.bold;
		remainingText = text.substr(2);
	}
	else if (text[1] == 'h') {
		if (!nextStyle.hotText.empty()) {
			nextStyle = styleStack.top();
			styleStack.pop();
			remainingText = text.substr(2);
		}
		else {
			styleStack.push(nextStyle);
			auto start = text.find("{");
			auto end = text.find("}");
			nextStyle.hotText = text.substr(start+1, end - start -1);
			nextStyle.colour = { 0.9,0.9,0,1 }; //TO DO: temp! create official style
			remainingText = text.substr(end+1);
		}

	}

	return { remainingText,nextStyle };
}

/** Add text to the block at the current write point. */
void CRichText::appendText(const std::string& text) {
	textObjs[writeHead.blockNo].text += text;
	writeHead.charNo += text.length();
	textEmpty = false;
}

/** Start a new style block at the current write point. */
void CRichText::appendStyle(TextStyle& newStyle) {
	if (textObjs[writeHead.blockNo].style == newStyle)
		return;
	CStyleBlock newBlock;
	newBlock.style = newStyle;
	writeHead.blockNo++;
	textObjs.emplace(textObjs.begin() + writeHead.blockNo, newBlock);
	writeHead.charNo = 0;
}

/** Return the style at the given read/write head position.*/
TextStyle CRichText::getStyleAt(CRWhead& head) {
	return textObjs[head.blockNo].style;
}

void CRichText::initialise() {
	//Create a starter text obj.
	CStyleBlock starterObj;
	starterObj.style.fontName = "smallSysFont";
	textObjs.push_back(starterObj);
	readHead = { 0,0 };
	writeHead = { 0,0 };
}

/** Return start of previous paragraph*/
int CRichText::getPrevPara(int endPos) {
	CRWhead currentPos = readHead;

	setReadPos(endPos);
	int charCount = endPos;
	char c;
	while (charCount > 0 ) {
		c = readPrevChar();
		if (c == '\n' && charCount < endPos -1)
			break;
		charCount--;
	}

	readHead = currentPos;
	return charCount;
}

/** Set font storing for any subsequent text we receive. */
void CRichText::setWriteFont(const std::string& fontName) {
	TextStyle currentWriteStyle = getWriteStyle();
	currentWriteStyle.fontName = fontName;
	appendStyle(currentWriteStyle);
}

/** Set colour storing for any subsequent text we receive. */
void CRichText::setWriteColour(glm::vec4& colour) {
	TextStyle currentWriteStyle = getWriteStyle();
	currentWriteStyle.colour = colour;
	appendStyle(currentWriteStyle);
}

/** Remove all text and reset, ready for more. */
void CRichText::clear() {
	textObjs.clear();
	textEmpty = true;
	initialise();
}

/** If we can, return the character at the current read position and
	advance. If we're already at the end of the text, set the eot flag. */
char CRichText::readNextChar() {
	endOfTextFlag = false;
	styleChangeFlag = false;

	//special case: body of text empty
	if (readHead.blockNo == 0 && textObjs[0].text.empty()) {
		endOfTextFlag = true;
		return 0;
	}

	//otherwise, read head should *always* be at a character
	char c = textObjs[readHead.blockNo].text[readHead.charNo];
	if (readHead.charNo + 1 >= textObjs[readHead.blockNo].text.size()) {
		if (readHead.blockNo + 1 < textObjs.size()) {
			readHead.charNo = 0;
			readHead.blockNo++;
			styleChangeFlag = true;
		}
		else
			endOfTextFlag = true;
	}
	else
		readHead.charNo++;

	return c;
}

/** If it exists, return the character before the current read head position,
	otherwise set the startOfText flag. */
char CRichText::readPrevChar() {
	startOfTextFlag = false;

	if (readHead.blockNo == 0 && (textObjs[0].text.empty() || readHead.charNo == 0)) {
		endOfTextFlag = true;
		return 0;
	}

	if (readHead.charNo == 0) {
		readHead.blockNo--;
		readHead.charNo = textObjs[readHead.blockNo].text.size() - 1;
	}
	else
		readHead.charNo--;

	return textObjs[readHead.blockNo].text[readHead.charNo];
}

/**  Return the text style at the read position. */
TextStyle CRichText::getReadStyle() {
	return textObjs[readHead.blockNo].style;
}

/** Return the text style at the write position. */
TextStyle CRichText::getWriteStyle() {
	return textObjs[writeHead.blockNo].style;
}

/** Return the string at the given position. */
std::string CRichText::getStringAt(int pos, int length) {
	int charCount = 0;
	for (unsigned int blockNo = 0; blockNo < textObjs.size(); blockNo++) {
		if (charCount + textObjs[blockNo].text.length() > pos) {
			pos = pos - charCount;
			return textObjs[blockNo].text.substr(pos,length);
		}
		charCount += textObjs[blockNo].text.length();
	}

	return std::string();
}



bool TextStyle::operator == (const TextStyle& style2) const {
	return (fontName == style2.fontName &&
		colour == style2.colour &&
		bold == style2.bold &&
		hotText == style2.hotText);
}