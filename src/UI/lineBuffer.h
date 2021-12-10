#pragma once

#include <random>

//include "../renderer/renderer.h"
//#include "text.h"
#include "imageBuf.h"
#include "textSprite.h"
#include "../renderer/shader.h"

#include "richText.h"

struct THotRec {
	//CHotTextSprite* sprite;
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
class CLineBuffer : public ITextSpriteCallback {
public:
	CLineBuffer();
	void setCallbackObj(ILineBuffer* obj);
	void setPageSize(int width, int height);
	void clear();
	bool addTextSprite(TFragment2& fragment);
	void draw();
	void renderSprites(float dT);
	int scrollDown3(int scrollAmount);
	int scrollUp3(int scrollAmount);
	int getOverlap();
	int getUnderrun();
	int getTopOverlap();
	CRenderTexture* getTextBuf();

	void setaddFragmentsAtTop(bool onOff);
	std::string onMouseMove(glm::i32vec2& mousePos);
	void onMouseOff();
	bool doesPageEndWithNewline() {
		return pageEndNewline;
	}
	int getPageTextStart();
	int getPageTextEnd();




private:
	CTextSprite* createSprite(TFragment2& fragment);
	void updateHotTextPeriods(float dT);
	float getHotPeriod(unsigned int hotId);
	float randomPeriod(float start);
//	void freeHotTextSprite(CHotTextSprite* sprite);
	void onMousedHotTextChange();

	glm::i32vec2 calcPageTextEnd();
	glm::i32vec2 calcPageTextStart();
	
	CRenderTexture pageBuf; ///The buffer we're drawing text sprites on.
	
	int width;
	int height;

	ILineBuffer* pCallbackObj;

	std::vector< std::unique_ptr<CTextSprite> > textSprites;

	glm::mat4 pageOrthoView;

	//TPagePos pageStart; ///<Identifies where in the textObjs this page starts.
	//TPagePos pageEnd;  ///<Identifies where in the textObjs this page ends.

	bool insertAtTop; ///<Set true to add new sprites above the current ones, instead of below.

	std::map<unsigned int, THotState> hotTexts; ///<The hotIds and periods of all displayed hot texts
//	std::vector<THotRec> hotTextSprites; ///<Pointers to and hot ids of all hot text sprites

	std::mt19937 randEngine;



	bool pageEndNewline; ///<Records if page ends by forcing a newline.

	int current1stSprite;

	bool pageDirty = true; ///<If true we re-render the page.
};





//class ILineBuffer {
//public:
//	//virtual TRichTextRec* getTexObjCallback(int objNo) = 0;
//	//virtual glm::vec4& getHotTextColour() = 0;
//	//virtual glm::vec4& getHotTextSelectedColour() = 0;
//	virtual void hotTextMouseOver(int hotId) = 0;
//};
//




