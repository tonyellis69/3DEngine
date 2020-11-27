#include "GUIpanelContainer.h"



int CGUIpanelContainer::Count = 1;



CGUIpanelContainer::CGUIpanelContainer(int x, int y, int w, int h) {
	GenName("PanelContainer",Count++);
	borderWidth = 1;
	horizontalBarActive = false;
	type = panelContainer;
	setLocalPos(x, y);
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
	CGUIbase::add(surface);
}


/** add an empty panel to the container's surface. */
void CGUIpanelContainer::addPanel(int size) {
	CGUIpanel* startPanel = new CGUIpanel(0,0,surface->viewBox.width,size);
	startPanel->drawBorder = false;
	startPanel->hFormat = hSpan;
	surface->addPanel(startPanel);
	
	adaptToContents();
	needsUpdate = true;
}

/** add controls to the given panel.*/
void CGUIpanelContainer::add(unsigned int panel, CGUIbase* child) {
	//check the panel exists
	if (panel > (surface->controls.size()-1))
		return; //Error!
	//add the child control
	surface->controls[panel]->add(child);
}


/** Move the given panel into the slot, moving the rest up.*/
void  CGUIpanelContainer::movePanel(unsigned int panel, unsigned int slot) {
	if ((panel > surface->controls.size() -1) )// || (slot > surface->controls.size() -1))
		return; //not an existing panel
	
	CGUIpanel* panelPtr = (CGUIpanel*)surface->controls[panel];
	if (panel > slot)
		panel++;
	else
		slot++;

	if (slot >= surface->controls.size()) {
		surface->controls.push_back(panelPtr);
	} else
		surface->controls.insert(surface->controls.begin() +slot,panelPtr);

	surface->controls.erase(surface->controls.begin() + panel);

	assignPanelPositions();
}

/** Assign each panel a positionHint on the surface based on its positionHint in the surface's list of controls. */
void CGUIpanelContainer::assignPanelPositions() {
	int x = 0; int y = 0;

	for (size_t p=0;p<surface->controls.size();p++) {
		surface->controls[p]->setLocalDimensions(x,y,surface->viewBox.width,surface->controls[p]->getHeight());
		y += surface->controls[p]->getHeight();
	}
}

/** Delete the given panel and all its controls. */
void CGUIpanelContainer::deletePanel(unsigned int panel) {
	if (panel > surface->controls.size() NONE ) //not an existing panel
		return;

	CGUIpanel* delPanel = (CGUIpanel*)surface->controls[panel];
	for (size_t  i=0;i < delPanel->controls.size();i++) {
		delete delPanel->controls[i];
		delPanel->controls.erase(delPanel->controls.begin()+i);

	}
	delete delPanel;
	surface->controls.erase(surface->controls.begin() + panel);
	
	assignPanelPositions();
	surface->encompassChildControls();
}

/** Returns a pointer to the given panel in this container. */
CGUIpanel* CGUIpanelContainer::getPanel(unsigned int panelNo) {
	if (panelNo > surface->controls.size() -1 ) //not an existing panel
		return NULL;
	return (CGUIpanel*)surface->controls[panelNo];
}



void CGUIstackSurface::addPanel(CGUIpanel* panel) {
	add(panel);
	assignPanelPositions();
}

/** Assign each panel a positionHint on the surface based on its positionHint in the surface's list of controls. */
void CGUIstackSurface::assignPanelPositions() {
	int x = 0; int y = 0;

	for (size_t p=0;p<controls.size();p++) {
		controls[p]->setLocalDimensions(x,y,viewBox.width,controls[p]->getHeight());
		y += controls[p]->getHeight();
	}
}