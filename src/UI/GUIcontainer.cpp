#include "GUIcontainer.h"

int CGUIcontainer::Count = 1;
const int defaultSpaceAroundControls = 20;


CGUIsysContainer::CGUIsysContainer(int x, int y, int w, int h){
	type = container;
	borderWidth = 1;
	localPos = glm::i32vec2(x, y);
	width = w; height = h;
	horizontalBarActive = false;
	createScrollbars();
	verticalBar->visible = false;
	horizontalBar->visible = false;
	createSurface();
	fitViewBoxToContainer(); //establish base viewbox
	adaptToContents(); //ensure surface fits
	wheelMultiplier = 10;
}

/** Create the scrollbar controls for this container, and make them children. */
void CGUIsysContainer::createScrollbars() {
	verticalBar = new CGUIsysScrollbar(vertical,0,1,height-barWidth);
	verticalBar->Value = height;
	verticalBar->hFormat = hRight;
	verticalBar->id = uiContainerVbarID;
	verticalBar->setMax(height);
	verticalBar->setMin(0);
	CGUIbase::Add(verticalBar);

	horizontalBar = new CGUIsysScrollbar(horizontal,1,height-barWidth,width-barWidth);
	horizontalBar->vFormat = vBottom;
	horizontalBar->Value = 0;
	horizontalBar->id = uiContainerHbarID;
	horizontalBar->setMax(width);
	horizontalBar->setMin(0);
	CGUIbase::Add(horizontalBar);
}

/** Create the 'surface' control to which the container's contents are attached. */
void CGUIsysContainer::createSurface() {
	surface = new CGUIsurface();
	CGUIbase::Add(surface);
}


/** Overloads the base Add method, so that child controls are added to the container's 'surface' control.*/
void CGUIsysContainer::Add(CGUIbase* child) {
	surface->Add(child);
	adaptToContents(); //resize surface as required, adding scrollbars
	fitViewBoxToContainer();
	needsUpdate = true;
}


/** Handles a scroll message from the container's scrollbars. */
void CGUIsysContainer::message(CGUIbase* sender, CMessage& msg) {
	if (sender->id == uiContainerVbarID) 
		surface->localPos.y =  0 - (verticalBar->Max - msg.value);

	if (sender->id == uiContainerHbarID) 
		surface->localPos.x = 0 - msg.value;
	needsUpdate = true;
}


/** Overloads the default handler to scroll the container. */
bool CGUIsysContainer::MouseWheelMsg(const  int mouseX, const  int mouseY, int wheelDelta, int key) {
	//first, see if any child controls can handle this mousewheel
	if (CGUIbase::MouseWheelMsg(mouseX,mouseY,wheelDelta, key))
		return true;
	
	if (verticalBar->visible) {
		verticalBar->SliderPos += (wheelDelta * wheelMultiplier);
		verticalBar->updateValue();
		return true;
	}
	return false;
}

void CGUIsysContainer::DrawSelf( ) {
	pDrawFuncs->setDrawColours(UIlightGrey, UIlightGrey);
	pDrawFuncs->drawBorder(screenPos.x,screenPos.y,width,height);
}


void CGUIsysContainer::updateAppearance() {
	CGUIbase::updateAppearance();
	// container may have been resized. Surface might now be too big for the viewbox, or viewbox so big bars no longer needed
	//or, a child control may have grown, necessitating scrollbars.
	
	adaptToContents();
	fitViewBoxToContainer();
}

/** Activates the vertical scrollbar or adjusts the existing one, if the container contents
	dictate it. Returns true if the scrollbar is newly activated. */
bool CGUIsysContainer::verticalBarCheck() {
	//first, check if scrollbar is even needed. 
	if (surface->height > surface->viewBox.height) { //surface bigger than viewbox
		bool newBar = false;
		//so it is needed. 1st, if it's only just becoming visible now, reset slider to default position
		if (!verticalBar->visible) {
			verticalBar->setMax(surface->height - surface->viewBox.height);
			verticalBar->visible = true;
			verticalBar->Value = verticalBar->Max;
			newBar = true;
		}
		//second, ensure it's the right size and slider size for the current container/surface ratio
		verticalBar->height = height - barWidth -(2*borderWidth);
		//because the ratio of surface to viewbox size may have been resized, we need to preserve the slider's relative position.
		float diff = (float) verticalBar->Max - verticalBar->Min;
		float ratio = diff / verticalBar->Value;

		verticalBar->setMax(surface->height - surface->viewBox.height);
		verticalBar->Value = (int)(verticalBar->Max / ratio);
		verticalBar->setSliderSize((float) surface->viewBox.height / surface->height);  
		verticalBar->updateSliderAppearance();
		//adust scrolling of surface accordingly:
		surface->localPos.y =  surface->viewBox.y - (verticalBar->Max - verticalBar->Value) ;
		return newBar;

	}
	else {
		verticalBar->visible = false;
		surface->localPos.y = surface->viewBox.y;
		return false;
	}
}

/** Activates the horizontal scrollbar or adjusts the existing one, if the container contents
	dictate it. Returns true if the scrollbar is newly activated. */
bool CGUIsysContainer::horizontalBarCheck() {
	//first, check if scrollbar is even needed. 
	if ((surface->width > surface->viewBox.width) && (horizontalBarActive)) { //surface wider than viewbox
		bool newBar = false;
		//so it is needed. 1st, if it's only just become visible now, reset slider to default position
		if (!horizontalBar->visible) {
			horizontalBar->setMax(surface->width - surface->viewBox.width);
			horizontalBar->visible = true;
			horizontalBar->Value = 0;
			bool newBar = true;
		}
		//second, ensure it's the right size and slider size for the current container/surface ratio
		horizontalBar->width = width - barWidth;
		//because the ratio of surface to viewbox size may have been resized, we need to preserve the slider's relative position.
		float diff = (float)(horizontalBar->Max - horizontalBar->Min);
		float ratio = diff / horizontalBar->Value;

		horizontalBar->setMax(surface->width - surface->viewBox.width);
		horizontalBar->setSliderSize((float) surface->viewBox.width / surface->width);  
		horizontalBar->updateSliderAppearance();
		surface->localPos.x = surface->viewBox.x - horizontalBar->Value;
		return newBar;
	}
	else {
		horizontalBar->visible = false;
		surface->localPos.x = surface->viewBox.x;
		return false;
	}
}

/** Return a pointer to the child control with the given id number. */
CGUIbase* CGUIsysContainer::getChild(int childID) {
	for (size_t i=0;i<surface->Control.size();i++){
		if ( surface->Control[i]->id == childID )
			return surface->Control[i];
	}
	return NULL;
}

/** Sets the gap between controls and the edge of the surface. */
void CGUIsysContainer::setControlMargin(int newMargin) {
	surface->spaceAroundControls = newMargin;
}

/** A convenience function to control the vertical scrollbar directly. */
void CGUIsysContainer::vScroll(int scroll) {
	if (verticalBar->visible) {
		verticalBar->SliderPos += scroll;
		verticalBar->updateValue();
	}

//	controlPanels->verticalBar->SliderPos = 0;
//		controlPanels->verticalBar->updateValue();
}

/** Returns the index number of the panel holding the given child control. */
int CGUIsysContainer::findControlPanel(CGUIbase* child) {
	for (size_t panelNo = 0; panelNo < surface->Control.size();panelNo++) {
		CGUIpanel* panel = (CGUIpanel*) surface->Control[panelNo];
		if (panel->findControl(child) != NULL)
			return panelNo;
	}
	return NONE;
}

//TO DO: needs second version that looks for ID.


/** Adjust viewbox to fit its container, accounting for scrollbars, borders, etc.*/ 
void CGUIsysContainer::fitViewBoxToContainer() {
	int viewWidth = width-(borderWidth*2);
	int viewHeight = height-(borderWidth*2);
	if (verticalBar->visible) 
		viewWidth -= barWidth;
	if (horizontalBar->visible) 
		viewHeight -= barWidth;
	surface->setViewbox(borderWidth,borderWidth, viewWidth,viewHeight);
}

/** Respond to contents, if any, by resizing the surface and adding scrollbars as
	required. */
void CGUIsysContainer::adaptToContents() {
	surface->encompassChildControls();
	//do we now need scrollbars?
	verticalBarCheck();
	if (horizontalBarCheck()) 
		verticalBarCheck();
	
	//child controls may now be clipped, so an updateAppearance is needed.
	needsUpdate = true;
}





CGUIbaseSurface::CGUIbaseSurface()  {
	spaceAroundControls = defaultSpaceAroundControls;
	type = surface;
}

/** Overload the default clipbox calculation, basing it on the rectangle described by viewBox. */
void CGUIbaseSurface::CalculateClipbox() {
	UIrect parentClipbox = parent->Clipbox;

	Clipbox.width = viewBox.width ; 
	Clipbox.x = parent->screenPos.x + viewBox.x ;
	if (Clipbox.x <parentClipbox.x) {
		Clipbox.width -= (parentClipbox.x -Clipbox.x); //reduce width by amount it's been clipped
		Clipbox.x = parentClipbox.x ;
	}

	Clipbox.height = viewBox.height;// -2; 
	Clipbox.y = parent->screenPos.y + viewBox.y;
	if (Clipbox.y < parentClipbox.y) {
		Clipbox.height -= (parentClipbox.y -Clipbox.y); //reduce height by amount it's been clipped
		if (Clipbox.height < 0)
			Clipbox.height=0;
		Clipbox.y = parentClipbox.y;
	}

	if ((Clipbox.x + Clipbox.width) > (parentClipbox.x + parentClipbox.width) ) 
		Clipbox.width -= (Clipbox.x + Clipbox.width) - (parentClipbox.x + parentClipbox.width);
	
	if ((Clipbox.y + Clipbox.height) > (parentClipbox.y + parentClipbox.height))
		Clipbox.height -= (Clipbox.y + Clipbox.height) - (parentClipbox.y + parentClipbox.height);

	if (Clipbox.height < 0) //Fixes leaking error, TO DO: may need this elsewhere
			Clipbox.height=0;
	if (Clipbox.width < 0)
			Clipbox.width=0;
}




/** Adjust surface to the smallest size that encompasses any controls it holds, without being smaller
	than the viewbox. */
void CGUIbaseSurface::encompassChildControls() {
	int newHeight = viewBox.height; //Ensure surface never smaller than viewbox
	int newWidth = viewBox.width;
	CGUIbase* child;

	for (size_t i=0;i<Control.size();i++) {
		child =Control[i];
		if ((child->localPos.y + child->height + spaceAroundControls ) > newHeight)
			newHeight = child->localPos.y + child->height + spaceAroundControls;
		if ((child->localPos.x +child->width + spaceAroundControls) > newWidth)
			newWidth = child->localPos.x + child->width + spaceAroundControls;
	}

	if ((height != newHeight) || (width != newWidth))
		parent->needsUpdate = true;
	height = newHeight;
	width = newWidth;
	
}



/** Set the viewbox to the given size and position on the container.*/
void CGUIbaseSurface::setViewbox(int x, int y, int w, int h) {
	viewBox.x = x; viewBox.y = y; viewBox.width = w;viewBox.height = h;
}



void CGUIbaseSurface::DrawSelf( ) {

}


CGUIcontainer::CGUIcontainer(int x, int y, int w, int h) : CGUIsysContainer(x,y,w,h) {
	GenName("Container",Count++);
	borderWidth = 1;
}