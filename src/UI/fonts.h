#pragma once

#include <string>
#include <fstream>


#include "font.h"

namespace fnt {
	CFont* loadFromFile(const std::string& filename, const std::string& name);
	CFont* loadFromStream(std::istream& input, const std::string& name);
	CFont* get(const std::string& name);







}