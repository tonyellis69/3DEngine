#pragma once

#include <string>
#include <memory>

#include <glm/glm.hpp>

#include "font.h"
#include "../renderer/buf.h"
#include "../renderer/shader.h"

struct TLineBufferShader {
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
	virtual void createText(CRenderTexture& texture);

	void setPosition(int posX, int posY);

	int makeTextVerts(const std::string& text, CFont* font);
	void writeTextToBuffer(CRenderTexture& buf, glm::vec4& colour);
	virtual void write2ndTextToBuffer(CRenderTexture& buf, glm::vec4& colour) {};
	void setCallbackObj(ITextSpriteCallback* obj) {
		callbackObj = obj;
	}
	void setPageBuf(CRenderTexture* pageBuf);
	void setOthoMatrix(glm::mat4* orthoMatrix);
	void setShader(TLineBufferShader* shader);
	virtual void draw();
	bool adjustYPos(int change);
	void freeVertBuffer();

	unsigned int bufId;
	unsigned int bufId2;
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
	TLineBufferShader* shader;
	glm::mat4 matrix;
	CRenderTexture* pageBuf;
	CBuf tmpVertBuf;
	ITextSpriteCallback* callbackObj;

private:
	void updateMatrix();


	glm::mat4* orthoMatrix;
};

class ITextSpriteCallback {
public:
	virtual int reserveImageSpace(glm::i32vec2& size) = 0;
	virtual void freeSpriteMemory(int bufId) = 0;
};

class CHotTextSprite : public CTextSprite {
public:
	~CHotTextSprite();
	void createText(CRenderTexture& texture);
	void write2ndTextToBuffer(CRenderTexture& buf, glm::vec4& colour);
	void draw();

	glm::vec4 hotTextColour;
private:
	CBuf tmpVertBuf2;
};