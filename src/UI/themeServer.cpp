#include "themeServer.h"


/**	Add a style to the named theme, creating the theme if necessary. */
void CThemeServer::addToTheme(const std::string& themeName, const TtextStyle& style) {
	//themes[themeName].styles.push_back(style);
	themes[themeName].styles[style.name] = style;
}

/** Return the named theme. */
TtextTheme& CThemeServer::getTheme(const std::string& themeName) {
	return themes[themeName];
}
