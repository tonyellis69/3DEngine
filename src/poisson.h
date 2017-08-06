#pragma once

#include <glm/glm.hpp>
#include <vector>

namespace pois {

	class grid2D {
	public:
		grid2D(int width, int height);
		~grid2D();
		glm::vec2* cell(int x, int y);

		int mWidth, mHeight;
		glm::vec2* cells;
	};

//	glm::i32vec2 imageToGrid(glm::vec2& point, float cellSize);
//	glm::vec2 generateRandomPointAround(glm::vec2& point, float mindist);
	std::vector <glm::vec2> generate_poisson(int width, int height, float min_dist, int new_points_count);
}