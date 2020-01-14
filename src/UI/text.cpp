#include "text.h"

TCharacterPos::TCharacterPos(int textObj, int textPos) {
	setPos(textObj, textPos);
}

void TCharacterPos::setPos(int textObj, int textPos) {
	this->textObj = textObj;
	this->pos = textPos;
	unset = false;

}

bool TCharacterPos::operator<(const TCharacterPos& rhs) {
	return textObj < rhs.textObj || (textObj == rhs.textObj && pos < rhs.pos);
}

bool TCharacterPos::operator>(const TCharacterPos& rhs) {
	return textObj > rhs.textObj || (textObj == rhs.textObj && pos > rhs.pos);
}
