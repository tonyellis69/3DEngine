#include "GUIpanelContainer.h"



int CGUIpanelContainer::Count = 1;



CGUIpanelContainer::CGUIpanelContainer(int x, int y, int w, int h) {
	GenName("PanelContainer",Count++);
	borderWidth = 1;
	horizontalBarActive = false;
	type = panelContainer;
	//localPos = glm::i32vec2(x, y);
	setPos(x, y);
	//width = w; height = h;
	setWidth(w);
	setHeight(h);
	
	createScrollbars();
	verticalBar->visible = false;
	horizontalBar->visible = false;
	createSurface();
	surface->setViewbox(borderWidth,borderWidth, w-(borderWidth*2),h-(borderWidth*2));
	surface->spaceAroundControls = 0;
	fitViewBoxToContainer();
	adaptToContents();
	wheelMultiplier = 10;
}

void CGUIpanelContainer::createSurface() {
	surface = new CGUIstackSurface();
	surface->backColour1 = UItransparent;
	surface->backColour2 = UItransparent;
	CGUIbase::Add(surface);
}


/** Add an empty panel to the container's surface. */
void CGUIpanelContainer::AddPanel(int size) {
	CGUIpanel* startPanel = new CGUIpanel(0,0,surface->viewBox.width,size);
	startPanel->drawBorder = false;
	startPanel->hFormat = hSpan;
	surface->addPanel(startPanel);
	
	adaptToContents();
	needsUpdate = true;
}

/** Add controls to the given panel.*/
void CGUIpanelContainer::Add(unsigned int panel, CGUIbase* child) {
	//check the panel exists
	if (panel > (surface->Control.size()-1))
		return; //Error!
	//add the child control
	surface->Control[panel]->Add(child);
}


/** Move the given panel into the slot, moving the rest up.*/
void  CGUIpanelContainer::movePanel(unsigned int panel, unsigned int slot) {
	if ((panel > surface->Control.size() -1) )// || (slot > surface->Control.size() -1))
		return; //not an existing panel
	
	CGUIpanel* panelPtr = (CGUIpanel*)surface->Control[panel];
	if (panel > slot)
		panel++;
	else
		slot++;

	if (slot >= surface->Control.size()) {
		surface->Control.push_back(panelPtr);
	} else
		surface->Control.insert(surface->Control.begin() +slot,panelPtr);
	//if (panel > slot)
	//	panel++;
	surface->Control.erase(surface->Control.begin() + panel);

	assignPanelPositions();
}

/** Assign each panel a position on the surface based on its position in the surface's list of controls. */
void CGUIpanelContainer::assignPanelPositions() {
	int x = 0; int y = 0;

	for (size_t p=0;p<surface->Control.size();p++) {
		surface->Control[p]->setLocalDimensions(x,y,surface->viewBox.width,surface->Control[p]->getHeight());
		y += surface->Control[p]->getHeight();
	}
}

/** Delete the given panel and all its controls. */
void CGUIpanelContainer::deletePanel(unsigned int panel) {
	if (panel > surface->Control.size() NONE ) //not an existing panel
		return;

	CGUIpanel* delPanel = (CGUIpanel*)surface->Control[panel];
	for (size_t  i=0;i < delPanel->Control.size();i++) {
		delete delPanel->Control[i];
		delPanel->Control.erase(delPanel->Control.begin()+i);

	}
	delete delPanel;
	surface->Control.erase(surface->Control.begin() + panel);
	
	assignPanelPositions();
	surface->encompassChildControls();
}

/** Returns a pointer to the given panel in this container. */
CGUIpanel* CGUIpanelContainer::getPanel(unsigned int panelNo) {
	if (panelNo > surface->Control.size() -1 ) //not an existing panel
		return NULL;
	return (CGUIpanel*)surface->Control[panelNo];
}



void CGUIstackSurface::addPanel(CGUIpanel* panel) {
	Add(panel);
	assignPanelPositions();
}

/** Assign each panel a position on the surface based on its position in the surface's list of controls. */
void CGUIstackSurface::assignPanelPositions() {
	int x = 0; int y = 0;

	for (size_t p=0;p<Control.size();p++) {
		Control[p]->setLocalDimensions(x,y,viewBox.width,Control[p]->getHeight());
		y += Control[p]->getHeight();
	}
}