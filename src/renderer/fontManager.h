#pragma once

#include <map>
#include <string>
#include <istream>

#include "..\UI\font.h"

/** A simple class for providing access to fonts. */


class CFontManager {
public:
	CFontManager() {};
	~CFontManager();
	CFont& createFromFile(std::string name, std::string filename);
	CFont& createFromStream(std::string name, std::istream& input);
	CFont& getFont(std::string name);



private:
	std::map<std::string,CFont*> fonts;


};
