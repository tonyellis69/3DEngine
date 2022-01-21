#pragma once

#include <glm/glm.hpp>


enum TGuiMsgType { gNone, gMouseMove, gMouseOff };


/** Format for GUI messages - typically mouse activity.
*/

class CguiMsg {
public:
	CguiMsg() {}
	TGuiMsgType type;
	glm::i32vec2 mousePos;

};