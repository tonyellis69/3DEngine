#include "hex.h"



CHex::CHex() {
	x = 0; y = 0; z = 0;
}

/** Construct from cube coordinates. */
CHex::CHex(int x, int y, int z) {
	this->x = x; this->y = y; this->z = z;
}

/** Construct from axial coordinates. */
CHex::CHex(int q, int r) {
	x = q;
	z = r;
	y = -x - z;
}

/** Construct from a worldspace location.*/
CHex::CHex(glm::vec3& worldSpace) {
	

}

/** Return position in axial coordiantes. */
glm::i32vec2 CHex::getAxial() {
	return glm::i32vec2(x,z);
}


/**Convert cube coordinates to axial.*/
CHex cubeToAxial(CHex & cube) {
	return CHex(cube.x, cube.z);
}

/**Convert axial coordinates to cube (float version). */
glm::vec3 axialToCube(float q, float r) {
	float x = q;
	float z = r;
	float y = -x - z;
	return glm::vec3(x, y, z);
}

/** Round floating point coordinates to the nearest hex. */
CHex cubeRound(glm::vec3& cubePos) {
	int rx = round(cubePos.x);
	int ry = round(cubePos.y);
	int rz = round(cubePos.z);

	float x_diff = abs(rx - cubePos.x);
	float y_diff = abs(ry - cubePos.y);
	float z_diff = abs(rz - cubePos.z);

	//reset component with largest change. This ensures
	//x + y + z = 0 as it should.
	if (x_diff > y_diff&& x_diff > z_diff)
		rx = -ry - rz;
	else if (y_diff > z_diff)
		ry = -rx - rz;
	else
		rz = -rx - ry;

	return CHex(rx, ry, rz);
}

/** Convert fractional axial coordinates to integer axials .*/
CHex hexRound(float q, float r) {
	return cubeToAxial(cubeRound(axialToCube(q, r)));
}

CHex worldSpaceToHex(glm::vec3& worldSpace) {
	//find fractional position
	float q = sqrt(3.0f) / 3.0f * worldSpace.x - 1.0f / 3.0f * -worldSpace.y;
	float r = 2.0f / 3.0f * -worldSpace.y;

	return hexRound(q, r);
}
