#include "fonts.h"

#include <map>
#include <memory>
#include <string>

std::map<std::string, std::shared_ptr<CFont> > fonts;

/** Load from file and store under the given name. */
CFont* fnt::loadFromFile(const std::string& filename, const std::string& name) {
	std::ifstream fontFile(filename, std::ios::binary);

	if (fontFile.fail() || fontFile.get() != 'F' || fontFile.get() != '0') {
		std::cerr << "\nNot a proper font file: " << filename;
		exit(EXIT_FAILURE);
	}

	CFont* font = loadFromStream(fontFile, name);
	fontFile.close();
	return font;
}

/** Load from stream and store under the given name. */
CFont* fnt::loadFromStream(std::istream& input, const std::string& name) {
	auto font = std::make_shared<CFont>();
	font->loadFromStream(input);
	font->name = name;
	fonts[name] = font;

	return font.get();
}

/** Return the named font, if found. */
CFont* fnt::get(const std::string& name) {
	auto f = fonts.find(name);
	if (f == fonts.end())
		return nullptr;
	else
		return f->second.get();
}
