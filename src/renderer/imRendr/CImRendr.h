#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "buf2.h"

#include "drawCmd.h"

class CImRendr {
public:
	void init();
	void draw(glm::mat4& mvp);
	void drawLine(const glm::vec3& a, const glm::vec3& b);
	void addVert(const glm::vec3& v);
	void setDrawColour(const glm::vec4& colour);
	void setMatrix(glm::mat4* matrix);


private:
	int getFreeCmd();
	CBuf2 vertBuf3v;
	std::vector<glm::vec3> srcBuf3v;
	int freeMem = 0;

	std::vector<CDrawCmd> drawCmds;
	int numDrawCmds = 0;


	glm::vec4 drawColour = { 1,1,1,1 };
	glm::mat4* pMatrix;
};