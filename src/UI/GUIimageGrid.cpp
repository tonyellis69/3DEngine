#include "GUIimageGrid.h"


int CGUIimageGrid::Count = 1;
const int noTileset = -1;

CGUIimageGrid::CGUIimageGrid(int x, int y, int _cols, int _rows, int size) {
	GenName("ImageGrid",Count++);
	type = imageGrid;

	cols = _cols; rows = _rows; cellSize = size;
	setLocalDimensions(x,y,cols*cellSize,rows*cellSize);
	highlightColour = UIblue;
	selectColour = UIyellow;
	resizeGrid();
	tilesheet = noTileset;
	selected = NONE;
	borderWidth = 1;
	cellScale = 1;
}

/** Overloads function to find which row and column the mouse is in. */
void CGUIimageGrid::OnMouseMove(const int mouseX, const int mouseY, int key) {
	mouseCol = mouseX / cellSize; 
	mouseRow = mouseY / cellSize;
}

/** Set the given cell to the given value. */
void CGUIimageGrid::setCell(int cellNo, int value) {
	if (cellNo > cellCount -1)
		return; //error, but just return.
	cell[cellNo] = value;
}


void CGUIimageGrid::DrawSelf( ) {
//	pDrawFuncs->setDrawColours(foreColour1, foreColour1);
//	pDrawFuncs->drawBorder(screenPos.x,screenPos.y,width,height);

	//draw grid
	//for (int c=1;c<cols;c++)
//		pDrawFuncs->drawLine(screenPos.x+(c*cellSize),screenPos.y,screenPos.x+(c*cellSize),screenPos.y + height);

//	for (int r=1;r<rows;r++)
	//	pDrawFuncs->drawLine(screenPos.x,screenPos.y+(r*cellSize),screenPos.x+width,screenPos.y+(r*cellSize));

	UIrect cellBounds;

//	pDrawFuncs->setDrawColours(UIwhite, UIwhite);
	cellBounds.width = cellSize;
	cellBounds.height = cellSize;
//	pDrawFuncs->setScale(cellScale,cellScale);
	int cellNo = 0;
	//if (tilesheet != noTileset)
	//	pDrawFuncs->setIconset(tilesheet);
	for (int r=0;r<rows;r++) {
		for (int c=0;c<cols;c++) {
			cellBounds.x = getScreenPos().x + (c*cellSize);
			cellBounds.y = getScreenPos().y + (r*cellSize);
			if (cell[cellNo] != NONE) {
				if (tilesheet == noTileset) {
				//	pDrawFuncs->drawImage(cell[cellNo],cellBounds);
				}
				else { 
				//	pDrawFuncs->drawTile(cell[cellNo],cellBounds);
				}
			}
			cellNo++;
		}
	}
//	pDrawFuncs->setScale(1,1);

	//draw highlighter box around cell if it gets mouseovered
	if (MouseOver == this) {

		if (cell[getCellNo(mouseCol,mouseRow)] != NONE) {
//			pDrawFuncs->setDrawColours(highlightColour, highlightColour);
		//	pDrawFuncs->drawBorder(screenPos.x + (mouseCol * cellSize),screenPos.y + (mouseRow * cellSize),cellSize,cellSize);
		}
	}

	//draw selection box
	if (selected != NONE) {
		int selCol, selRow;
		getColRow(selected, selCol, selRow);
	//	pDrawFuncs->setDrawColours(selectColour, selectColour);
	//	pDrawFuncs->drawBorder(screenPos.x + (selCol * cellSize),screenPos.y + (selRow * cellSize),cellSize,cellSize);
	}
}

/** User has mouseuped on a cell; send a message saying which one.*/
void CGUIimageGrid::OnClick(const  int mouseX, const  int mouseY) {
	mouseCol = mouseX / cellSize; 
	mouseRow = mouseY / cellSize;
	int cellNo = getCellNo(mouseCol,mouseRow);
	if (cellNo != NONE) {
		Message.Msg = uiClick; //iMsgLMouseUp;
		Message.value = cell[cellNo];
	//	pDrawFuncs->handleUImsg(*this,Message);	
	}
	selected = Message.value;
}

/** Returns the number of the cell at the given row and column. */
int CGUIimageGrid::getCellNo(int x, int y) {
	if (x <0 || x >= cols || y < 0 || y >= rows)
		return NONE;
	return (y * cols) + x ;
}

/** Resize the grid with the given number of rows. */
void CGUIimageGrid::setRows(int newRows) {
	rows = newRows;
	resizeGrid();
}

/** Resize the grid with the given number of columns. */
void CGUIimageGrid::setColumns(int newCols) {
	cols = newCols;
	resizeGrid();
}

/** Resize the grid with the cell size set to the given number of pixels.*/
void CGUIimageGrid::setCellSize(int size) {
	cellSize = size;
	resizeGrid();
}

/** Recalculate the width/height of the grid. */
void CGUIimageGrid::resizeGrid() {
	cellCount = rows * cols;
	cell.resize(cellCount,-1);
	setLocalDimensions(getLocalPos().x, getLocalPos().y,cols*cellSize,rows*cellSize);
}

/** Return the cell number of the first empty cell, if any.*/
int CGUIimageGrid::firstEmptyCell() {
	for (int c=0;c < (rows*cols);c++) {
		if (cell[c] == -1)
			return c;
	}
	return NONE;
}

/** Returns the row and column of the given cell number. */ 
void CGUIimageGrid::getColRow(int cellNo, int& col, int& row) {
	row = cellNo / cols;
	col = cellNo % cols;
}



/*
CGUIimageGrid::~CGUIimageGrid() {

}
*/

CGUIgridMouse::CGUIgridMouse() {
	id = uiGridMouseID;
	tilesheet = NULL;
}

/** Draws the image being dragged from this image grid, at the current mouse position. */
void CGUIgridMouse::DrawSelf( ) {
	imageBounds.x = mouse->getScreenPos().x;
	imageBounds.y = mouse->getScreenPos().y;
//	pDrawFuncs->setDrawColours(UIwhite, UIwhite);
//	if (tilesheet == NULL)
	//	pDrawFuncs->drawImage(dragData,imageBounds);
	//else {
	//	pDrawFuncs->setIconset(tilesheet);
	//	pDrawFuncs->drawTile(dragData,imageBounds);
	//}
}


