#pragma once

#include <vector> 
#include <unordered_map>

#include "hexElement.h"
#include "hex.h"
#include "hexObject.h"

typedef std::unordered_map<glm::i32vec2, CHex> TCameFrom; 

struct TMapEffects {
	float fog;
	float visibility;
	float highlight;
};

/** A 2D container for CHexElement objects. */
class CHexArray {
public:
	CHexArray() {};
	void init(int w, int h);
	//void setEntityList(THexObjs* pEntities);
	CHexElement& getHexOffset(int x, int y);
	CHexElement& getHexAxial(int q, int r);
	CHexElement& getHexCube(const CHex& cube);
	glm::vec3 getWorldPos(CHex& hex);
	THexList breadthFirstPath(CHex& start, CHex& end);
	THexList dijkstraPath(CHex& start, CHex& end);
	THexList aStarPath(CHex& start, CHex& end);
	THexList aStarPath(CHex& start, CHex& end, bool fogCheck);
	THexList aStarPath(CHex& start, CHex& end, CHex& omit);
	THexList aStarPath(CHex& start, CHex& end, bool fogCheck, CHex& omit);
	bool outsideArray(CHex& hex);
	CHex findLineEnd(CHex& start, CHex& target);
	THexList findLineHexes(CHex& start, CHex& end);
	bool inLineOfSight(CHex& start, CHex& end);
	bool inLineOfSight2(CHex& start, CHex& end);
	CHex indexToCube(int x, int y);
	CHex indexToCube(glm::i32vec2& index);
	glm::i32vec2 cubeToIndex(CHex& hex);
	bool isValidPath(CHex& start, CHex& end);
	void clear();
	void setFog(int x, int y, float fog);
	void setFog(CHex& hex, float fog);
	float getFog(CHex& hex);

	void setHighlight(CHex& hex, float highlight);

	void setVisibility(CHex& hex, float visibility);
		 
	int width;
	int height;

	glm::vec3 worldPosCornerDist; ///<Dist from origin to corner.

	std::vector<TMapEffects> effectsData; ///<Fog state, etcc, for each hex of the map.
	bool effectsNeedUpdate = true; ///<Notifies that effects need redrawing.


private:
	THexList walkBack(CHex& start, CHex& end, TCameFrom& cameFrom);
	virtual bool fromToBlocked(CHex& from, CHex& to) {
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

const int solidHex = 2;
const int emptyHex = 1;

