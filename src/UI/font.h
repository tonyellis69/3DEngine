#pragma once

//#include <fstream>
#include <iostream>
#include <vector>

#include "..\renderer\renderTexture.h"

struct glyph {
	float u,v, s,t;
	float width;
	float height;
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
	
	int lineHeight;
	std::vector<glyph> glyphs;
	glyph* table[256];
	CRenderTexture texture;
};