#include "GUItrackImage.h"


void CGUItrackImage::OnMouseMove(const int mouseX, const int mouseY, int key) {
	if (callbackObj) {
		CMessage msg;
		msg.Msg = uiMsgMouseMove;
		msg.x = mouseX;
		msg.y = mouseY;
		callbackObj->GUIcallback(this, msg);
	}

}
