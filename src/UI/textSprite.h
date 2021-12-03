#pragma once

#include <string>
#include <memory>

#include <glm/glm.hpp>

#include "font.h"
//#include "../renderer/buf.h"
#include "../renderer/shader.h"
#include "buf2.h"

struct TTextSpriteShader {
	unsigned int hOrthoMatrix;
	unsigned int hTextureUnit;
	unsigned int hOffset;
	unsigned int hOffset2;
	unsigned int hSize;
	unsigned int hAlpha;
	CShader* shader;
};

class ITextSpriteCallback;
/** Defines a rectangular block of pre-rendered text. */
class CTextSprite {
public:
	CTextSprite();
	virtual ~CTextSprite();
	virtual void createTextImage(CRenderTexture& texture);
	void setTextObjData(int textObj, int textPos, int textLength);
	void setPagePosition(int posX, int posY);
	virtual void setTextColour(glm::vec4& colour);

	void makeTextQuads(const std::string& text, CFont* font);
	void renderTextQuads(CRenderTexture& buf, glm::i32vec2& storageId, glm::vec4& colour);
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
	virtual void setHotId(unsigned int hotId) {}
	virtual unsigned int getHotId() { return -1; }

	CBuf2 tmpQuadBuf;

	//unsigned int bufId;
	glm::i32vec2 bufId;
	
	glm::i32vec2 positionOnPage;
	glm::i32vec2 size;
	glm::vec4 textColour;

	int textObj;
	int textPos;
	int textEnd;
	int textLength;
	bool causesNewLine;
	int textStart;

	bool isHotText = false;

protected:
	CRenderer* pRenderer;
	CFont* font;
	TTextSpriteShader* textSpriteShader;
	glm::mat4 matrix;
	CRenderTexture* pageTexture;

	ITextSpriteCallback* callbackObj;

private:
	void updateMatrix();

	glm::mat4* pageOrthoViewMatrix;
};



class CHotTextSprite : public CTextSprite {
public:
	CHotTextSprite();

	bool highlighted = false;
};




//class CHotTextSprite : public CTextSprite {
//public:
//	 ~CHotTextSprite();
//	void createTextImage(CRenderTexture& texture);
//	void setTextColour(glm::vec4& colour);
//	void draw();
//	void setHotId(unsigned int hotId);
//	unsigned int getHotId();
//
//	glm::vec4 hotTextColour;
//	glm::i32vec2 bufId2;
//private:
//	//CBuf tmpVertBuf2;
//
//	unsigned int hotId;
//};
//
class ITextSpriteCallback {
public:
	virtual glm::i32vec2 reserveSpriteImageSpace(glm::i32vec2& size) = 0;
	virtual void freeSpriteImageSpace(glm::i32vec2& bufId) = 0;
	//virtual void freeHotTextSprite(CHotTextSprite* sprite) = 0;
	//virtual glm::vec4 getHotTextColour() = 0;
	virtual float getHotPeriod(unsigned int hotId) = 0;
};