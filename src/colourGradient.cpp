#include "colourGradient.h"
#include <algorithm> 
#include <iostream>

#include "colour.h"

using namespace glm;


ColourGradient::ColourGradient() {
	tabs[0] = glm::i32vec4( 0,0,0,255);
	tabs[255] = glm::i32vec4( 255,255,255,255);
}

/** Create a colourGradient object with the given palette. */
ColourGradient::ColourGradient(std::initializer_list<int> bytes) : ColourGradient() {
	int elements[5];
	int count = 0;
	for (auto element : bytes) {
		elements[count] = element;
		count++;
		if (count == 5) {
			changeColour(elements[0], i32vec4(elements[1], elements[2], elements[3], elements[4]));
			count = 0;
		}
	}
}

/** Provide 256 entries of colour data suitable for a 1D texture. */
void * ColourGradient::getData() {
	pixels.clear();
	auto nextTab = tabs.begin();
	auto prevTab = tabs.begin();

	for (int slot = 0; slot < 256; slot++) {
		float ratio = float(slot - prevTab->first) / (nextTab->first - prevTab->first);
		glm::i32vec4 colour = glm::mix(prevTab->second, nextTab->second, ratio);
		TPixel pixel = {(unsigned char) colour.r,(unsigned char)colour.g,(unsigned char)colour.b,(unsigned char)colour.a };
		pixels.push_back(pixel);


		if (nextTab->first == slot) {
			prevTab = nextTab;
			nextTab++;
		}
	}
	//pixels[254] = { 255,0,0,255 };
	return (void*)pixels.data();
}

void ColourGradient::insertColour(unsigned char index, glm::i32vec4& newColour) {
	tabs[index] = newColour;
	
}

/** Change the colour of the tab at this position. */
void ColourGradient::changeColour(unsigned char tabPos, glm::i32vec4& newColour) {
	tabs[tabPos] = newColour;
}

glm::i32vec4 ColourGradient::getColour(int index) {
	if (tabs.find(index) != tabs.end())
		return tabs[index];

	auto nextTab = tabs.begin();
	for (nextTab = tabs.begin(); nextTab != tabs.end(); nextTab++) {
		if (nextTab->first > index)
			break;
	}; 
	auto prevTab = std::prev(nextTab);
	float ratio = float(index - prevTab->first) / (nextTab->first - prevTab->first);
	return glm::mix(prevTab->second, nextTab->second, ratio);
}

/** Move a tab somewhere else - if it exists at this index position. */
int ColourGradient::moveTab(int oldPos, int newPos) {

	if (tabs.find(newPos) != tabs.end() && newPos != 255 && newPos != 0) {
		return oldPos;
	}

	tabs[newPos] = tabs[oldPos];;
	
	if (oldPos != 0 && oldPos != 255)
		tabs.erase(oldPos);
	return newPos;
}

void ColourGradient::deleteTab(unsigned char tabPos) {
	if (tabPos != 0 && tabPos != 255 && tabs.find(tabPos) != tabs.end() )
		tabs.erase(tabPos);
}

/** Rotate all the tab colours by the given amount, using the HSV colour model. */
void ColourGradient::rotateTabHues(float rotation) {
	for (auto& tab : tabs) {
		i32vec4 colour = tab.second;
		vec3 rgb = col::byteToFloat(colour);
		vec3 hsv = glm::hsvColor(rgb);
		hsv.x += rotation;
		hsv.x = fmod(hsv.x, 360);
		rgb = glm::rgbColor(hsv);
		colour = i32vec4(vec3(rgb * 255.0f), colour.a);
		tab.second = colour;
	}
}

const std::map<int, glm::i32vec4>& ColourGradient::getTabs() {
	return tabs;
}
