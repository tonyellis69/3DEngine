#include "CImRendr.h"

#include "renderer/renderer.h"
#include "UI/fonts.h"

CImRendr::CImRendr() {
	font = fnt::get("sysFont");
}

/** Erase everything ready for another frame of drawing. */
void CImRendr::init() {
	srcBuf3v.clear();
	freeMem = 0;
	numDrawCmds = 0;
}

void CImRendr::draw(glm::mat4& mvp) {

	for (auto& it = drawTexts.begin(); it != drawTexts.end();) {
		if (it->second.use_count() < 2)
			it = drawTexts.erase(it);
		else
			it++;
	}

	vertBuf3v.storeVerts(srcBuf3v, 3);

	vertBuf3v.setVAO();
	for (unsigned int n = 0; n < numDrawCmds; n++) {
		auto& cmd = drawCmds[n];
		switch(cmd.cmdType) {
		case drwCmdDrawLine:
			renderer.setShader(cmd.shader);
			renderer.setShaderValue(0, 1, cmd.vec4_0);
			renderer.setShaderValue(5, 1, *pMatrix);
			renderer.setShaderValue(6, 1, cmd.vec2_0);

			glDrawArrays(GL_LINE_STRIP_ADJACENCY, cmd.vertStart, cmd.vertCount);
			break;

		case drwCmdText:
			cmd.drawText->draw();
			cmd.drawText.reset(); //don't want recycled cmds to have copies
			break;

		}
	}
	vertBuf3v.clearVAO();
	init();
}

void CImRendr::drawLine(const glm::vec3& a, const glm::vec3& b) {
	int cmd = getFreeCmd();

	drawCmds[cmd].cmdType = drwCmdDrawLine;
	drawCmds[cmd].shader = renderer.shaderList["lineModel"]->hShader;
	drawCmds[cmd].vec4_0 = drawColour;
	drawCmds[cmd].vec2_0 = { renderer.Width, renderer.Height };

	drawCmds[cmd].vertStart = freeMem;
	drawCmds[cmd].vertCount = 4;

	addVert(2.0f * a -b);
	addVert(a);
	addVert(b);
	addVert(2.0f * b - a);
}


void CImRendr::drawText(int x, int y, const std::string& text) {
	int cmd = getFreeCmd();
	drawCmds[cmd].cmdType = drwCmdText;

	auto it = drawTexts.find(text);
	if (it == drawTexts.end()) {
		drawCmds[cmd].drawText = std::make_shared<CDrawText>();
		drawTexts[text] = drawCmds[cmd].drawText;
	}
	else {
		drawCmds[cmd].drawText = it->second;
	}

	if (font != NULL)
		drawCmds[cmd].drawText->setFont(font);
	drawCmds[cmd].shader = renderer.texShader->getShaderHandle();
	drawCmds[cmd].vec4_0 = drawColour;


	

	drawCmds[cmd].drawText->drawText(x, y, text);

}

/** Add this vert to those we'll use this frame. */
void CImRendr::addVert(const glm::vec3& v) {
	srcBuf3v.push_back(v);
	freeMem += 1;
}

void CImRendr::setDrawColour(const glm::vec4& colour) {
	this->drawColour = colour;
}

void CImRendr::setMatrix(glm::mat4* matrix) {
	this->pMatrix = matrix;
}

void CImRendr::setFont(const std::string& fontName) {
	font = fnt::get(fontName);
}

int CImRendr::getFreeCmd() {
	if (numDrawCmds == drawCmds.size())
		drawCmds.resize(numDrawCmds + 1);

	return numDrawCmds++;
}
