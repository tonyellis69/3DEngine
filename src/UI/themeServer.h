#pragma once

#include <map>

#include "GUIrichText.h" //for TtextTheme

/** A class defining an object that looks after a list of GUI themes, and can search
	for a named one. */

class CThemeServer {
public:
	CThemeServer() {};
	void addToTheme(const std::string& themeName, const TtextStyle& style);
	TtextTheme& getTheme(const std::string& themeName);

private:
	std::map<std::string,TtextTheme> themes;
};