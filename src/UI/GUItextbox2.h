#pragma once

#include "GUIbetterBase.h"
#include "textBuf.h"

/** Simple textbox control using shader and render-to-texture. */
class CGUItextbox2 : public CGUIbetterBase {
public:
	CGUItextbox2(int x, int y, int w, int h);

	void setFont(CFont* font);
	void setText( std::string newText);
	void setTextColour(float r, float g, float b, float a);
	void setTextColour(UIcolour colour);
	void setHorizontalAlignment(TTextAlign align);
	void DrawSelf();
	void OnClick(const  int mouseX, const  int mouseY);
	void onKeyPress(unsigned int Key, long Mod);
	void OnCharEntry(unsigned int Key, long Mod);
	void message(CGUIbase& sender, CMessage& msg);

	CTextBuffer textBuf;
	glm::vec4 textColour;

private:
	void dataEnteredAlert();
};