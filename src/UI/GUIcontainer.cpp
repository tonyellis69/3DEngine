#include "GUIcontainer.h"

using namespace glm;

int CGUIcontainer::Count = 1;
const int defaultSpaceAroundControls = 0;// 20;


CGUIsysContainer::CGUIsysContainer(int x, int y, int w, int h){
	type = container;
	borderWidth = 1;
	setLocalPos(x, y);
	setWidth(w);
	setHeight(h);

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
	verticalBar = new CGUIsysScrollbar(vertical,0,1,getHeight()-barWidth);
	verticalBar->Value = getHeight();
	verticalBar->hFormat = hRight;
	verticalBar->id = uiContainerVbarID;
	verticalBar->setMax(getHeight());
	verticalBar->setMin(0);
	CGUIbase::Add(verticalBar);

	horizontalBar = new CGUIsysScrollbar(horizontal,1, getHeight()-barWidth,getWidth()-barWidth);
	horizontalBar->vFormat = vBottom;
	horizontalBar->Value = 0;
	horizontalBar->id = uiContainerHbarID;
	horizontalBar->setMax(getWidth());
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
	needsUpdate = true;
}


/** Handles a scroll message from the container's scrollbars. */
void CGUIsysContainer::message(CGUIbase* sender, CMessage& msg) {
	if (sender->id == uiContainerVbarID) {
		surface->setPosY( 0 - (verticalBar->Max - msg.value) );
		needsUpdate = true;
	}
	if (sender->id == uiContainerHbarID) {
		surface->setPosX(  0 - msg.value);
		needsUpdate = true;
	}
	if (msg.Msg == uiMsgChildResize)
		adaptToContents();

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
	pDrawFuncs->drawBorder2(drawBox, uiDarkGrey);
}


void CGUIsysContainer::updateAppearance() {
	CGUIbase::updateAppearance();
	// container may have been resized. Surface might now be too big for the viewbox, or viewbox so big bars no longer needed
	//or, a child control may have grown, necessitating scrollbars.
	adaptToContents();
}

/** Activates the vertical scrollbar or adjusts the existing one, if the container contents
	dictate it. Returns true if the scrollbar is newly activated. */
bool CGUIsysContainer::verticalBarCheck() {
	//first, check if scrollbar is even needed. 
	if (surface->getHeight() > surface->viewBox.height) { //surface bigger than viewbox
		bool newBar = false;
		//so it is needed. 1st, if it's only just becoming visible now, reset slider to default positionHint
		if (!verticalBar->visible) {
			verticalBar->setMax(surface->getHeight() - surface->viewBox.height);
			verticalBar->visible = true;
			verticalBar->Value = verticalBar->Max;
			newBar = true;
		}
		//second, ensure it's the right size and slider size for the current container/surface ratio
		verticalBar->setHeight( getHeight() - barWidth -(2*borderWidth));
		//because the ratio of surface to viewbox size may have been resized, we need to preserve the slider's relative positionHint.
		float diff = (float) verticalBar->Max - verticalBar->Min;
		float ratio = diff / verticalBar->Value;

		verticalBar->setMax(surface->getHeight() - (surface->viewBox.height + borderWidth));
		verticalBar->Value = (int)(verticalBar->Max / ratio);
		verticalBar->setSliderSize((float) surface->viewBox.height / surface->getHeight());  
		verticalBar->updateSliderAppearance();
		//adust scrolling of surface accordingly:
		surface->setPosY(surface->viewBox.y - (verticalBar->Max - verticalBar->Value) );
		
		return newBar;

	}
	else {
		verticalBar->visible = false;
		surface->setPosY( surface->viewBox.y);
		return false;
	}
}

/** Activates the horizontal scrollbar or adjusts the existing one, if the container contents
	dictate it. Returns true if the scrollbar is newly activated. */
bool CGUIsysContainer::horizontalBarCheck() {
	//first, check if scrollbar is even needed. 
	if ((surface->getWidth() > surface->viewBox.width) && (horizontalBarActive)) { //surface wider than viewbox
		bool newBar = false;
		//so it is needed. 1st, if it's only just become visible now, reset slider to default positionHint
		if (!horizontalBar->visible) {
			horizontalBar->setMax(surface->getWidth() - surface->viewBox.width);
			horizontalBar->visible = true;
			horizontalBar->Value = 0;
			bool newBar = true;
		}
		//second, ensure it's the right size and slider size for the current container/surface ratio
		horizontalBar->setWidth( getWidth() - barWidth);
		//because the ratio of surface to viewbox size may have been resized, we need to preserve the slider's relative positionHint.
		float diff = (float)(horizontalBar->Max - horizontalBar->Min);
		float ratio = diff / horizontalBar->Value;

		horizontalBar->setMax(surface->getWidth() - surface->viewBox.width);
		horizontalBar->setSliderSize((float) surface->viewBox.width / surface->getWidth());  
		horizontalBar->updateSliderAppearance();
		surface->setPosX(surface->viewBox.x - horizontalBar->Value);
		return newBar;
	}
	else {
		horizontalBar->visible = false;
		surface->setPosX( surface->viewBox.x);
		return false;
	}
}

/** Return a pointer to the child control with the given id number. */
CGUIbase* CGUIsysContainer::getChild(int childID) {
	for (size_t i=0;i<surface->controls.size();i++){
		if ( surface->controls[i]->id == childID )
			return surface->controls[i];
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
}

/** Returns the index number of the panel holding the given child control. */
int CGUIsysContainer::findControlPanel(CGUIbase* child) {
	for (size_t panelNo = 0; panelNo < surface->controls.size();panelNo++) {
		CGUIpanel* panel = (CGUIpanel*) surface->controls[panelNo];
		if (panel->findControl(child) != NULL)
			return panelNo;
	}
	return NONE;
}

//TO DO: needs second version that looks for ID.


/** Adjust viewbox to fit its container, accounting for scrollbars, borders, etc.*/ 
void CGUIsysContainer::fitViewBoxToContainer() {
	int viewWidth = getWidth()-(borderWidth*2);
	int viewHeight = getHeight()-(borderWidth*2);
	if (verticalBar->visible) 
		viewWidth -= barWidth;
	if (horizontalBar->visible) 
		viewHeight -= barWidth;
	surface->setViewbox(borderWidth, borderWidth, viewWidth,viewHeight);
}

/** Respond to contents, if any, by resizing the surface and adding scrollbars as
	required. */
void CGUIsysContainer::adaptToContents() {
	surface->encompassChildControls();
	//do we now need scrollbars?
	verticalBarCheck();
	if (horizontalBarCheck()) 
		verticalBarCheck();
	
	fitViewBoxToContainer();
}





CGUIbaseSurface::CGUIbaseSurface()  {
	spaceAroundControls = defaultSpaceAroundControls;
	type = surface;
}

/** Overload the default clipbox calculation, basing it on the rectangle described by viewBox rather than
	the dimensions of the parent container itself. */
void CGUIbaseSurface::recalculateClipbox() {
	UIrect parentClipbox = parent->Clipbox;
	
	Clipbox.width = viewBox.width ; 
	Clipbox.x = parent->drawBox.pos.x + viewBox.x ;
	if (Clipbox.x <parentClipbox.x) {
		Clipbox.width -= (parentClipbox.x -Clipbox.x); //reduce width by amount it's been clipped
		Clipbox.x = parentClipbox.x ;
	}

	Clipbox.height = viewBox.height;// -2; 
	Clipbox.y = parent->drawBox.pos.y + viewBox.y;
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

	for (size_t i = 0; i < controls.size(); i++) {
		child = controls[i];
		if (child->anchorBottom == NONE) { //TO DO: may want to disqualify for use of vFormat too
			newHeight = max(newHeight, child->getLocalPos().y + child->getHeight() + spaceAroundControls);
		}
		if (child->anchorRight == NONE) {//TO DO: and hFormat
			newWidth = max(newWidth, child->getLocalPos().x + child->getWidth() + spaceAroundControls);
		}
	}

	if ((getHeight() != newHeight) || (getWidth() != newWidth)) {
		parent->needsUpdate = true;
	}

	setWidth(newWidth);
	setHeight(newHeight);
}



/** Set the viewbox to the given size and positionHint on the container.*/
void CGUIbaseSurface::setViewbox(int x, int y, int w, int h) {
	viewBox.x = x; viewBox.y = y; viewBox.width = w;viewBox.height = h;
}

/** Here we can catch something like a menu resizing itself. */
void CGUIbaseSurface::message(CGUIbase * sender, CMessage & msg) {
	if (msg.Msg == uiMsgChildResize)
		parent->message(this, msg);
}

bool CGUIbaseSurface::onDoubleClick(const int mouseX, const int mouseY, int key) {
	CMessage msg;
	msg.Msg = uiMsgDoubleClick;
	parent->message(this, msg);
	return true;
}


CGUIcontainer::CGUIcontainer(int x, int y, int w, int h) : CGUIsysContainer(x,y,w,h) {
	GenName("Container",Count++);
	borderWidth = 1;
}


