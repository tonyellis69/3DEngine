#pragma once

#include <string>

#include <glm/glm.hpp>

/**Namespace library of funcs to use with immediate mode drawing. */
namespace imRendr {

	void drawLine(const glm::vec3& a, const glm::vec3& b);
	void setMatrix( glm::mat4* matrix);
	void setDrawColour(const glm::vec4& colour);
	void drawText(int x, int y, const std::string& text);
	void setFont(const std::string& fontName);


}