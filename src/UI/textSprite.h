#pragma once

#include <string>
#include <memory>

#include <glm/glm.hpp>

#include "font.h"
#include "../renderer/buf.h"
#include "../renderer/shader.h"

struct TTextSpriteShader {
	unsigned int hOrthoMatrix;
	unsigned int hTextureUnit;
	unsigned int hOffset;
	unsigned int hSize;
	CShader* shader;
};

class ITextSpriteCallback;
/** Defines a rectangular block of pre-rendered text. */
class CTextSprite {
public:
	CTextSprite();
	~CTextSprite();
	virtual void createTextImage(CRenderTexture& texture);
	void setTextObjData(int textObj, int textPos, int textLength);
	void setPagePosition(int posX, int posY);
	virtual void setTextColour(glm::vec4& colour);

	void makeTextQuads(const std::string& text, CFont* font);
	void renderTextQuads(CRenderTexture& buf, int storageId, glm::vec4& colour);
	virtual void write2ndTextToBuffer(CRenderTexture& buf, glm::vec4& colour) {};
	void setCallbackObj(ITextSpriteCallback* obj) {
		callbackObj = obj;
	}
	void setPageTexture(CRenderTexture* pageBuf);
	void setPageOthoMatrix(glm::mat4* pageOrthoViewMatrix);
	void setShader(TTextSpriteShader* shader);
	virtual void draw();
	bool adjustYPos(int change);
	void freeQuadBuffer();

	unsigned int bufId;
	
	glm::i32vec2 positionOnPage;
	glm::i32vec2 size;
	glm::vec4 textColour;

	int textObj;
	int textPos;
	int textEnd;
	int textLength;

protected:
	CRenderer* pRenderer;
	CFont* font;
	TTextSpriteShader* textSpriteShader;
	glm::mat4 matrix;
	CRenderTexture* pageTexture;
	CBuf tmpQuadBuf;
	ITextSpriteCallback* callbackObj;

private:
	void updateMatrix();

	glm::mat4* pageOrthoViewMatrix;
};

class ITextSpriteCallback {
public:
	virtual int reserveSpriteImageSpace(glm::i32vec2& size) = 0;
	virtual void freeSpriteImageSpace(int bufId) = 0;
	virtual glm::vec4 getHotTextColour() = 0;
};

class CHotTextSprite : public CTextSprite {
public:
	~CHotTextSprite();
	void createTextImage(CRenderTexture& texture);
	void setTextColour(glm::vec4& colour);
	void draw();

	glm::vec4 hotTextColour;
private:
	CBuf tmpVertBuf2;
	unsigned int bufId2;
};