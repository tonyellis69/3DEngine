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

void CGUItrackImage::onMouseOff(const int mouseX, const int mouseY, int key) {
	if (callbackObj) {
		CMessage msg;
		msg.Msg = uiMsgMouseOff;
		callbackObj->GUIcallback(this, msg);
	}
}

void CGUItrackImage::OnRMouseDown(const int mouseX, const int mouseY, int key) {
	if (callbackObj) {
		CMessage msg;
		msg.x = mouseX;
		msg.y = mouseY;
		msg.Msg = uiMsgRMdown;
		callbackObj->GUIcallback(this, msg);
	}
}

bool CGUItrackImage::MouseWheelMsg(const int mouseX, const int mouseY, int wheelDelta, int key) {
	if (callbackObj) {
		CMessage msg;
		msg.x = mouseX;
		msg.y = mouseY;
		msg.value = wheelDelta;
		msg.Msg = uiMouseWheel;
		callbackObj->GUIcallback(this, msg);
	}
	return true;
}

void CGUItrackImage::onDoubleClick(const int mouseX, const int mouseY, int key) {
	if (callbackObj) {
		CMessage msg;
		msg.Msg = uiMsgDoubleClick;
		msg.value = key;
		callbackObj->GUIcallback(this, msg);
	}
}
