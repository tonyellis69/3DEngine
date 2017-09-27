#pragma once

#include <vector>

#include "glm\glm.hpp"


namespace shape {
	/* A convenience library for creating shapes such as cubes. */


	void cube(std::vector<glm::vec3>& verts, std::vector<glm::vec3>& normals, std::vector<unsigned int>& index) {
		float u = 0.5f;

		glm::vec3 A(-u, u, u);
		glm::vec3 B(u, u, u);
		glm::vec3 C(u, -u, u);
		glm::vec3 D(-u, -u, u);
		glm::vec3 E(-u, u, -u);
		glm::vec3 F(u, u, -u);
		glm::vec3 G(u, -u, -u);
		glm::vec3 H(-u, -u, -u);

		std::vector<glm::vec3> v(24);
		std::vector<glm::vec3> normal(24);

		v[0] = A; v[1] = B; v[2] = C; v[3] = D;//front face
		normal[0] = normal[1] = normal[2] = normal[3] = glm::vec3(0, 0, 1);
		v[4] = B; v[5] = F; v[6] = G; v[7] = C;//right face
		normal[4] = normal[5] = normal[6] = normal[7] = glm::vec3(1, 0, 0);
		v[8] = F; v[9] = E; v[10] = H; v[11] = G;//back face
		normal[8] = normal[9] = normal[10] = normal[11] = glm::vec3(0, 0, -1);
		v[12] = E; v[13] = A; v[14] = D; v[15] = H;//left face
		normal[12] = normal[13] = normal[14] = normal[15] = glm::vec3(-1, 0, 0);
		v[16] = E; v[17] = F; v[18] = B; v[19] = A;//top face
		normal[16] = normal[17] = normal[18] = normal[19] = glm::vec3(0, 1, 0);
		v[20] = D; v[21] = C; v[22] = G; v[23] = H;//bottom face
		normal[20] = normal[21] = normal[22] = normal[23] = glm::vec3(0, -1, 0);

		index = { 1, 0, 3, 1, 3, 2,
			5, 4, 7, 5, 7, 6,
			9, 8, 11, 9, 11, 10,
			13, 12, 15, 13, 15, 14,
			16, 19, 18, 18, 17, 16,
			21, 20, 23, 21, 23, 22 };

		verts = v;
		normals = normal;
	}

	/** Scale the given vector of verts. */
	void scale(std::vector<glm::vec3>& verts, glm::vec3& scale) {
		for (auto i : verts) {
			i *= scale;
		}
	}






}