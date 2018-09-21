#include "colourGradient.h"

ColourGradient::ColourGradient() {
	colours.push_back({ 0,{0,0,0,255} });
	colours.push_back({ 255,{255,255,255,255} });
}

/** Provide 256 entries of colour data suitable for a 1D texture. */
void * ColourGradient::getData() {

	

	int lastElement = 0; 
	TColour lastColour = colours[0];
	TColour nextColour = getNextEntry(0);
	int nextElement = nextColour.index;

	for (int slot = 0; slot < 256; slot++) {
		float ratio = float(slot - lastElement) / (nextElement - lastElement);
		glm::i32vec4 colour = glm::mix(lastColour.colour, nextColour.colour, ratio);
		TPixel pixel = { colour.r,colour.g,colour.b,colour.a };
		pixels.push_back(pixel);


		if (nextElement == slot) {
			lastElement = slot;
			lastColour = nextColour;
			nextColour = getNextEntry(slot);
			nextElement = nextColour.index;
		}
	}
	return (void*)pixels.data();
}

void ColourGradient::insertColour(unsigned int index, glm::i32vec4 newColour) {
	TColour colour = { index, {newColour} };
	for (unsigned int entry = 0; colours.size(); entry++) {
		if (colours[entry].index == index) {
			colours[entry] = colour;
			return;
		}
		if (colours[entry].index > index) {
			colours.insert(colours.begin() + entry, colour);
			return;
		}
	}
	
}

TColour ColourGradient::getNextEntry(unsigned char index) {
	for (auto entry : colours) {
		if (entry.index > index)
			return entry;
	}
	return colours.back();
}
