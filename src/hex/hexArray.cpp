#include "hexArray.h"

#include <algorithm>"
#include <queue>
#include <unordered_map>

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/hash.hpp"


/** Initialise the array to the given dimensions. */
void CHexArray::init(int w, int h) {
	flatArray.clear();
	flatArray.resize(w * h);
	width = w; height = h;
	float xStep = sqrt(3);
	float yStep = 1.5f;
	bbTL = glm::vec3(0);
	bbBR = glm::vec3(0);

	glm::vec3 origin (w * 0.5f * xStep, h * 0.5f * yStep, 0.0f);
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			glm::vec3 offset = glm::vec3(x * xStep, y * yStep, 0) - origin;
			offset += glm::vec3(xStep * 0.5f, 0, 0) * float(y & 1);
			getHex(x, y).position = offset;
			getHex(x, y).content = 1; //TO DO: temp!


			//TO DO: also temp - do this more elegantly!
		}

	}
	bbTL = origin * glm::vec3(-1,-1,-1);
	bbBR = origin * glm::vec3(1,1,-1);

}

/** Return the hex at the given 2D coordinates. */
CHexElement& CHexArray::getHex(int x, int y) {
	x = std::clamp(x, 0, width - 1);
	y = std::clamp(y,0, height - 1);
	return flatArray[y * width + x];
}

/** Return the worldspace position of this hex. */
glm::vec3 CHexArray::getWorldPos(CHex& hex) {
	glm::i32vec2 offset = cubeToOffset(hex);
	offset.x += width / 2;
	offset.y = -offset.y + height / 2;

	return getHex(offset.x, offset.y).position;
}

/**Return a path from start to end using the breadth-first algorithm. */
THexList CHexArray::breadthFirstPath(CHex& start, CHex& end) {
	std::queue<CHex>  frontier;
	frontier.push(start);

	std::unordered_map<glm::i32vec2,CHex> cameFrom;


	while (!frontier.empty()) {
		CHex current = frontier.front();
		frontier.pop();

		//for each neighbour of this hex
		for (int dir = 0; dir < 6; dir++) {
			CHex& next = getNeighbour(current, (THexDir)dir);
			
			//check for obstacles here
			if (abs(next.getAxial().x) > width / 2 || abs(next.getAxial().y) > height / 2)
				break;

			if (cameFrom.find(next.getAxial()) == cameFrom.end() ) {
				frontier.push(next);
				cameFrom[next.getAxial()] = current;
			}
		}
	}

	CHex current = end;
	THexList path;
	while (current != start) {
		path.push_back(current);
		current = cameFrom[current.getAxial()];
	}
	std::reverse(path.begin(),path.end());


	return path;
}