#include "CImRendr.h"

#include "renderer/renderer.h"

/** Erase everything ready for another frame of drawing. */
void CImRendr::init() {
	srcBuf3v.clear();
	freeMem = 0;
	drawCmds.clear();
}

void CImRendr::draw(glm::mat4& mvp) {

	vertBuf3v.storeVerts(srcBuf3v, 3);

	vertBuf3v.setVAO();
	for (auto& cmd : drawCmds) {
		if (cmd.cmdType == drwCmdDrawLine) {
			renderer.setShader(cmd.shader);
			renderer.setShaderValue(0, 1, cmd.vec4_0);
			renderer.setShaderValue(1,1, mvp);
			renderer.setShaderValue(2, 1, cmd.vec2_0);

			glDrawArrays(GL_LINE_STRIP_ADJACENCY, cmd.vertStart, cmd.vertCount);
			
		}



	}
	vertBuf3v.clearVAO();
	init();
}

void CImRendr::drawLine(const glm::vec3& a, const glm::vec3& b) {
	CDrawCmd drawCmd;

	drawCmd.cmdType = drwCmdDrawLine;
	drawCmd.shader = renderer.shaderList["lineModel"]->hShader;
	drawCmd.vec4_0 = drawColour;
	drawCmd.vec2_0 = { renderer.Width, renderer.Height };

	drawCmd.vertStart = freeMem;
	drawCmd.vertCount = 4;

	addVert(2.0f * a -b);
	addVert(a);
	addVert(b);
	addVert(2.0f * b - a);

	//add to list
	drawCmds.push_back(drawCmd);
}

/** Add this vert to those we'll use this frame. */
void CImRendr::addVert(const glm::vec3& v) {
	srcBuf3v.push_back(v);
	freeMem += 1;// sizeof(v);
}
