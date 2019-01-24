#include "GUIswatchContainer.h"

#include <fstream>
#include <sstream>
#include <iomanip>

#include "GUItextbox2.h"
#include "GUIroot.h"

using namespace glm;

CGUIswatchContainer::CGUIswatchContainer(int x, int y, int w, int h) 
	: CGUIcontainer(x,y,w,h) {
	swatchSize = 20;
	swatchGap = 10;
	groupGap = 2;
	addButtonX = 10;
	surroundGap = 3;
	nextGroupY = groupGap;
	defaultSwatches = { {255,0,0,255}, {255,165,0,255}, {255,255,0,255} ,{255,0,0,255}, {255,165,0,255}, {255,255,0,255}
				,{255,0,0,255}, {255,165,0,255} , { 255,255,0,255 }
};

	addButton = new CGUIbutton2(addButtonX, nextGroupY, 100, 30);
	addButton->setText("Add group");
	Add(addButton);
	addButton->setGUIcallback(this);

	setControlMargin(5);

	
	addSwatchGroup("Swatch group", &defaultSwatches);
	//addButton->setPos(indent, nextGroupY);
	//respaceControls();
//	updateAppearance();
	//needsUpdate = true;
	//newGroup->calcRowColumnSize();


	colourPicker = new CGUIcolourPicker(0, 0, 500, 400);
	colourPicker->setGUIcallback(this);
	colourPicker->setVisible(false);
	rootUI->Add(colourPicker);
}

CGUIswatchContainer::~CGUIswatchContainer() {
	writeDataFile();
	CGUIcontainer::~CGUIcontainer();
}


/** Add a swatch group with the given name and colour swatches. */
void CGUIswatchContainer::addSwatchGroup(std::string name, std::vector<glm::i32vec4>* colours) {
	int defaultWidth = surface->drawBox.size.x;// -(indent * 2);
	CGUIswatchGroup* newGroup = new CGUIswatchGroup(swatchGap, nextGroupY, defaultWidth, swatchSize + swatchGap );
	newGroup->anchorLeft = true;
	newGroup->anchorRight = 5;
	newGroup->swatchSize = swatchSize;
	newGroup->swatchGap = swatchGap;
	newGroup->surroundGap = surroundGap;
	newGroup->nameTextbox->setText(name);
	newGroup->setGUIcallback(this);
	newGroup->addColours(colours);
	Add(newGroup);
	nextGroupY += newGroup->drawBox.size.y;// +groupGap;
	respaceControls();
	
	//fitViewBoxToContainer();
	//updateAppearance();
	//needsUpdate = true;
	/*addButton->setPos(indent, nextGroupY);
	respaceControls();
	updateAppearance();
	needsUpdate = true;
	newGroup->calcRowColumnSize();*/
}

void CGUIswatchContainer::onDoubleClick(const int mouseX, const int mouseY, int key) {

}

void CGUIswatchContainer::message(CGUIbase * sender, CMessage & msg) {
	CGUIcontainer::message(sender, msg);
	
}

void CGUIswatchContainer::GUIcallback(CGUIbase * sender, CMessage & msg) {
	if (sender == addButton) {
		addSwatchGroup("New group", &defaultSwatches);
	}

	if (msg.Msg == uiClick && sender->type == uiSwatchGroup) {
		openColourPicker(static_cast<CGUIswatchGroup*>(sender), msg.value);
	}

	if (sender == colourPicker) {
		if (msg.Msg == uiOKid) {
			lastClickedSwatchGroup->setSwatchColour(lastClickedSwatch, colourPicker->getColour());
			colourPicker->setVisible(false);
			colourPicker->makeUnModal();
		}
		if (msg.Msg == uiCancelId) {
			colourPicker->setVisible(false);
			colourPicker->makeUnModal();
		}
	}

	if (msg.Msg == uiMsgChildResize && sender->type == uiSwatchGroup) {
		respaceControls();
	}

	if (msg.Msg == uiMsgDoubleClick && sender->type == uiSwatchGroup) {
		openColourPicker(static_cast<CGUIswatchGroup*>(sender), msg.value);
	}

	if (sender->type == uiSwatchGroup && msg.Msg == uiMsgDelete) {
		deleteGroup((CGUIswatchGroup*)sender);
	}
}

void CGUIswatchContainer::openColourPicker(CGUIswatchGroup* group, int swatchNo) {
	i32vec4 colour = group->getSwatchColour(swatchNo);
	lastClickedSwatch = swatchNo;
	lastClickedSwatchGroup = group;
	colourPicker->setColour(colour);
	colourPicker->makeModal();
	colourPicker->setVisible(true);
}

/** Rearrange controls not to overlap after a swatch group is resized. */
void CGUIswatchContainer::respaceControls() {
	int lastY = groupGap;
	for (auto control : surface->Control) {
		if (control->type == uiSwatchGroup) {
			control->setPos(control->localPos.x, lastY);
			lastY = control->localPos.y + control->drawBox.size.y + groupGap;
		}
	}
	addButton->setPos(addButtonX, lastY);
	adaptToContents();
	needsUpdate = true;
}

/** Assign and attempt to read a text file of swatch group definitions. */
void CGUIswatchContainer::assignDataFile(std::string filename) {
	datafileName = filename;
	clearSwatchGroups();
	ifstream inFile(filename, ifstream::in);
	string line;
	string groupName;
	vector<glm::i32vec4> colours;
	while (getline(inFile, line)) {
		if (line[0] == '\"') {
			if (colours.size() > 0) {
				addSwatchGroup(groupName, &colours);
			}

			groupName = line.substr(1, line.size() - 2);
			colours.clear();
			continue;
		}
		if (isdigit(line[0])) {
			stringstream ss(line);
			string number; int i = 0;
			i32vec4 colour;
			while (getline(ss, number, ',')) {
				colour[i++] = stoi(number);
			}
			colours.push_back(colour);
		}
		if (line[0] == '#') {
			i32vec4 colour(255); size_t i = 0;
			size_t n = 1;
			//line = line.substr(1, line.size());
			while (n < line.size()) {
				colour[i++] = stoi(line.substr(n,2), nullptr, 16);
				n += 2;
			}
			colours.push_back(colour);
		}
	}

	if (colours.size() > 0) {
		addSwatchGroup(groupName, &colours);
	}
	inFile.close();
}

/** Remove all existing swatch groups and their labels. */
void CGUIswatchContainer::clearSwatchGroups() {
	for (auto it = surface->Control.begin(); it != surface->Control.end(); )
	{
		if ((*it)->type == uiSwatchGroup)
			it = surface->Control.erase(it);
		else
			++it;
	}
	nextGroupY = groupGap;
}

/** Write the current swatch groups to the data file, if one is assigneed. */
void CGUIswatchContainer::writeDataFile() {
	if (datafileName == "")
		return;

	ofstream outFile(datafileName, ofstream::out);
	for (auto control : surface->Control) {
		//if (control->type == uiTextbox) {
		//	outFile << std::quoted(static_cast<CGUItextbox2*>(control)->getText()) << "\n";
		//}
		if (control->type == uiSwatchGroup) {
			outFile << std::quoted(static_cast<CGUIswatchGroup*>(control)->nameTextbox->getText()) << "\n";
			for (auto colour : static_cast<CGUIswatchGroup*>(control)->swatches) {
				outFile << colour.r << "," << colour.g << "," << colour.b << ","
					<< colour.a << "\n";
			}
			outFile << "\n";
		}
	}
	outFile.close();
}

/** Remove this group and rearrange the remainers. */
void CGUIswatchContainer::deleteGroup(CGUIswatchGroup * group) {
	delete group;
	respaceControls();
}











CGUIswatchGroup::CGUIswatchGroup(int x, int y, int w, int h) 
	: CGUIpanel(x, y, w, h) {
	type = uiSwatchGroup;
	nameTextbox = new CGUItextbox2(0, 0, w/2, 18);
	nameTextbox->anchorLeft = true;
	nameTextbox->anchorRight = 5;
	nameTextbox->borderOn(false);
	nameTextbox->setBackColour1(uiLightGrey);
	nameTextbox->setBackColour2(uiLightGrey);
	Add(nameTextbox);
	nameTextbox->borderOn(true);
}

void CGUIswatchGroup::DrawSelf() {
//	CGUIpanel::DrawSelf();
	guiRect swatchBox = { drawBox.pos.x + swatchGap,nameTextbox->drawBox.pos.y + nameTextbox->drawBox.size.y + surroundGap,
		swatchSize,swatchSize };
	int columnCount = 1;
	for (auto swatch : swatches) {
		vec4 colour = vec4(swatch) / 255.0f;
		pDrawFuncs->drawRect2(swatchBox, colour, colour);
		pDrawFuncs->drawBorder2(swatchBox, uiLightGrey);
		if (columnCount < totalColumns) {
			columnCount++;
			swatchBox.pos.x += swatchSize + swatchGap;
		}
		else {
			columnCount = 1;
			swatchBox.pos = { drawBox.pos.x + swatchGap,swatchBox.pos.y + swatchSize + swatchGap };
		}
	}
}

/** Alert the container if the user clicks on a swatch. */
void CGUIswatchGroup::OnClick(const int mouseX, const int mouseY) {
	int index = getSwatchIndex(mouseX, mouseY);
	if (index == -1)
		return;
		CMessage msg;
		msg.Msg = uiClick;
		msg.value = index;
		callbackObj->GUIcallback(this, msg);
}

int CGUIswatchGroup::getSwatchIndex(const int mouseX, const int mouseY) {
	i32vec2 localMouse = getLocalPos(mouseX, mouseY);
	localMouse.y -= nameTextbox->drawBox.size.y + surroundGap;
	localMouse.x -= swatchGap;
	if (localMouse.y < 0 || localMouse.x < 0)
		return -1;
	i32vec2 rowCol = localMouse / (swatchSize + swatchGap);
	if (localMouse.x > (rowCol.x * (swatchSize + swatchGap) + swatchSize)
		|| localMouse.y > (rowCol.y * (swatchSize + swatchGap) + swatchSize))
		return -1;

	int index = rowCol.x + (rowCol.y * totalColumns);
	if (index >= swatches.size())
		return -1;
	return index;
}

void CGUIswatchGroup::onDoubleClick(const int mouseX, const int mouseY, int key) {
	addUserSwatch(i32vec4(127,127,127,255 ));
}

/** Use to delete group if Ctrl held down. */
void CGUIswatchGroup::OnRMouseDown(const int mouseX, const int mouseY, int key) {
	if (key == GLFW_MOD_CONTROL) {
		CMessage msg;
		msg.Msg = uiMsgDelete;
		callbackObj->GUIcallback(this, msg);
	}
}

/** Catch attemp to drag on a swatch. If so, create a drag-drop object with that swatch's colour. */
void CGUIswatchGroup::onDrag(const int mouseX, const int mouseY) {
	if (dragDropObj == NULL) {
		int index = getSwatchIndex(mouseX, mouseY);
		if (index != -1) {
			i32vec4 colour = getSwatchColour(index);
			dragDropObj = new CGUIdragDropSwatch();
			static_cast<CGUIdragDropSwatch*>(dragDropObj)->colour = colour;
			static_cast<CGUIdragDropSwatch*>(dragDropObj)->swatchSize = swatchSize;
		}
		else
			MouseDown = false;//don't check for dragging again until the next LMouseDown

	}

}

/** Catch an attempt to drop a swatch on this group.*/
void CGUIswatchGroup::onDrop(const int mouseX, const int mouseY) {
	if (dragDropObj->isDropType(this)) {
		i32vec4 colour = static_cast<CGUIdragDropSwatch*>(dragDropObj)->colour;
		int index = getSwatchIndex(mouseX, mouseY);
		if (index == -1) {
			addSwatch(colour);
		}
		else {
			setSwatchColour(index,colour);
		}
	}
	deleteDragDropObj();
}



void CGUIswatchGroup::resize(int w, int h) {
	//do we need to adjust height to fit in existing swatches?
	int requiredHeight = calcRequiredHeight(w);
	CGUIbase::resize(w, requiredHeight);
	if (requiredHeight > h) {
		if (callbackObj) {
			CMessage msg;
			msg.Msg = uiMsgChildResize;
			callbackObj->GUIcallback(this, msg);
		}
	}
	calcRowColumnSize();
}

/** Add one or more colours to the group. */
void CGUIswatchGroup::addColours(std::vector<glm::i32vec4>* swatches) {
	calcRowColumnSize();
	if (swatches) {
		//this->swatches.assign(swatches->begin(), swatches->end());

		for (auto swatch : *swatches) {
			addSwatch(swatch);
		}
	}
//	calcRequiredHeight();
}

glm::i32vec4& CGUIswatchGroup::getSwatchColour(int swatchNo) {
	return swatches[swatchNo];
}

void CGUIswatchGroup::setSwatchColour(int & swatchNo, glm::i32vec4 & colour) {
	swatches[swatchNo] = colour;
}

/** Add a single new swatch to the group. */
void CGUIswatchGroup::addSwatch(i32vec4& newSwatch) {
	swatches.push_back(newSwatch);
	if (((swatches.size() - 1) / totalColumns) == totalRows) {
		int requiredHeight = calcRequiredHeight(drawBox.size.x);
		resize(drawBox.size.x, drawBox.size.y);
	}
}

void CGUIswatchGroup::addUserSwatch(i32vec4& newSwatch) {
	addSwatch(newSwatch);
	//tell container to open colour picjer
	if (callbackObj) {
		CMessage msg;
		msg.Msg = uiMsgDoubleClick;
		msg.value = swatches.size() - 1;
		callbackObj->GUIcallback(this, msg);
	}
}


void CGUIswatchGroup::calcRowColumnSize() {
	totalColumns = (drawBox.size.x ) / (swatchSize + swatchGap);
	totalRows = (drawBox.size.y - (nameTextbox->drawBox.size.y - swatchGap)) / (swatchSize + swatchGap);
}

/** Find the necessary height to fit in all existing swatches at this width. */
int CGUIswatchGroup::calcRequiredHeight(int width) {
	int availableColumns = (drawBox.size.x ) / (swatchSize + swatchGap);
	int totalSwatches = swatches.size();
	int requiredRows = ( (totalSwatches -1) / availableColumns) + 1;

	return (requiredRows * (swatchSize + swatchGap)) + nameTextbox->drawBox.size.y + surroundGap;
}

void CGUIdragDropSwatch::drawSelf(int mouseX, int mouseY) {
	guiRect swatchBox = { mouseX, mouseY, swatchSize,swatchSize };
	vec4 realColour = vec4(colour) / 255.0f;
	pDrawFuncs->drawRect2(swatchBox, realColour, realColour);
	pDrawFuncs->drawBorder2(swatchBox, uiLightGrey);

}
