#pragma once

#include "GUIcontainer.h"
#include "GUIcolourPicker.h"
#include "GUIbutton2.h"

/** A GUI container for holding a nunber of grouped colour swatches. These
	can be added to, edited, dragged and dropped. The container can use a 
	text file to save and restore its swatches. */
class CGUIswatchGroup;
class CGUIswatchContainer : public CGUIcontainer {
public:
	CGUIswatchContainer(int x, int y, int w, int h);
	 ~CGUIswatchContainer();
	void addSwatchGroup(std::string name, std::vector<glm::i32vec4>* colours);
	void onDoubleClick(const int mouseX, const int mouseY, int key);
	void message(CGUIbase* sender, CMessage& msg);
	void GUIcallback(CGUIbase* sender, CMessage& msg);
	void openColourPicker(CGUIswatchGroup * group, int swatchNo);
	void respaceControls();
	void assignDataFile(std::string filename);
	void clearSwatchGroups();
	void writeDataFile();
	void deleteGroup(CGUIswatchGroup * group);

	int swatchSize;
	int swatchGap;
	int groupGap;
	int surroundGap;
	int nextGroupY;

	int addButtonX;
	CGUIbutton2* addButton;
	vector<glm::i32vec4> defaultSwatches;

	CGUIcolourPicker* colourPicker;
	int lastClickedSwatch;
	CGUIswatchGroup* lastClickedSwatchGroup;
	std::string datafileName; ///<Name of the file used to save/restore, if any.
};



class CGUIswatchGroup : public CGUIpanel {
public:
	CGUIswatchGroup(int x, int y, int w, int h);
	void DrawSelf();
	void OnClick(const  int mouseX, const  int mouseY);
	int getSwatchIndex(const int mouseX, const int mouseY);
	void onDoubleClick(const int mouseX, const int mouseY, int key);
	void OnRMouseDown(const  int mouseX, const  int mouseY, int key);
	void onDrag(const  int mouseX, const  int mouseY);
	void onDrop(const  int mouseX, const  int mouseY);


	void resize(int w, int h);


	void addColours(std::vector<glm::i32vec4>* swatches);
	glm::i32vec4& getSwatchColour(int swatchNo);
	void setSwatchColour(int& swatchNo, glm::i32vec4& colour);
	void addSwatch(glm::i32vec4& newSwatch);
	void addUserSwatch(glm::i32vec4 & newSwatch);
	void calcRowColumnSize();
	int calcRequiredHeight(int width);

	int swatchSize;
	int swatchGap;
	int surroundGap;
	std::vector<glm::i32vec4> swatches;
	int totalRows;
	int totalColumns;

	CGUItextbox2* nameTextbox;
};

class CGUIdragDropSwatch : public CGUIdragDrop {
public:
	virtual bool isDropType(CGUIbase* droppee) {
		if (droppee->type == uiSwatchGroup || droppee->type == uiPaletteTab ||
			droppee->type == uiPaletteBar)
			return true;
		return false;
	};
	~CGUIdragDropSwatch() {};
	void drawSelf(int mouseX, int mouseY);

	glm::i32vec4 colour; //<Colour of swatch being dragged.
	int swatchSize;
};


