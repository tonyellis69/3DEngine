#include "fontManager.h"





/** Create a new font from a file and store it. */
CFont& CFontManager::createFromFile(std::string name, std::string filename) {
	CFont* font = new CFont();
	font->loadFromFile(filename);
	font->name = name;
	fonts[name] = font;
	return *font;
}

CFont & CFontManager::createFromStream(std::string name, std::istream & input) {
	CFont* font = new CFont();
	font->loadFromStream(input);
	font->name = name;
	fonts[name] = font;
	return *font;
}

CFont & CFontManager::getFont(std::string name) {
	for (auto font : fonts) {
		if (font.first == name)
			return *font.second;
	}
	//Hope we have at least one font installed
	return *fonts.begin()->second;  
}



CFontManager::~CFontManager() {
	for (auto font : fonts) {
		delete font.second;
	}
}
