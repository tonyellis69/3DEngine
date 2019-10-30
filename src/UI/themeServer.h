#pragma once

#include <map>
#include <vector>

#include "text.h" //for TtextTheme

/** A class defining an object that looks after a list of GUI themes, and can search
	for a named one. */

class CThemeServer {
public:
	CThemeServer() {};
	//CThemeServer(std::map<std::string, TtextTheme> newThemes) : themes  { { newThemes} } {};
	CThemeServer(std::vector<TtextTheme>& newThemes) {
		for (auto theme : newThemes)
			themes[theme.themeName] = theme;
	};
	void addToTheme(const std::string& themeName, const TtextStyle& style);
	TtextTheme& getTheme(const std::string& themeName);


	std::map<std::string,TtextTheme> themes;
};