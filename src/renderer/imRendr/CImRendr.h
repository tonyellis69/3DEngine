#pragma once

#include <vector>
#include <unordered_map>

#include <glm/glm.hpp>

#include "buf2.h"

#include "drawCmd.h"

class CImRendr {
public:
	CImRendr();
	void init();
	void draw(glm::mat4& mvp);
	void drawLine(const glm::vec3& a, const glm::vec3& b);
	void drawText(int x, int y, const std::string& text);
	void addVert(const glm::vec3& v);
	void setDrawColour(const glm::vec4& colour);
	void setMatrix(glm::mat4* matrix);
	void setFont(const std::string& fontName);


private:
	int getFreeCmd();
	CBuf2 vertBuf3v;
	std::vector<glm::vec3> srcBuf3v;
	int freeMem = 0;

	std::vector<CDrawCmd> drawCmds;
	int numDrawCmds = 0;


	glm::vec4 drawColour = { 1,1,1,1 };
	glm::mat4* pMatrix;
	CFont* font;

	std::unordered_map<std::string, std::shared_ptr<CDrawText>> drawTexts;
};