#pragma once

#include "Types.h"

struct Glyph
  {
    float tex_x1, tex_y1, tex_x2;
    size_t advance;
	TRect Rect;
  };

// This is how glyphs are stored in the file.
struct Glyph_Buffer
{
  unsigned char ascii, width;
  unsigned short x, y;
};

class CTexFont
{
public:
	CTexFont(void);
	~CTexFont(void);


	Glyph* Glyphs;
	Glyph* table[256];
	int textureHandle; //TO DO: scrap, font should be renderer agnostic.
	int textureNo;
	int ID;
};
