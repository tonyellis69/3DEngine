#pragma once

//include "../renderer/renderer.h"
#include "text.h"
#include "imageBuf.h"
#include "textSprite.h"
#include "../renderer/shader.h"


class ILineBufferCallback;
/** A class that draws multiple blocks of text to a texture buffer for output,
	in real-time, employing whatever effects these blocks require. */
class CLineBuffer2 : public ITextSpriteCallback {
public:
	CLineBuffer2();
	~CLineBuffer2();
	void setCallbackObj(ILineBufferCallback* obj);
	void setPageSize(int width, int height);
	void clear();
	void addTextSprite(TLineFragment& fragment);
	void draw();
	void renderSprites();
	int scrollDown(int scrollAmount);
	int scrollUp(int scrollAmount);
	int getOverlap();
	int getTopOverlap();
	CRenderTexture* getTextBuf();
	TLineFragment getFinalFrag();
	TCharacterPos getPageStart();
	TCharacterPos getPageEnd();
	void setAddFragmentsAtTop(bool onOff);

private:
	void initShader();
	void updateFinalFrag(CTextSprite* sprite);
	void recalcPageState();
	void setPageStartEnd(TLineFragment& fragment);
	int calcSpriteYpos(TLineFragment& fragment);
	CTextSprite* createSprite(TLineFragment& fragment);
	int reserveImageSpace(glm::i32vec2& size);
	void freeSpriteMemory(int bufId);

	C2DimageBuf spriteBuffer; ///<Provides storage for text sprite image buffers.

	CRenderTexture textBuf; ///The text buffer we're drawing to.
	
	int width;
	int height;

	ILineBufferCallback* pCallbackObj;
	CRenderer* pRenderer;

	std::vector<CTextSprite*> textSprites;

	TLineBufferShader textSpriteShader;
	glm::mat4 orthoView;

	TLineFragment finalFrag;

	int yPosTracker; ///<Tracks how far down the page we've got
	TCharacterPos pageStart; ///<Identifies where in the textObjs this page starts.
	TCharacterPos pageEnd; ///<Identifies where in the textObjs this page ends.
	bool insertAtTop; ///<Set true to add new sprites above the current ones, instead of below.
};

class ILineBufferCallback {
public:
	virtual TRichTextRec& getTexObjCallback(int objNo) = 0;
	virtual glm::vec4& getHotTextColour() = 0;
	virtual glm::vec4& getHotTextSelectedColour() = 0;

};



