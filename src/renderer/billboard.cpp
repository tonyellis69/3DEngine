

#include "billboard.h"
#include "..\vertBufs.h"
#include "renderer.h"

CBillboard::CBillboard(glm::vec3 & pos, glm::vec2 & size) {
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
	
	unsigned short index[6] = { 0,2,3,0,3,1 };

	storeVertexes(vert, sizeof(vert), 4);
	storeIndex(index, sizeof(index), 6);
	storeLayout(3, 2, 0, 0);
}

void CBillboard::drawNew() {
	material->assign();

	CBillboardShader* billboardShader = (CBillboardShader*) material->getShader();
	billboardShader->setCentrePos(getPos());
	billboardShader->setCamWorldMatrix(pRenderer->currentCamera->worldMatrix);
	billboardShader->setBillboardSize(size.x);
	
	pRenderer->drawModel(*this);

	material->unAssign();
}
