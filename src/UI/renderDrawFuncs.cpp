#include "renderDrawFuncs.h"
#include "..\vertBufs.h"

#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtc\matrix_access.hpp"

using namespace glm;


void CRenderDrawFuncs::setRenderer(CRenderer * renderer) {
	pRenderer = renderer;
}

void CRenderDrawFuncs::loadShaders() {
	uiRectShader = new CGUIrectShader();
	uiRectShader->pRenderer = pRenderer;
	uiRectShader->create(pRenderer->dataPath + "uiRect");
	uiRectShader->getShaderHandles();
	uiRectShader->setType(uiRect);
	shaderList.push_back(uiRectShader);
}



/** Register this control with the uiEngine, storing its details for drawing. */
void CRenderDrawFuncs::registerControl(CGUIbetterBase & control) {
	CBuf newBuf;
	quadBufs[control.uniqueID] = newBuf;
	//A B
	//C D
	vBuf::T3Dvert corners[4];
	corners[0].v = vec3(control.drawBox.pos,0); //A
	corners[1].v = vec3(control.drawBox.pos + i32vec2(control.drawBox.size.x, 0),0); //B
	corners[2].v = vec3(control.drawBox.pos + i32vec2(0, control.drawBox.size.y),0); //C
	corners[3].v = vec3(control.drawBox.pos + control.drawBox.size,0); //D


	corners[0].colour = (vec4&)control.backColour1;
	corners[1].colour = (vec4&)control.backColour1;
	corners[2].colour = (vec4&)control.backColour2;
	corners[3].colour = (vec4&)control.backColour2;


	unsigned short index[6] = { 0,2,3,0,3,1 };

	quadBufs[control.uniqueID].storeVertexes(corners, sizeof(corners), 4);
	quadBufs[control.uniqueID].storeIndex(index, sizeof(index), 6);
	quadBufs[control.uniqueID].storeLayout(3, 4, 3, 0);

}

/** Draw the drawBox of this control. */
void CRenderDrawFuncs::drawCtrlRect(CGUIbetterBase & control) {
	//find buffer
	CBuf* buf = &quadBufs[control.uniqueID];
	
	//set up the shader
	pRenderer->setShader(uiRectShader);

	uiRectShader->setMVP(orthoView);

	//draw our rectangle
	pRenderer->drawBuf(*buf, drawTris);
	pRenderer->setShader(0);
}

void CRenderDrawFuncs::setScreenSize(int width, int height) {
	mat4 flip;
	flip[1].y = -1;
	flip[2].y = height;
	orthoView = flip * glm::ortho<float>(0, width, 0, height) ;

	screenWidth = width; screenHeight = height;
}

CRenderDrawFuncs::~CRenderDrawFuncs() {
	for (size_t s = 0; s < shaderList.size(); s++) {
		delete shaderList[s];

	}
}

