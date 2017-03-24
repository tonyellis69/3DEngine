#include "direction.h"

using namespace glm;

Tdirection flipDir(Tdirection dir) {
	switch (dir) {
	case north : return south;
	case east : return west;
	case south : return north;
	case west : return east;
	case up : return down;
	case down : return up;
	}
}



glm::vec3 dirToVec(Tdirection dir) {
	switch (dir) {
		case north : return vec3(0,0,-1);
		case east : return vec3(1,0,0);
		case south : return vec3(0,0,1);
		case west : return vec3(-1,0,0);
		case up: return vec3(0,1,0);
		case down: return vec3(0,-1,0);
	}
}

glm::i32vec3 rotateByDir(i32vec3 vec, Tdirection dir) {
	i32vec3 rVec;
	switch (dir) {
		case north : rVec = vec; break;
		case east : rVec.x = vec.z; rVec.y = vec.y; rVec.z = vec.x; break;
		case south :  rVec = vec; break;
		case west : rVec.x = vec.z; rVec.y = vec.y; rVec.z = vec.x; break;
		case up: rVec.x = vec.x; rVec.y = vec.z; rVec.z = vec.y; break;
		case down: rVec.x = vec.x; rVec.y = vec.z; rVec.z = vec.y; break;
	}
	return rVec;
}

int  getAxis(Tdirection dir) {
	if ((dir == north)||(dir == south))
		return 2;
	if ((dir == east)||(dir == west))
		return 0;
	return 1;
}

int getAxis(i32vec3& vec) {
	if (vec[2] != 0)
		return 2;
	if (vec[0] != 0)
		return 0;
	return 1;
}

Tdirection vecToDir(glm::i32vec3& vec) {
	if (vec.x == 1)
		return east;
	if (vec.x == -1)
		return west;
	if (vec.y == 1)
		return up;
	if (vec.y == -1)
		return down;
	if (vec.z == 1)
		return south;
	if (vec.z == -1)
		return north;
}

int getSign(Tdirection dir) {
	glm::i32vec3 vec = dirToVec(dir);
	int axis = getAxis(dir);
	return vec[axis];
}

int getXaxis(Tdirection dir) {
	if ((dir == east) || (dir == west))
		return 2;
	return 0;
}

int getYaxis(Tdirection dir) {
	if ((dir == up) || (dir == down))
		return 2;
	return 1;
}

Tdirection getXstart(Tdirection dir) {
	if ((dir == west) || (dir == east))
		return north;
	return west;
}

Tdirection getYstart(Tdirection dir) {
	if ((dir == up) || (dir == down))
		return north;
	return down;
}

Tdirection getXdir(Tdirection dir) {
	if ((dir == west) || (dir == east))
		return south;
	return east;
}

Tdirection getYdir(Tdirection dir) {
	if ((dir == up) || (dir == down))
		return south;
	return up;
}