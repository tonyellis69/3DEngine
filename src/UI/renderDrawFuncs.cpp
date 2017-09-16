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
	uiRectShader->create(pRenderer->dataPath + "uiRect");
	uiRectShader->getShaderHandles();
	uiRectShader->setType(uiRect);
	shaderList.push_back(uiRectShader);

	uiTexShader = new CGUItexShader();
	uiTexShader->create(pRenderer->dataPath + "uiTexture");
	uiTexShader->getShaderHandles();
	uiTexShader->setType(uiTex);
	shaderList.push_back(uiTexShader);

}



/** Register this control with the uiEngine, storing its details for drawing. */
void CRenderDrawFuncs::registerControl(CGUIbetterBase & control) {
	
	quadBufs[control.uniqueID].border = (CBuf*)pRenderer->createBuffer();
	quadBufs[control.uniqueID].rect = (CBuf*)pRenderer->createBuffer();
	//A B
	//C D
	vBuf::T2DtexVert corners[4];
	corners[0].v = vec2(control.drawBox.pos); //A
	corners[1].v = vec2(control.drawBox.pos + i32vec2(control.drawBox.size.x, 0)); //B
	corners[2].v = vec2(control.drawBox.pos + i32vec2(0, control.drawBox.size.y)); //C
	corners[3].v = vec2(control.drawBox.pos + control.drawBox.size); //D
	corners[0].tex = vec2(0);
	corners[1].tex = vec2(1,0);
	corners[2].tex = vec2(0,1);
	corners[3].tex = vec2(1,1);

	unsigned int index[4] = { 2,3,0,1 };
	quadBufs[control.uniqueID].rect->storeVertexes(corners, sizeof(corners), 4);
	quadBufs[control.uniqueID].rect->storeIndex(index,  4);
	quadBufs[control.uniqueID].rect->storeLayout(2, 2, 0, 0);

	unsigned int index2 [4] = { 1,0,2,3 };
	quadBufs[control.uniqueID].border->storeVertexes(corners, sizeof(corners), 4);
	quadBufs[control.uniqueID].border->storeIndex(index2,  4);
	quadBufs[control.uniqueID].border->storeLayout(2, 2,0, 0);
}

/** Draw the drawBox of this control. */
void CRenderDrawFuncs::drawCtrlRect(CGUIbetterBase & control) {
	CBuf* buf = quadBufs[control.uniqueID].rect;
	pRenderer->setShader(uiRectShader);
	uiRectShader->setColour1((vec4&)control.backColour1);
	uiRectShader->setColour2((vec4&)control.backColour2);
	uiRectShader->setOrtho(orthoView);
	pRenderer->drawBuf(*buf, drawTriStrip);
	pRenderer->setShader(0);
}

void CRenderDrawFuncs::drawCtrlBorder(CGUIbetterBase & control) {
	CBuf* buf = quadBufs[control.uniqueID].border;
	pRenderer->setShader(uiRectShader);
	uiRectShader->setColour1((vec4&)control.borderColour);
	uiRectShader->setColour2((vec4&)control.borderColour);
	uiRectShader->setOrtho(orthoView);
	pRenderer->drawBuf(*buf, drawLineLoop);
	pRenderer->setShader(0);

}

void CRenderDrawFuncs::setScreenSize(int width, int height) {
	mat4 flip;
	flip[1].y = -1;
	flip[2].y = float(height);
	orthoView = flip * glm::ortho<float>(0.1f, (float)width, 0.1f, (float)height) ;

	screenWidth = width; screenHeight = height;
}

unsigned int CRenderDrawFuncs::getTextureHandle(const std::string & textureName) {
	CRenderTexture* renTex = (CRenderTexture*)pRenderer->textureManager.getTexture(textureName);
	return renTex->handle;
}

void CRenderDrawFuncs::drawTexture(CGUIbetterBase & control, CBaseTexture& texture) {
	CBuf* buf = quadBufs[control.uniqueID].rect;
	//set shader
	pRenderer->setShader(uiTexShader);
	//pass the texture
	CRenderTexture* rendTex = (CRenderTexture*)&texture;
	pRenderer->attachTexture(0, texture);

	uiTexShader->setTextureUnit(0);
	uiTexShader->setTiling(vec2(1,1));
	uiTexShader->setOffset(vec2(0));
	uiTexShader->setOrtho(orthoView);
	pRenderer->drawBuf(*buf, drawTriStrip);
	//pRenderer->attachTexture(0, 0);

}

CRenderDrawFuncs::~CRenderDrawFuncs() {
	for (size_t s = 0; s < shaderList.size(); s++) {
		delete shaderList[s];

	}
}

