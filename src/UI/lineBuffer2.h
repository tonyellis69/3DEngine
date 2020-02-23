#pragma once

#include <random>

//include "../renderer/renderer.h"
#include "text.h"
#include "imageBuf.h"
#include "textSprite.h"
#include "../renderer/shader.h"

struct THotRec {
	CHotTextSprite* sprite;
	unsigned int hotId;
};

enum TSelectionState {selNone,selWarmup,selLit, selWarmdown};
struct THotState {
	float period;
	TSelectionState selectionState;
};

class ILineBuffer;
/** A class that draws multiple blocks of text to a texture buffer for output,
	in real-time, employing whatever effects these blocks require. */
class CLineBuffer2 : public ITextSpriteCallback {
public:
	CLineBuffer2();
	~CLineBuffer2();
	void setCallbackObj(ILineBuffer* obj);
	void setPageSize(int width, int height);
	void clear();
	void addTextSprite(TLineFragment& fragment);
	void draw();
	void renderSprites(float dT);
	int scrollDown(int scrollAmount);
	int scrollUp(int scrollAmount);
	int getOverlap();
	int getUnderrun();
	int getTopOverlap();
	CRenderTexture* getTextBuf();
	TLineFragment getFinalFrag();
	TCharacterPos getPageStart();
	TCharacterPos getPageEnd();
	void setAddFragmentsAtTop(bool onOff);
	void onMouseMove(glm::i32vec2& mousePos);

private:
	void initShader();
	void updateFinalFrag(CTextSprite& sprite);
	void recalcPageState();
	void updatePageStartEnd(TLineFragment& fragment);
	int calcSpriteYpos(TLineFragment& fragment);
	CTextSprite* createSprite(TLineFragment& fragment);
	glm::i32vec2 reserveSpriteImageSpace(glm::i32vec2& size);
	void freeSpriteImageSpace(glm::i32vec2& bufId);
	glm::vec4 getHotTextColour();
	void updateHotTextPeriods(float dT);
	float getHotPeriod(unsigned int hotId);
	float randomPeriod(float start);
	void freeHotTextSprite(CHotTextSprite* sprite);
	void onMousedHotTextChange();

	C2DimageBuf spriteBuffer; ///<Provides storage for text sprite image buffers.

	CRenderTexture pageBuf; ///The buffer we're drawing text sprites on.
	
	int width;
	int height;

	ILineBuffer* pCallbackObj;
	CRenderer* pRenderer;

	//std::vector<CTextSprite*> textSprites;
	std::vector< std::unique_ptr<CTextSprite> > textSprites;

	TTextSpriteShader textSpriteShader;
	glm::mat4 pageOrthoView;

	TLineFragment finalFrag;

	int yPosTracker; ///<Tracks how far down the page we've got
	TCharacterPos pageStart; ///<Identifies where in the textObjs this page starts.
	TCharacterPos pageEnd; ///<Identifies where in the textObjs this page ends.
	bool insertAtTop; ///<Set true to add new sprites above the current ones, instead of below.

	std::map<unsigned int, THotState> hotTexts; ///<The hotIds and periods of all displayed hot texts
	std::vector<THotRec> hotTextSprites; ///<Pointers to and hot ids of all hot text sprites

	std::mt19937 randEngine;

	int mousedHotText;
	int prevMousedHotText;
};

class ILineBuffer {
public:
	virtual TRichTextRec& getTexObjCallback(int objNo) = 0;
	virtual glm::vec4& getHotTextColour() = 0;
	virtual glm::vec4& getHotTextSelectedColour() = 0;
	virtual void hotTextMouseOver(int hotId) = 0;
};





