#include "hexRenderer.h"

#include "../utils/log.h"

#include "UI/GUIstyleSheet.h"

#include <cmath>

#include <glm/gtc/matrix_transform.hpp>


CHexRenderer::CHexRenderer() : hexModel(6) {
	pRenderer = &CRenderer::getInstance();
	tmpCreateHexagonModel();


	createSolidHexModel();

	createLineShader();
	createHexShader();

	camera.setNearFar(0.1f, 1000.0f);
	camera.setPos(glm::vec3(0, -0, 12));
	cameraPitch = 45;
	camera.pitch(cameraPitch);
	followCam = false;
	screenScrollSpeed = 20;// 15.0f;

	floorplanLineColour = glm::vec4(0.3, 1, 0.3, 1);
	floorplanSpaceColour = glm::vec4(0.6431, 0.7412, 0.9882, 0.03);
	floorplanSpaceColour = glm::vec4(0, 0, 0, 1);
	floorplanSolidColour = glm::vec4(0, 0.65f, 0, 1);
	floorplanSolidColour = glm::vec4(0, 0.47f, 0.16f, 1);
	solidHex = &lineModels["solidHex"];
}


void CHexRenderer::setMap(CHexArray* hexArray){
	this->hexArray = hexArray;
	
	fillFloorplanLineBuffer();
	fillFloorplanSolidBuffer(floorplanSolidBuf, 2, 1);
	fillFloorplanSolidBuffer(floorplanSpaceBuf, 1, 0.9f);
}

void CHexRenderer::draw() {
	drawFloorPlan();
}


void CHexRenderer::drawFog() {
	return;
	glm::mat4 mvp = camera.clipMatrix;
	pRenderer->setShader(lineShader);
	lineShader->setShaderValue(hMVP, mvp);

	glDisable(GL_DEPTH_TEST);
	TModelNode& node = solidHex->model;
	glm::mat4 scaleM = glm::scale(glm::mat4(1), glm::vec3(1.05f));
	for (int x = 0; x < hexArray->width; x++) {
		for (int y = 0; y < hexArray->height; y++) {
			if (hexArray->getHexOffset(x, y).fogged) {
				CHex hex = hexArray->indexToCube(x, y);
				glm::mat4 worldM = glm::translate(glm::mat4(1), hexArray->getWorldPos(hex));		
				worldM = worldM * scaleM;
				node.meshes[0].colour = glm::vec4(0, 0, 0, 1); //style::uialmostBlack; // glm::vec4(0, 0, 1, 1);
				drawNode2(node, worldM, solidHex->buffer2);



			}
		}
	}





	glEnable(GL_DEPTH_TEST);
}


void CHexRenderer::drawFloorPlan() {
	
	glm::mat4 mvp = camera.clipMatrix;
	pRenderer->setShader(lineShader);
	lineShader->setShaderValue(hMVP, mvp);
	
	glDisable(GL_DEPTH_TEST);
	//draw inner hex for empty hexes
	lineShader->setShaderValue(hColour, floorplanSpaceColour);
	//pRenderer->drawTriStripBuf(floorplanSpaceBuf);

	//draw filled hexes for solid hexes
	lineShader->setShaderValue(hColour, floorplanSolidColour);
	//pRenderer->drawTriStripBuf(floorplanSolidBuf);

	//draw hex wireframe grid
//	lineShader->setShaderValue(hColour, floorplanLineColour);
//	pRenderer->drawLineStripBuf(floorplanLineBuf);



	//!!!!!!!!!!!!!!!!!!temp hardcoding
	pRenderer->setShader(hexShader);
	hexShader->setShaderValue(hHexMVP, mvp);
	hexShader->setShaderValue(hGridSize, glm::i32vec2(hexArray->width, hexArray->height));
	hexShader->setShaderValue(hViewPort, camera.getView());
	pRenderer->drawPointsBuf(hexShaderBuf, 0, hexShaderBuf.numElements);


	glEnable(GL_DEPTH_TEST);
}


void CHexRenderer::drawPath(THexList* path, glm::vec4& pathStartColour, glm::vec4& pathEndColour) {
	pRenderer->setShader(lineShader);
	glm::mat4 mvp(1);
	float inc = 1.0 / path->size();  float t = 0;
	for (auto hex : *path) {
		glm::mat4 worldPos = glm::translate(glm::mat4(1), hexArray->getWorldPos(hex));
		mvp = camera.clipMatrix * worldPos;
		lineShader->setShaderValue(hMVP, mvp);
		glm::vec4 pathColour = glm::mix(pathStartColour, pathEndColour, 1 - t);
		lineShader->setShaderValue(hColour, pathColour);
		pRenderer->drawTriStripBuf(solidHexBuf);
		t += inc;
	}
}


void CHexRenderer::drawLineModel(CLineModel& lineModel) {
	TModelNode& node = lineModel.model;
	drawNode2(node, glm::mat4(1), lineModel.buffer2);
}

void CHexRenderer::drawNode(TModelNode& node, glm::mat4& parentMatrix, CBuf* buf) {
	glm::mat4 mvp = camera.clipMatrix * node.matrix *parentMatrix;// **drawData.worldMatrix;
	lineShader->setShaderValue(hMVP, mvp);

	for (auto mesh : node.meshes) {
		lineShader->setShaderValue(hColour, mesh.colour);
		pRenderer->drawLinesRange(mesh.indexStart, mesh.indexSize, *buf);
	}

	for (auto subNode : node.subNodes)
		drawNode(subNode, node.matrix * parentMatrix, buf);

}

void CHexRenderer::drawNode2(TModelNode& node, glm::mat4& parentMatrix, CBuf2* buf) {
	pRenderer->setShader(lineShader);
	glm::mat4 mvp = camera.clipMatrix * node.matrix * parentMatrix;// **drawData.worldMatrix;
	lineShader->setShaderValue(hMVP, mvp);

	for (auto mesh : node.meshes) {
		lineShader->setShaderValue(hColour, mesh.colour);

		if (mesh.isLine) //TO DO: ugh, try to avoid
			pRenderer->drawLinesBuf(*buf, (void*)(mesh.indexStart * sizeof(unsigned short)), mesh.indexSize);
		else
			pRenderer->drawTrisBuf(*buf, (void*)(mesh.indexStart * sizeof(unsigned short)), mesh.indexSize);

	}

	for (auto subNode : node.subNodes)
		drawNode2(subNode, node.matrix * parentMatrix, buf);

}



/** Fill the floorplan line buffer with outline hexagons translated to worldspace. */
void CHexRenderer::fillFloorplanLineBuffer() {
	std::vector<glm::vec3> verts;
	std::vector<unsigned int> indices; 	std::vector<unsigned int> hexIndices;
	std::vector<glm::i32vec2> shaderVerts; ////!!!!!!!!!!Temp!!
	int index = 0; int vNum = 0; int hexIndex = 0;
	for (int y = 0; y < hexArray->height; y++) {
		for (int x= 0; x < hexArray->width; x++) {
			if (hexArray->getHexOffset(x, y).content != 0) {
				glm::vec3 pos;
				for (auto corner : hexModel) {
					pos = corner + hexArray->getHexOffset(x, y).position;
					verts.push_back(pos);
					indices.push_back(vNum++);
					index++;
			

				}
				shaderVerts.push_back({ x,y });
				indices.push_back(indices.back() - 5);
				indices.push_back(65535);
				hexIndices.push_back(hexIndex++);
			}
		}
	}

	floorplanLineBuf.storeVertexes((void*)verts.data(), sizeof(glm::vec3) * verts.size(), verts.size());
	floorplanLineBuf.storeIndex(indices.data(), indices.size());
	floorplanLineBuf.storeLayout(3, 0, 0, 0);

	hexShaderBuf.storeVerts(shaderVerts,hexIndices,2);
}


/** Fill the floorplan solid buffer with solid hexagon polys translated to worldspace. */
void CHexRenderer::fillFloorplanSolidBuffer(CBuf& buf, int drawValue, float scale) {
	std::vector<glm::vec3> verts;
	std::vector<unsigned int> indices;
	int vNum = 0;
	for (int x = 0; x < hexArray->width; x++) {
		for (int y = 0; y < hexArray->height; y++) {
			if (hexArray->getHexOffset(x, y).content == drawValue) {
				glm::vec3 pos;
				for (auto corner : hexModel) {
					pos = (corner * scale) + hexArray->getHexOffset(x, y).position;
					verts.push_back(pos);
					vNum++;
				}
				indices.push_back(vNum - 3);
				indices.push_back(vNum - 2);
				indices.push_back(vNum - 4);
				indices.push_back(vNum - 1);
				indices.push_back(vNum - 5);
				indices.push_back(vNum - 6);
				indices.push_back(65535);
			}
		}

	}

	buf.storeVertexes((void*)verts.data(), sizeof(glm::vec3) * verts.size(), verts.size());
	buf.storeIndex(indices.data(), indices.size());
	buf.storeLayout(3, 0, 0, 0);
}

void CHexRenderer::createSolidHexModel() {
	std::vector<glm::vec3> verts;
	std::vector<unsigned int> indices;
	int vNum = 0;
	for (auto corner : hexModel) {
		glm::vec3  pos = corner;
		verts.push_back(pos);
	}
	indices.push_back(3);
	indices.push_back(4);
	indices.push_back(2);
	indices.push_back(5);
	indices.push_back(1);
	indices.push_back(0);
	indices.push_back(65535);


	solidHexBuf.storeVertexes((void*)verts.data(), sizeof(glm::vec3)* verts.size(), verts.size());
	solidHexBuf.storeIndex(indices.data(), indices.size());
	solidHexBuf.storeLayout(3, 0, 0, 0);
}





void CHexRenderer::dollyCamera(float delta) {
	camera.dolly(delta);
}

void CHexRenderer::pitchCamera(float delta) {
	cameraPitch += delta;
	camera.pitch(delta);
}

void CHexRenderer::moveCamera(glm::vec3& move) {
	camera.translate(move);
}

void CHexRenderer::tmpCreateHexagonModel() {
	float radius = 1.0f;
	for (int corner = 0; corner < 6; corner++) {
		float angle = 60 * corner - 30.0f;
		angle = M_PI / 180 * angle;
		hexModel[corner] = glm::vec3(radius * cos(angle),
			radius * sin(angle),0.0f);
	}

}

void CHexRenderer::createLineShader() {
	lineShader = pRenderer->createShader("lineModel");
	hMVP = lineShader->getUniformHandle("mvpMatrix");
	hColour = lineShader->getUniformHandle("colour");
}

void CHexRenderer::createHexShader() {
	hexShader = pRenderer->createShader("hexShader");
	hHexMVP = hexShader->getUniformHandle("mvpMatrix");
	hGridSize = hexShader->getUniformHandle("gridSize");
	hViewPort = hexShader->getUniformHandle("viewPort");
}

void CHexRenderer::setCameraAspectRatio(glm::vec2 ratio) {
	camera.setAspectRatio(ratio.x, ratio.y);
}

/** Return the hex coordinates for the given screen positon. */
CHex CHexRenderer::pickHex(int screenX, int screenY) {
	//convert to clip coordinates
	glm::vec2 screenSize = camera.getView();
	glm::vec4 clip( (2.0f * screenX) / screenSize.x - 1.0f, 1.0f - (2.0f * screenY) / screenSize.y, -1.0f, 1.0f);

	//undo perspective 
	glm::vec4 view = glm::inverse(camera.perspectiveMatrix) * clip;
	view.z = -1.0f; view.w = 0.0f;

	//undo view (camera position)
	glm::vec4 worldPos = camera.worldMatrix * view;
	glm::vec3 ray = glm::normalize(glm::vec3(worldPos));

	//this should be a ray, projecting from 0,0,0 in the given direction.

	glm::vec3 p = castFromCamToHexPlane(ray);

	CHex hexPos = worldSpaceToHex(p);

	//liveLog << "\n" << hexPos.x << " " << hexPos.y << " " << hexPos.z;


	return hexPos;
}

void CHexRenderer::loadMesh(const std::string& name, const std::string& fileName) {
	importer.loadFile(fileName);
	
	//store vert buffer

	CBuf meshBuf;
	modelBuffers2.push_back(meshBuf);

	CBuf2 meshBuf2;
	modelBuffers.push_back(meshBuf2);

	importer.getSingleMesh().exportToBuffer(modelBuffers2.back());
	importer.getSingleMesh().exportToBuffer(modelBuffers.back());

	//store model
	TModelNode& model = importer.getMeshNodes();
	model.name = name;
	lineModels[name] = { model,&modelBuffers2.back(),&modelBuffers.back() };
	lineModels[name].setColourR(floorplanLineColour); //////temp!!!!!!!!!!!
}


CLineModel CHexRenderer::getLineModel(const std::string& name) {
	return lineModels[name];
}

/** Fill the structure used for drawing the path between player and cursor. */
void CHexRenderer::setCursorPath(CHex& playerPos, CHex& cursorPos) {
	cursorPath = *hexLine(playerPos, cursorPos);;
}

/** Set path directly. */
void CHexRenderer::setCursorPath(THexList& path) {
	cursorPath = path;
}


void CHexRenderer::toggleFollowCam() {
	followCam = !followCam;

	if (followCam) {
		//find camera vector to xy plane
		glm::vec3 camVector = camera.getTargetDir();


		//glm::vec3 planeN(0, 0, 1); //normal of plane on which hexes lie.
		//float d = 0; //distance of plane from origin
		//float t = -(glm::dot(camera.getPos(), planeN) + d)
		//	/ glm::dot(camVector, planeN);
		////t = distance from camera to plane for target vector

		//glm::vec3 p = camera.getPos() + camVector * t; //extend vector to find where it hits plane.

		followCamVec = camera.getPos() - castFromCamToHexPlane(camVector);
	}

}

/** Position the camera so that it's displaced from the given target by 
	whatever the followCamVec currently is. */
void CHexRenderer::followTarget(glm::vec3& target) {
	glm::vec3 newPos = target + followCamVec;
	camera.setPos(newPos);
}

/** Scroll the screen if the mouse is at the edge. */
void CHexRenderer::attemptScreenScroll(glm::i32vec2& mousePos, float dT) {
	//if (followCam)
	//	return;
	float xMove = dT * screenScrollSpeed;
	float yMove = xMove;
	glm::vec2 screenSize = camera.getView();

	glm::vec3 cameraMove(0);
	if (mousePos.x >= screenSize.x - 5 )
		cameraMove ={ xMove,0,0 };
	else if (mousePos.x < 5 )
		cameraMove = { -xMove,0,0 };

	if (mousePos.y >= screenSize.y - 5 )
		cameraMove = { 0,-yMove,0 };
	else if (mousePos.y < 5)
		cameraMove = { 0,yMove,0 };
	
	if (cameraMove.length() == 0)
		return;
	

	glm::vec3 camTarget = castFromCamToHexPlane(camera.getTargetDir());
	camTarget += cameraMove;

	glm::vec3 mapSize = hexArray->worldPosCornerDist;
	

	if (camTarget.x < -mapSize.x || camTarget.x > mapSize.x
		|| camTarget.y < -mapSize.y || camTarget.y > mapSize.y)
		return;

	moveCamera(cameraMove);
}

glm::vec3 CHexRenderer::castFromCamToHexPlane(glm::vec3& ray) {
	glm::vec3 planeN(0, 0, 1); //normal of plane on which hexes lie.
	float d = 0; //distance of plane from origin
	float t = -(glm::dot(camera.getPos(), planeN) + d)
		/ glm::dot(ray, planeN);
	//t = distance from camera to plane for this ray

	return camera.getPos() + ray * t; //extend ray to find where it hits plane.

}

/** Return the on screen position of a point in world space. */
glm::i32vec2 CHexRenderer::worldPosToScreen(glm::vec3& worldPos) {
	glm::vec4 translatedPos = camera.clipMatrix * glm::vec4(worldPos,1.0f);

	translatedPos /= translatedPos.w;

	translatedPos = (translatedPos + glm::vec4(1.0f)) * 0.5f;
	translatedPos.x *= camera.getView().x;
	translatedPos.y = (1.0f - translatedPos.y) * camera.getView().y;


	return translatedPos;
}

/** Highlight this hex. */
void CHexRenderer::highlightHex(CHex& hex) {
	glDisable(GL_DEPTH_TEST);
	//draw coloured filled hex
	TModelNode& node = solidHex->model;
	glm::mat4 worldM = glm::translate(glm::mat4(1), hexArray->getWorldPos(hex));
	node.meshes[0].colour = glm::vec4(0, 0, 0.8f, 1); 
	drawNode2(node, worldM, solidHex->buffer2);


	//draw smaller background colour hex
	glm::mat4 scaleM = glm::scale(glm::mat4(1), glm::vec3(0.9f));
	worldM = worldM * scaleM;
	node.meshes[0].colour =  floorplanSpaceColour;
	drawNode2(node, worldM, solidHex->buffer2);

	glEnable(GL_DEPTH_TEST);
}
