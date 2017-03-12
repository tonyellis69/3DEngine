#include "TexFont.h"

#define NULL 0

CTexFont::CTexFont(void){ 
	Glyphs = NULL;
}

CTexFont::~CTexFont(void) {
	if (Glyphs != NULL)
		delete[] Glyphs;
}
