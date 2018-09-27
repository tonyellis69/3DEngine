#include "colourGradient.h"
#include <algorithm> 
#include <iostream>

ColourGradient::ColourGradient() {
	colours[0] = glm::i32vec4( 0,0,0,255);
	colours[255] = glm::i32vec4( 255,255,255,255);
}

/** Provide 256 entries of colour data suitable for a 1D texture. */
void * ColourGradient::getData() {
	pixels.clear();
	auto nextTab = colours.begin();
	auto prevTab = colours.begin();

	for (int slot = 0; slot < 256; slot++) {
		float ratio = float(slot - prevTab->first) / (nextTab->first - prevTab->first);
		glm::i32vec4 colour = glm::mix(prevTab->second, nextTab->second, ratio);
		TPixel pixel = { colour.r,colour.g,colour.b,colour.a };
		pixels.push_back(pixel);


		if (nextTab->first == slot) {
			prevTab = nextTab;
			nextTab++;
		}
	}
	return (void*)pixels.data();
}

void ColourGradient::insertColour(unsigned char index, glm::i32vec4& newColour) {
	colours[index] = newColour;
	
}

/** Change the colour of the tab at this position. */
void ColourGradient::changeColour(unsigned char tabPos, glm::i32vec4& newColour) {
	colours[tabPos] = newColour;
}

glm::i32vec4 ColourGradient::getColour(int index) {
	if (colours.find(index) != colours.end())
		return colours[index];

	auto nextTab = colours.begin();
	for (nextTab = colours.begin(); nextTab != colours.end(); nextTab++) {
		if (nextTab->first > index)
			break;
	}; 
	auto prevTab = std::prev(nextTab);
	float ratio = float(index - prevTab->first) / (nextTab->first - prevTab->first);
	return glm::mix(prevTab->second, nextTab->second, ratio);
}

/** Move a tab somewhere else - if it exists at this index position. */
void ColourGradient::moveTab(int oldPos, int newPos) {
	if (colours.find(oldPos) == colours.end() || oldPos == newPos) {
		return;
	}
	
	colours[newPos] = colours[oldPos];;
	
	if (oldPos != 0 && oldPos != 255)
		colours.erase(oldPos);
}

void ColourGradient::deleteTab(unsigned char tabPos) {
	if (tabPos != 0 && tabPos != 255 && colours.find(tabPos) != colours.end() )
		colours.erase(tabPos);
}
