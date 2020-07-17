#pragma once

#define _USE_MATH_DEFINES //for cmath
#include <vector>

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"

#include "../renderer/renderer.h"
#include "../renderer/buf.h"
#include "../importer/importer.h"
#include "../hex/hexArray.h"
#include "../hex/hexObject.h"
#include "../hex/hex.h"
#include "../hex/IHexRenderer.h"

#include "hex/lineModel.h"




/**	A class for drawing 3D hex-based graphics. */
class CHexRenderer: public IHexRenderer {
public:
	CHexRenderer();
	void start();
	void setMap(CHexArray* hexArray);
	void draw();
	void drawPath(THexList* path, glm::vec4& pathStartColour, glm::vec4& pathEndColour);
	void dollyCamera(float delta);
	void pitchCamera(float delta);
	void moveCamera(glm::vec3& move);
	void setCameraAspectRatio(glm::vec2 ratio);
	CHex pickHex(int screenX, int screenY);
	void loadMesh(const std::string& name, const std::string& fileName);
	CLineModel getLineModel(const std::string& name);
	void setCursorPath(CHex& playerPos, CHex& cursorPos);
	void setCursorPath(THexList& path);
	THexList& getCursorPath() { return cursorPath; }
	void toggleFollowCam();
	bool following() {
		return followCam;
	}

	void followTarget(glm::vec3& target);
	void attemptScreenScroll(glm::i32vec2& mousePos, float dT);

	glm::vec3 castRay(glm::vec3& ray);

	glm::i32vec2 worldPosToScreen(glm::vec3& worldPos);

	void highlightHex(CHex& hex);

private:
	void tmpCreateHexagonModel();

	void fillFloorplanLineBuffer();
	void fillFloorplanSolidBuffer(CBuf& buf, int drawValue, float scale);
	void createSolidHexModel();
	void createLineShader();
	void drawFloorPlan();
	void drawLineModel(CLineModel& lineModel);

	void drawNode(TModelNode& node, glm::mat4& parentMatrix, CBuf* buf);

	void drawNode2(TModelNode& node, glm::mat4& parentMatrix, CBuf2* buf);

	void drawFov(TFov& fov, CHex& origin);

	CRenderer* pRenderer;
	CBuf floorplanLineBuf;
	CBuf floorplanSpaceBuf;
	CBuf floorplanSolidBuf;

	CBuf solidHexBuf;
	std::vector<glm::vec3> hexModel;

	CLineModel* solidHex;

	CShader* lineShader;
	unsigned int hMVP;
	unsigned int hColour;

	glm::vec4 floorplanLineColour;
	glm::vec4 floorplanSpaceColour;
	glm::vec4 floorplanSolidColour;

	CCamera camera;
	float cameraPitch;
	bool followCam;
	glm::vec3 followCamVec; ///<Vector from target point on plane to camera
	float screenScrollSpeed;

	CHexArray* hexArray;

	std::list<CBuf2> modelBuffers;
	std::list<CBuf> modelBuffers2;

	std::map<std::string, CLineModel> lineModels;


	THexList cursorPath;

	CImporter importer;
};




