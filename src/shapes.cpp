#include "shapes.h"



namespace shape {

	using namespace std;

	void cube(std::vector<glm::vec3>* verts, std::vector<glm::vec3>* normals, std::vector<unsigned int>* index) {
		cube(verts, normals, NULL, index, 1.0f);
	}

	void cube(std::vector<glm::vec3>* verts, std::vector<glm::vec3>* normals,std::vector<glm::vec2>* texCoords, std::vector<unsigned int>* index, float topScale) {
		float u = 0.5f;

		glm::vec3 A(-u * topScale, u, u * topScale);//
		glm::vec3 B(u * topScale, u, u * topScale);//
		glm::vec3 C(u, -u, u);
		glm::vec3 D(-u, -u, u);
		glm::vec3 E(-u * topScale , u, -u * topScale);//
		glm::vec3 F(u * topScale, u, -u * topScale);//
		glm::vec3 G(u, -u, -u);
		glm::vec3 H(-u, -u, -u);


		*verts = { A, B, C, D, //front face
		B, F, G, C, //right face
		F, E, H, G, //back face
		E, A, D, H, //left face
		E, F, B, A, //top face
		D, C, G, H }; //bottom face

		if (texCoords)
			*texCoords = {	{0,0}, { 1,0 }, { 1,1 }, { 0,1},
						{0, 0}, { 1,0 }, { 1,1 }, { 0,1},
						{0, 0}, { 1,0 }, { 1,1 }, { 0,1},
						{0, 0}, { 1,0 }, { 1,1 }, { 0,1},
						{0, 0}, { 1,0 }, { 1,1 }, { 0,1},
						{0, 0}, { 1,0 }, { 1,1 }, { 0,1} };
	
	
		//TO DO: get rid of this when we get rid of the old CModel and use CMesh everywhere
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
			*index = { 1, 0, 3, 3, 2, 1,
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
	}

	void cylinder( CMesh& mesh, float r, float h, int steps) {
		float step = 360.0f / steps;

		const int noVerts = (steps * 4) + 2;
		const int topDisc = 0; const int botDisc = steps; const int top = steps * 2;
		const int bot = steps * 3; const int topCent = steps * 4; const int botCent = topCent + 1;

		mesh.vertices.resize(noVerts);
		mesh.normals.resize(noVerts);
		//rotate through the segments, and create 4 rings of vertices:
		//top, where all the norms point up, base where they all point down,
		//and 2 for the body where they point out,
		//then create indexes 
		float angle = 0; float rangle = 0;
		for (int seg = 0; seg < steps; seg++) {
			rangle = glm::radians(angle);
			mesh.vertices[topDisc + seg] = glm::vec3(sin(rangle)*r, h, cos(rangle)*r);
			mesh.normals[topDisc + seg] = glm::vec3(0, 1, 0);
			mesh.vertices[botDisc + seg] = glm::vec3(sin(rangle)*r, 0, cos(rangle)*r);
			mesh.normals[botDisc + seg] = glm::vec3(0, -1, 0);
			mesh.vertices[top + seg] = glm::vec3(sin(rangle)*r, h, cos(rangle)*r);
			mesh.normals[top + seg] = glm::vec3(sin(rangle), 0, cos(rangle));
			mesh.vertices[bot + seg] = glm::vec3(sin(rangle)*r, 0, cos(rangle)*r);
			mesh.normals[bot + seg] = glm::vec3(sin(rangle), 0, cos(rangle));
			angle += step;
		}
		mesh.vertices[topCent] = glm::vec3(0, h, 0); mesh.normals[topCent] = glm::vec3(0, 1, 0);
		mesh.vertices[botCent] = glm::vec3(0, 0, 0); mesh.normals[botCent] = glm::vec3(0, -1, 0);

		const int noTriangles = steps * 4;
		mesh.indices.resize(noTriangles * 3);
		int i = 0;
		const int finalSeg = steps - 1;
		for (int seg = 0; seg < finalSeg; seg++) {
			mesh.indices[i++] = topDisc + seg; mesh.indices[i++] = topDisc + seg + 1; mesh.indices[i++] = topCent;
			mesh.indices[i++] = botDisc + seg; mesh.indices[i++] = botCent; mesh.indices[i++] = botDisc + seg + 1;
			mesh.indices[i++] = bot + seg; mesh.indices[i++] = bot + seg + 1; mesh.indices[i++] = top + seg + 1;
			mesh.indices[i++] = top + seg + 1; mesh.indices[i++] = top + seg; mesh.indices[i++] = bot + seg;
		}

		mesh.indices[i++] = topDisc + finalSeg; mesh.indices[i++] = topDisc; mesh.indices[i++] = topCent;
		mesh.indices[i++] = botDisc + finalSeg; mesh.indices[i++] = botCent; mesh.indices[i++] = botDisc;
		mesh.indices[i++] = bot + finalSeg; mesh.indices[i++] = bot; mesh.indices[i++] = top;
		mesh.indices[i++] = top; mesh.indices[i++] = top + finalSeg; mesh.indices[i++] = bot + finalSeg;


	}

	/** Scale the given vector of verts. */
	void scale(std::vector<glm::vec3>& verts, glm::vec3& scale) {
		for (auto& i : verts) {
			i *= scale;
		}
	}

	/** Create a cube stored in a CMesh object and return it .*/
	CMesh cubeMesh() {
		CMesh mesh;
		cube(&mesh.vertices, &mesh.normals, &mesh.indices);
		mesh.type = meshTrisIndexed;
		mesh.calculateVertexNormals();
		return mesh;
	}

	/** Create a fustrum in a CMesh object and return it. */
	CMesh fustrumMesh(float gradient) {
		CMesh mesh;
		cube(&mesh.vertices, &mesh.normals, &mesh.texCoords, &mesh.indices,gradient);

		mesh.type = meshTrisIndexed;
		return mesh;
	}

	/** Create a cylinder stored in a CMesh object and return it .*/
	CMesh cylinderMesh(float radius, float height, float steps) {
		CMesh mesh;
		cylinder(mesh,radius,height,steps);
		mesh.type = meshTrisIndexed;
		//mesh.calculateVertexNormals();
		return mesh;
	}
}