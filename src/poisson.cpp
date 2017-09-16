#include "poisson.h"

#include <cstdlib>
#include <vector>
#include <list>

namespace pois {

#define PI 3.1415926535897

	glm::i32vec2 imageToGrid(glm::vec2& point, float cellSize) {
		int gridX = (int)(point.x / cellSize);
		int gridY = (int)(point.y / cellSize);
		return glm::i32vec2(gridX, gridY);
	}

	//non-uniform, favours points closer to the inner ring, leads to denser packings
	glm::vec2 generateRandomPointAround(glm::vec2& point, float mindist) {
		float r1 = (float)rand() / RAND_MAX; //random point between 0 and 1
		float r2 = (float)rand() / RAND_MAX;
		//random radius between mindist and 2 * mindist
		float radius = mindist * (r1 + 1);
		//random angle
		float angle = 2.0f * float(PI) * r2;
		//the new point is generated around the point (x, y)
		float newX = point.x + radius * cos(angle);
		float newY = point.y + radius * sin(angle);
		return glm::vec2(newX, newY);
	}


	bool inNeighbourhood(grid2D& grid, glm::vec2& point, float mindist, float cellSize) {
		glm::i32vec2 gridPoint = imageToGrid(point, cellSize);
		//get the neighbourhood if the point in the grid
		gridPoint -= glm::i32vec2(2, 2);

		for (int x = gridPoint.x; x < gridPoint.x+5; x++) {
			for (int y = gridPoint.y; y < gridPoint.y+5; y++) {
				glm::vec2 cell = *grid.cell(x, y);
				if (cell != glm::vec2(0)) {
					if (glm::distance(cell, point) < mindist)
						return true;
				}
			}
		}
		return false;
	}


	bool inRectangle(int width, int height, glm::vec2& point) {
		return glm::all(glm::greaterThanEqual(point, glm::vec2(0)))
			&& glm::all(glm::lessThan(point, glm::vec2(width, height)));
	}


	std::vector <glm::vec2> generate_poisson(int width, int height, float min_dist, int new_points_count) {
		//Create the grid
		float cellSize = min_dist / sqrt(2.0f);

		grid2D grid(int(width / cellSize), int(height / cellSize));

		std::vector <glm::vec2> processList; //random
		std::vector <glm::vec2> samplePoints;

	

		//generate the first point randomly
		glm::vec2 firstPoint = glm::vec2(rand() % width, rand() % height);

		//update containers
		processList.push_back(firstPoint);
		samplePoints.push_back(firstPoint);

		glm::i32vec2 index = imageToGrid(firstPoint, cellSize);
		*grid.cell(index.x,index.y) = firstPoint;

		//generate other points from points in queue.
		while (processList.size() > 0) {
			int randIndex = rand() % processList.size();
			glm::vec2 point = processList[randIndex];
			processList.erase(processList.begin() + randIndex);
			for (int i = 0; i < new_points_count; i++) {
				glm::vec2 newPoint = generateRandomPointAround(point, min_dist);
				//check that the point is in the image region
				//and no points exists in the point's neighbourhood
				if (inRectangle(width,height, newPoint) &&
					!inNeighbourhood(grid, newPoint, min_dist, cellSize)) {
					//update containers
					processList.push_back(newPoint);
					samplePoints.push_back(newPoint);
					glm::i32vec2 index = imageToGrid(newPoint, cellSize);
					*grid.cell(index.x, index.y ) = newPoint;
				}
			}
		}		return samplePoints;
	}



	grid2D::grid2D(int width, int height) {
		mWidth = width;
		mHeight = height;

		cells = new glm::vec2[width * height];
		for (int c = 0; c < width*height; c++)
			cells[c] = glm::vec2(0);
	}

	grid2D::~grid2D() {
		delete cells;
	}

	/** Return a pointer to the indexed cell, wrapping if necessary. */
	glm::vec2 * grid2D::cell(int x, int y) {
		if (x < 0)
			x = mWidth + x;
		if (y < 0)
			y = mHeight + y;

		if (x >= mWidth)
			x -= mWidth;
		if (y >= mHeight)
			y -= mHeight;

		return &cells[(y * mWidth) + x];
	}

}
