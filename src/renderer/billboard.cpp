

#include "billboard.h"
#include "..\vertBufs.h"
#include "renderer.h"

CBillboard::CBillboard(glm::vec3 & pos, glm::vec2 & size) {
	//pRenderer = renderer; //TO DO: ugh
	//buf.setRenderer(renderer); 
	setPos(pos);
	setSize(size);
}

void CBillboard::setSize(glm::vec2 & size) {
	CBillboard::size = size;
	vBuf::T3DtexVert vert[4];

	float halfWidth = size.x * 0.5f;
	float halfHeight = size.y * 0.5f;

	vert[0].v = glm::vec3(-halfWidth, halfHeight, 0); vert[0].tex = glm::vec2(0, 1);
	vert[1].v = glm::vec3(halfWidth, halfHeight, 0); vert[1].tex = glm::vec2(1, 1);
	vert[2].v = glm::vec3(-halfWidth, -halfHeight, 0); vert[2].tex = glm::vec2(0, 0);
	vert[3].v = glm::vec3(halfWidth, -halfHeight, 0); vert[3].tex = glm::vec2(1, 0);
	
	unsigned int index[6] = { 0,2,3,0,3,1 };

	storeVertexes(vert, sizeof(vBuf::T3DtexVert), 4);
	storeIndex(index,  6);
	storeLayout(3, 2, 0, 0);
}

void CBillboard::drawNew() {
	material->assign();

	CShader* billboardShader = (CRenderShader*)pRenderer->billboardShader;
	
	billboardShader->setShaderValue(pRenderer->hBillCentre,getPos());
	//billboardShader->setShaderValue(pRenderer->hBillMVP, pRenderer->currentCamera->worldMatrix);
	billboardShader->setShaderValue(pRenderer->hBillboardSize, size.x);
	
	pRenderer->drawModel(*this);

	material->unAssign();
}
