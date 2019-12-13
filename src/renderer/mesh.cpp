#include "mesh.h"


void CMesh::calculateVertexNormals() {
	normals.resize(vertices.size());
	//for each face, find the normal,
	//make that the normal for the verts
	for (unsigned int v = 0; v < vertices.size(); v += 3) {
		glm::vec3 edge1 = vertices[indices[v]] - vertices[indices[v + 1]];
		glm::vec3 edge2 = vertices[indices[v + 2]] - vertices[indices[v + 1]];
		glm::vec3 normal = glm::cross(edge2, edge1);
		normals[indices[v]] = normals[indices[v + 1]] = normals[indices[v + 2]] = normal;

	}
}

/** Write the vert data to the given graphics buffer object. */
void CMesh::writeToBufferv3i(CBuf& buf) {
	buf.storeVertexes(vertices.data(), sizeof(glm::vec3) * vertices.size(), vertices.size());
	buf.storeIndex(indices.data(), indices.size());
	buf.storeLayout(3, 0, 0, 0);
}
//TO DO: look into making a general purpose template-based writeToBuffer


/** Clear all vertex buffers etc so mesh can be resused. */
void CMesh::clear() {
	vertices.clear();
	normals.clear();
	indices.clear();
	texCoords.clear();
}