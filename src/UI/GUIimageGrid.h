#pragma once
#include "GUIbase.h"
#include "GUImouse.h"


/** A control for displaying images in a grid format. */
class CGUIimageGrid :  public CGUIbase {
public:
	CGUIimageGrid();
	CGUIimageGrid(int x, int y, int _cols, int _rows, int size);
	//~CGUIimageGrid();
	void DrawSelf();
//	void OnLMouseUp(const  int mouseX, const  int mouseY);
	void OnClick(const  int mouseX, const  int mouseY);

	void OnMouseMove(const int mouseX, const int mouseY, int key);
	void setCell(int cellNo, int value);
	void setRows(int newRows);
	void setColumns(int newCols);
	void setCellSize(int size);
	void resizeGrid();
	int firstEmptyCell();
	int getCellNo(int x, int y);
	void getColRow(int cellNo, int& col, int& row);

	static int Count;
	int cols; ///<Number of columns the grid has.
	int rows; ///<Number of rows the grid has/
	int cellSize; ///<Size of each cell in pixls.
	float cellScale; ///<scaling applied to the cell images.

	UIcolour highlightColour;
	UIcolour selectColour;
	
	std::vector<int> cell; ///<A vist of values held in each cell.
	int tilesheet; ///<tilesheet used, if any. -1 means grid holds individual images instead,
	int selected; ///<Identifies the currently selected cell, if any.

private:
	int mouseCol; ///<X positionHint of the highlit cell.
	int mouseRow;  ///<Y positionHint of the highlit cell.
	int cellCount; ///<Number of cells.

}
;

/** Helper mouse subclass for dragging items from an image grid. */
class CGUIgridMouse : public CGUImouse {
public:
	CGUIgridMouse();
	void DrawSelf( );

	UIrect imageBounds; ///<Position width etc of image.
	int tilesheet; ///<Tilesheet to use if mouse dragging a tile.
};
