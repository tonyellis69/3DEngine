#pragma once

#include <vector> 
#include <unordered_map>

#include "hexElement.h"
#include "hex.h"
#include "hexObject.h"

typedef std::unordered_map<glm::i32vec2, CHex> TCameFrom; 

/** A 2D container for CHexElement objects. */
class CHexArray {
public:
	CHexArray() {};
	void init(int w, int h);
	//void setEntityList(THexObjs* pEntities);
	CHexElement& getHexOffset(int x, int y);
	CHexElement& getHexAxial(int q, int r);
	CHexElement& getHexCube(CHex& cube);
	glm::vec3 getWorldPos(CHex& hex);
	THexList breadthFirstPath(CHex& start, CHex& end);
	THexList dijkstraPath(CHex& start, CHex& end);
	THexList aStarPath(CHex& start, CHex& end);
	bool outsideArray(CHex& hex);
	CHex findLineEnd(CHex& start, CHex& target);
	CHex indexToCube(int x, int y);
	CHex indexToCube(glm::i32vec2& index);
	glm::i32vec2 cubeToIndex(CHex& hex);
		 
	int width;
	int height;

	glm::vec3 worldPosCornerDist; ///<Dist from origin to corner.

private:
	THexList walkBack(CHex& start, CHex& end, TCameFrom& cameFrom);
	virtual bool entityBlockCheck(CHex& hex) {
		return false;
	};

	std::vector<CHexElement> flatArray;




};



/** Used in pathfinding priority queue: */

typedef std::pair<int, CHex> PQElement;
struct order {
	bool operator()(PQElement const& a, PQElement const& b) const {
		return a.first > b.first;
	}
};

