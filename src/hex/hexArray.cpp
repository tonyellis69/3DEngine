#include "hexArray.h"

#include <algorithm>"
#include <queue>
#include <unordered_map>
#include <functional>

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/hash.hpp"

#include "../utils/log.h"


/** Initialise the array to the given dimensions. */
void CHexArray::init(int w, int h) {
	flatArray.clear();
	flatArray.resize(w * h);
	width = w; height = h;
	float xStep = sqrt(3);
	float yStep = 1.5f;

	glm::vec3 origin (w * 0.5f * xStep, h * 0.5f * yStep, 0.0f);
	glm::i32vec2 intOrigin(w * 0.5f, h * 0.5f);
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			//glm::vec3 offset = glm::vec3(x * xStep, (h-y) * yStep, 0) - origin;
			//offset += glm::vec3(xStep * 0.5f, 0, 0) * float(y & 1);
			//getHexOffset(x, y).position = offset;
			glm::i32vec2 offset = glm::i32vec2(x, y) - intOrigin;

			glm::vec3 worldPos(0);
			worldPos.x = (offset.x + (0.5f * (offset.y & 1))) * hexWidth;
			worldPos.y = -offset.y * 1.5f;


			getHexOffset(x, y).position = worldPos;
		}

	}
}



/** Return a reference to the hex at the given offset coordinates. */
CHexElement& CHexArray::getHexOffset(int x, int y) {
	x = std::clamp(x, 0, width - 1);
	y = std::clamp(y,0, height - 1);
	return flatArray[y * width + x];
}

/** Return a reference to the hex at the given axial coordinates.*/
CHexElement& CHexArray::getHexAxial(int q, int r) {
	glm::i32vec2 offset = axialToOffset(q, r);
	offset.x += width / 2;
	offset.y = offset.y + height / 2;
	return getHexOffset(offset.x, offset.y);
}

/** Return a reference to the hex at the given cube coordinates.*/
CHexElement& CHexArray::getHexCube(CHex& cube) {
	glm::i32vec2 offset = cubeToOffset(cube);
	offset.x += width / 2;
	offset.y = offset.y + height / 2;
	return getHexOffset(offset.x, offset.y);
}

/** Return the worldspace position of this hex. */
glm::vec3 CHexArray::getWorldPos(CHex& hex) {
	glm::i32vec2 offset = cubeToOffset(hex);
	offset.x += width / 2;
	offset.y = offset.y + height / 2;

	return getHexOffset(offset.x, offset.y).position;
}

/**Return a path from start to end using the breadth-first algorithm. */
THexList CHexArray::breadthFirstPath(CHex& start, CHex& end) {
	std::queue<CHex>  frontier;
	frontier.push(start);

	std::unordered_map<glm::i32vec2,CHex> cameFrom;
	cameFrom[start.getAxial()] = start;

	while (!frontier.empty()) {
		CHex current = frontier.front();
		frontier.pop();

		if (current == end)
			 break;

		//for each neighbour of this hex
		for (int dir = 0; dir < 6; dir++) {
			CHex& next = getNeighbour(current, (THexDir)dir);
			
			if (getHexCube(next).content == 2)
				continue; //solid hex

			if (outsideArray(next))
				continue;

			if (cameFrom.find(next.getAxial()) == cameFrom.end()) {
				frontier.push(next);
				cameFrom[next.getAxial()] = current;
			}

		}
	}

	return walkBack(start, end, cameFrom);
}

/**Return a path from start to end using Dijkstra's algorithm. */
THexList CHexArray::dijkstraPath(CHex& start, CHex& end) {
	std::priority_queue<PQElement, std::vector<PQElement>, order> frontier;
	frontier.push({ 0,start });

	TCameFrom cameFrom;
	std::unordered_map<glm::i32vec2, int> costSoFar;
	cameFrom[start.getAxial()] = start;
	costSoFar[start.getAxial()] = 0;

	const int findCost = 1;
	while (!frontier.empty()) {
		CHex current = frontier.top().second;
		frontier.pop();

		if (current == end)
			 break;

		//for each neighbour of this hex
		for (int dir = 0; dir < 6; dir++) {
			CHex& next = getNeighbour(current, (THexDir)dir);

			if (outsideArray(next))
				continue;

			if (getHexCube(next).content == 2)
				continue; //solid hex

			int newCost = costSoFar[current.getAxial()] + findCost;

			if (costSoFar.find(next.getAxial()) == costSoFar.end() || newCost < costSoFar[next.getAxial()]) {
				costSoFar[next.getAxial()] = newCost;
				frontier.push({newCost,next });
				cameFrom[next.getAxial()] = current;
			}
		}
	}

	return walkBack(start,end,cameFrom);
}

/**Return a path from start to end using the A* algorithm. */
THexList CHexArray::aStarPath(CHex& start, CHex& end) {
	std::priority_queue<PQElement, std::vector<PQElement>, order> frontier;
	frontier.push({ 0,start });

	TCameFrom cameFrom;
	std::unordered_map<glm::i32vec2, int> costSoFar;
	cameFrom[start.getAxial()] = start;
	costSoFar[start.getAxial()] = 0;

	int count = 0;

	CHex closest = start;
	bool success = false;

	const int findCost = 1;
	while (!frontier.empty()) {
		CHex current = frontier.top().second;
		frontier.pop();

		if (current == end) {
			success = true;
			break;
		}

		//for each neighbour of this hex
		for (int dir = 0; dir < 6; dir++) {
			CHex next;
			if (current.x & 1 || current.z & 1)
				next = getNeighbour(current, (THexDir)dir);
			else
				next = getNeighbour(current, (THexDir)(5-dir));

			if (outsideArray(next))
				continue;

			if (getHexCube(next).content == 2)
				continue; //solid hex

			if (entityCheck(next) == true /* && next != end*/)
				continue;



			int newCost = costSoFar[current.getAxial()] + findCost;

			if (costSoFar.find(next.getAxial()) == costSoFar.end() || newCost < costSoFar[next.getAxial()]) {
				costSoFar[next.getAxial()] = newCost;

				int distToGoal = cubeDistance(next, end);
				if (distToGoal < cubeDistance(closest, end))
					closest = next;

				int priority = newCost + distToGoal;

				frontier.push({ priority,next });
				cameFrom[next.getAxial()] = current;
			}
		}
		count++;
	}

	THexList path;
	if (success)
		path = walkBack(start, end, cameFrom);
	else
		path = walkBack(start, closest, cameFrom);
	return path;
}

/** Returns true if this hex lies outside the array dimensions.*/
bool CHexArray::outsideArray(CHex& hex) {
	glm::i32vec2 offset = cubeToOffset(hex);
	offset.x += width / 2;
	offset.y = offset.y + height / 2;

	if (offset.x >= width || offset.x < 0 || offset.y >= height || offset.y < 0)
		return true;

	return false;
}

/** Returns the hex at which the line from start, passing through target,
	hits a solid object. */
CHex CHexArray::findLineEnd(CHex& start, CHex& target) {
	int N = cubeDistance(start, target);

	glm::vec3 A(start.x, start.y, start.z);
	A += glm::vec3(1e-6, 2e-6, -3e-6);
	glm::vec3 B(target.x, target.y, target.z);

	glm::vec3 travelVector = (B - A) / float(N);

	CHex travelHex;
	float dist = 1.0f;
	do {
		glm::vec3 travelPoint = A + (travelVector * dist);
		travelHex = hexRound(travelPoint);
		dist += 1.0f;
	} while (getHexCube(travelHex).content != 2 && entityCheck(travelHex) == false);

	return travelHex;
}

///////////////PRIVATE//////////////

/** Utility func to return a journey from a pathfinding results list.*/
THexList CHexArray::walkBack(CHex& start, CHex& end, TCameFrom& cameFrom) {
	CHex current = end;
	THexList path;
	while (current != start) {
		path.push_back(current);
		current = cameFrom[current.getAxial()];
	}
	std::reverse(path.begin(), path.end());
	return path;
}

