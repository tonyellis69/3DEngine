#include "font.h"

#include <fstream>
#include <string>
#include <iostream>

// Helper function to read a piece of data from a stream.
template<class T, class S>
void readObject(T& to_read, S& in) {
	in.read(reinterpret_cast<char*>(&to_read), sizeof(T));
}

CFont::CFont() {
	for (int f = 0; f < 256; f++)
		table[f] = NULL;
}

/** Create font from the given stream. */
void CFont::loadFromStream(std::istream & input) {

	int width, height, nChars;
	float texLineHeight;
	readObject(width, input);
	readObject(height, input);
	readObject(lineHeight, input);
	readObject(nChars, input);
	texLineHeight = static_cast<float>(lineHeight) / height;
	//lineHeight /= 2;

	// Read every glyph, store it in the glyph array and set the right
	// pointer in the table.
	glyphs.resize(nChars);
	glyphBuffer buffer;
	for (int i = 0; i < nChars; ++i) {
		readObject(buffer, input);
		glyphs[i].u = static_cast<float>(buffer.x) / width;
		glyphs[i].s = static_cast<float>(buffer.x + buffer.width) / width;
		glyphs[i].t = static_cast<float>(buffer.y) / height;
		glyphs[i].v = glyphs[i].t + texLineHeight;

		glyphs[i].width = buffer.width  ;// / 2;
		glyphs[i].height = lineHeight;
		//Font->Glyphs[i].Rect.originX = 0;
		//Font->Glyphs[i].Rect.originY = (float)line_height;

		table[buffer.ascii] = &glyphs[i];
		//TO DO use index rather than pointer to memory
	};


	//Point chars with no glyph to point to the default glyph.
	glyph* defaultGlyph = table[(unsigned char)'\xFF'];
	if (defaultGlyph == NULL) {
		std::cerr << "\nNo default character found in font file.";
		exit(EXIT_FAILURE);
	}
	for (int i = 0; i != 256; ++i) {
		if (table[i] == NULL)
			table[i] = defaultGlyph;
	}

	//read font texture
	/*unsigned char* RGBA = new unsigned char[width * height * 4];
	char c; int t;
	for (int h = 0; h<height; h++) {
		for (int w = 0; w<width; w++) {
			input.read(&c, 1);
			t = ((h*width) + w) * 4;
			RGBA[t] = RGBA[t + 1] = RGBA[t + 2] = 255;
			RGBA[t + 3] = c;
		}
	}

	//turn it into an alpha texture
	texture.createRGBA(RGBA, width, height);
	delete[] RGBA;*/

	unsigned char* data = new unsigned char[width * height];
	char c; int t;
	for (int h = 0; h<height; h++) {
		for (int w = 0; w<width; w++) {
			input.read(&c, 1);
			t = ((h*width) + w);
			data[t] = c;
		}
	}

	//turn it into an alpha texture
	texture.createGreyscale(data, width, height);
	delete[] data; 

}

void CFont::loadFromFile(std::string filename) {
	std::ifstream fontFile(filename,  std::ios::binary);
	
	if (fontFile.fail() || fontFile.get() != 'F' || fontFile.get() != '0') {
		std::cerr << "\nNot a proper font file: " << filename;
		exit(EXIT_FAILURE);
	}

	loadFromStream(fontFile);
	fontFile.close();
}
