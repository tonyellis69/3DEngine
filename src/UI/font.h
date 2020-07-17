#pragma once

//#include <fstream>
#include <iostream>
#include <vector>



#include "..\renderer\renderTexture.h"

struct glyph {
	float u,v, s,t;
	int width;
	int height;
	size_t advance;
};

// This is how glyphs are stored in the file.
struct glyphBuffer {
	unsigned char ascii, width;
	unsigned short x, y;
};

/** A class for maintaining font data. */
class CFont {
public:
	CFont();
	void loadFromStream(std::istream &input);
	void loadFromFile(std::string filename);
	
	int lineHeight;
	std::vector<glyph> glyphs;
	glyph* table[256];
	CRenderTexture texture;

	std::string name;
};