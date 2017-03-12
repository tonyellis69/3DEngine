#pragma once
#include <vector>
#include "TileLayer.h"

/** A class for managing the backdrops and tiled layers of a 2D scene. */
class C2DScene
{
public:
	C2DScene(void);
	~C2DScene(void);

	std::vector<CTileLayer> TileLayerList;
};
