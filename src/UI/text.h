
#pragma once

#include <string>
#include <vector>
#include <map>

#include "glm/glm.hpp"

/** Collection of text style structs and other stuff, used all over. */

struct TtextStyle {
	std::string name;
	std::string font;
	glm::vec4 colour;
	bool operator == (const TtextStyle& style) const {
		return this->font == style.font && this->colour == style.colour &&
			this->name == style.name;
	};
};


struct TtextTheme {
	std::string themeName;
	//std::vector<TtextStyle> styles;
	std::map<std::string, TtextStyle> styles;

};