#pragma once

//include "../renderer/renderer.h"
#include "text.h"
#include "imageBuf.h"
#include "textSprite.h"
#include "../renderer/shader.h"


class ILineBufferCallback;
/** A class that draws multiple blocks of text to a texture buffer for output,
	in real-time, employing whatever effects these blocks require. */
class CLineBuffer2 {
public:
	CLineBuffer2();
	void setCallbackObj(ILineBufferCallback* obj);
	void setPageSize(int width, int height);
	void clear();
	void addTextSprite(TLineFragment& fragment);
	CRenderTexture& getBuffer();
	void renderToTextBuf(CRenderTexture& texBuf);

private:
	C2DimageBuf imageBuf; ///<Provides storage for text sprite image buffers.

	int width;
	int height;

	ILineBufferCallback* pCallbackObj;
	CRenderer* pRenderer;

	std::vector<CTextSprite> textSprites;

	CShader* lineBufShader;
	unsigned int hOrthoMatrix;
	unsigned int hTextureUnit;
	unsigned int hOffset;
	unsigned int hSize;
	glm::mat4 orthoView;
};

class ILineBufferCallback {
public:
	virtual TRichTextRec& getTexObjCallback(int objNo) { return TRichTextRec();  }
};



