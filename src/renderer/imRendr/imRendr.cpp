#include "imRendr.h"

#include "renderer/renderer.h"

void imRendr::drawLine(const glm::vec3& a, const glm::vec3& b) {
	renderer.imRendr.drawLine(a, b);

}

void imRendr::setMatrix(glm::mat4* matrix) {
	renderer.imRendr.setMatrix(matrix);
}

void imRendr::setDrawColour(const glm::vec4& colour) {
	renderer.imRendr.setDrawColour(colour);
}

void imRendr::drawText(int x, int y, const std::string& text) {
	renderer.imRendr.drawText(x, y, text);
}

void imRendr::setFont(const std::string& fontName) {
	renderer.imRendr.setFont(fontName);
}