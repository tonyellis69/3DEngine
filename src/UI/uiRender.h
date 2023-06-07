#pragma once

#include "UI/GUIbase.h" //for guiRect
#include "renderer/renderTexture.h" //until I use ids or strings instead
#include "font.h"

#include "glm/glm.hpp"

/** A library of functions for drawing GUI stuff. */

namespace uiDraw {

	void onSpawn();
	void drawRect(guiRect& drawBox, const glm::vec4& colour1, const glm::vec4& colour2);
	void drawBorder(guiRect& drawBox, const glm::vec4& colour);
	void drawTexture(guiRect& drawBox, CRenderTexture& texture);
	void drawTextureGradient(guiRect& drawBox, CBaseTexture& texture);
	CFont* getFont(const std::string& name);
	void setClip(UIrect& rect);








}


