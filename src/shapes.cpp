#include "shapes.h"



namespace shape {

	using namespace std;
	void cube(std::vector<glm::vec3>* verts, std::vector<glm::vec3>* normals, std::vector<unsigned int>* index) {
		float u = 0.5f;

		glm::vec3 A(-u, u, u);
		glm::vec3 B(u, u, u);
		glm::vec3 C(u, -u, u);
		glm::vec3 D(-u, -u, u);
		glm::vec3 E(-u, u, -u);
		glm::vec3 F(u, u, -u);
		glm::vec3 G(u, -u, -u);
		glm::vec3 H(-u, -u, -u);


		*verts = { A, B, C, D, //front face
		B, F, G, C, //right face
		F, E, H, G, //back face
		E, A, D, H, //left face
		E, F, B, A, //top face
		D, C, G, H }; //bottom face
	
		if (normals) {
			normals->resize(24);
			(*normals)[0] = (*normals)[1] = (*normals)[2] = (*normals)[3] = glm::vec3(0, 0, 1);
			(*normals)[4] = (*normals)[5] = (*normals)[6] = (*normals)[7] = glm::vec3(1, 0, 0);
			(*normals)[8] = (*normals)[9] = (*normals)[10] = (*normals)[11] = glm::vec3(0, 0, -1);
			(*normals)[12] = (*normals)[13] = (*normals)[14] = (*normals)[15] = glm::vec3(-1, 0, 0);
			(*normals)[16] = (*normals)[17] = (*normals)[18] = (*normals)[19] = glm::vec3(0, 1, 0);
			(*normals)[20] = (*normals)[21] = (*normals)[22] = (*normals)[23] = glm::vec3(0, -1, 0);
		}

		if (index) 
			*index = { 1, 0, 3, 1, 3, 2,
			5, 4, 7, 5, 7, 6,
			9, 8, 11, 9, 11, 10,
			13, 12, 15, 13, 15, 14,
			16, 19, 18, 18, 17, 16,
			21, 20, 23, 21, 23, 22 };
	}

	void cubePoints(std::vector<glm::vec3>* verts, std::vector<unsigned int>* index) {
		float u = 0.5f;

		glm::vec3 A(-u, u, u);
		glm::vec3 B(u, u, u);
		glm::vec3 C(u, -u, u);
		glm::vec3 D(-u, -u, u);
		glm::vec3 E(-u, u, -u);
		glm::vec3 F(u, u, -u);
		glm::vec3 G(u, -u, -u);
		glm::vec3 H(-u, -u, -u);


		*verts = { A, B, C, D, E, F, G, H };

		/*		if (index)
					*index = { 1, 0, 3, 1, 3, 2,
					5, 4, 7, 5, 7, 6,
					9, 8, 11, 9, 11, 10,
					13, 12, 15, 13, 15, 14,
					16, 19, 18, 18, 17, 16,
					21, 20, 23, 21, 23, 22 };
			} */

	}




	/** Scale the given vector of verts. */
	void scale(std::vector<glm::vec3>& verts, glm::vec3& scale) {
		for (auto& i : verts) {
			i *= scale;
		}
	}

	/** Create a cube stored in a CMesh object and return it .*/
	CMesh<glm::vec3> cubeMesh() {
		CMesh<glm::vec3> mesh;
		cube(&mesh.vertices, &mesh.normals, &mesh.indices);
		mesh.type = meshTrisIndexed;
		return mesh;
	}

}