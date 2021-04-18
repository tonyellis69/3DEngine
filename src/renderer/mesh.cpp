#include "mesh.h"
#include <map>
#include <unordered_map>
#include <algorithm>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

CMesh::CMesh() {

}

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
//void CMesh::exportToBuffer(CBuf& buf) {
//	buf.storeVertexes(vertices.data(), sizeof(glm::vec3) * vertices.size(), vertices.size());
//	buf.storeIndex(indices.data(), indices.size());
//	buf.storeLayout(3, 0, 0, 0);
//
//}

void CMesh::exportToBuffer(CBuf2& buf) {
	buf.storeVerts(vertices, indices, 3);
}
//TO DO: look into making a general purpose template-based writeToBuffer


/** Clear all vertex buffers etc so mesh can be resused. */
void CMesh::clear() {
	vertices.clear();
	normals.clear();
	indices.clear();
	texCoords.clear();
}

/** Remove duplicate vertices and update the indices accordingly. */
void CMesh::mergeUniqueVerts() {
	//create a new list of unique verts
	std::unordered_map<glm::vec3, int> uniqueVertsIndexed;
	int index = 0; int oldIndex = 0;

	for (auto v : vertices) {
		int newIndex;

		auto merged = uniqueVertsIndexed.find(v);
		if (merged == uniqueVertsIndexed.end()) {
			newIndex = index;
			uniqueVertsIndexed[v] = index++;
		}
		else
			newIndex = merged->second;

		std::replace(indices.begin(), indices.end(), oldIndex, newIndex);
		oldIndex++;
	}
	std::vector<glm::vec3> uniqueVerts(uniqueVertsIndexed.size());
	for (auto v : uniqueVertsIndexed) {
		uniqueVerts[v.second] = v.first;
	}

	vertices = uniqueVerts;
}

/** Convert a bunch of individual lines to line strips. Merge them
	together if they share vertexes, and separate them with the
	primitive restart value if not. */
void CMesh::linesToLineStrip() {
	orderLines();

	unsigned int primitiveRestart = 0xFFFF;
	if (vertices.size() > 0xFFFE)
		primitiveRestart = 0xFFFFFFFF;

	//run through index line pairs
	std::vector<unsigned int> stripIndices;
	for (unsigned int i = 0; i < indices.size(); i++) {
		stripIndices.push_back(indices[i]);
		if (i % 2 && i < indices.size() - 1) { //second vertex of a line pair
			if (indices[i] == indices[i + 1])
				i++; //skip next vertex
			else {
				stripIndices.push_back(primitiveRestart);
			}
		}
	}

	indices = stripIndices;
}

/** Ensure loops end at their starting vertex, and insert primitive restarts 
	for every new loop in a sequence of line loops. */
void CMesh::closeLineLoops(std::vector<unsigned int>& loopSizes) {
	unsigned int primitiveRestart = 0xFFFF;
	if (vertices.size() > 0xFFFE)
		primitiveRestart = 0xFFFFFFFF;

	unsigned int loopNo = 0;
	unsigned int vertNo = 1;
	unsigned int startingVert = indices[0];
	std::vector<unsigned int> restartIndices;
	for (unsigned int i = 0; i < indices.size(); i++) {		
		restartIndices.push_back(indices[i]);
		if (loopNo < loopSizes.size()  && vertNo == loopSizes[loopNo]) {
			restartIndices.push_back(startingVert);
			restartIndices.push_back(primitiveRestart);
			loopNo++;
			vertNo = 1;
			startingVert = indices[i + 1];
		}
		else
			vertNo++;
	}
	restartIndices.push_back(startingVert);

	indices = restartIndices;
}


/** add an extra point at the start and end of each line.*/
void CMesh::addAdjacencyVerts() {
	unsigned int primitiveRestart = 0xFFFF;
	if (vertices.size() > 0xFFFE)
		primitiveRestart = 0xFFFFFFFF;

	std::vector<unsigned int> adjacencyIndices;

	adjacencyIndices.push_back(makeAdjacencyVert(vertices[indices[0]],vertices[indices[1]]));

	auto lineStart = indices.begin(); //position in the indices vector of the line's starting index
	unsigned int lineAdjacencyA = 0;

	for (auto linePos = indices.begin(); linePos != indices.end(); linePos++) {
		if (*linePos == primitiveRestart) {
			if (*(linePos - 1) == *lineStart) { //this line loops
				adjacencyIndices[lineAdjacencyA] = *(linePos - 2);
				adjacencyIndices.push_back(*(lineStart + 1));
			}
			else
				adjacencyIndices.push_back(makeAdjacencyVert(vertices[*(linePos - 1)], vertices[*(linePos - 2)]));

			adjacencyIndices.push_back(*linePos);
			adjacencyIndices.push_back(makeAdjacencyVert(vertices[*(linePos + 1)], vertices[*(linePos + 2)]));

			lineAdjacencyA = adjacencyIndices.size() - 1;
			lineStart = linePos + 1;
		}
		else
			adjacencyIndices.push_back(*linePos);
	}

	if (indices.back() == *lineStart) { //this line loops
		adjacencyIndices[lineAdjacencyA] = indices.end()[-2];
		adjacencyIndices.push_back(*(lineStart+1));
	}
	else
		adjacencyIndices.push_back(makeAdjacencyVert(vertices[indices.back()], vertices[indices.end()[-2]]));


	indices = adjacencyIndices;
}

unsigned int CMesh::makeAdjacencyVert(glm::vec3& A, glm::vec3& B) {
	glm::vec3 adjacencyVert = 2.0f * A - B;
	vertices.push_back(adjacencyVert);
	return vertices.size() - 1;
}





/** Append the given mesh, updating its index data in the process. */
TMeshRec CMesh::add(CMesh& mesh) {
	TMeshRec meshRec;
	meshRec.indexStart = indices.size();
	meshRec.indexSize = mesh.indices.size();
	meshRec.vertStart = vertices.size();

	vertices.insert(vertices.end(), mesh.vertices.begin(), mesh.vertices.end());
	for (auto index : mesh.indices) {
		indices.push_back(index + meshRec.vertStart);
	}

	return meshRec;
}




/** Sort line-pair vertexes to form as long a sequence as possible. This is
	to prep them for converting into line strips. */
void CMesh::orderLines() {
	std::vector<unsigned int> orderedIndices; std::vector<unsigned int> rejects;
	for (auto linePairA = indices.begin(); linePairA != indices.end(); linePairA += 2) {
		bool inserted = false;
		for (auto orderedPairA = orderedIndices.begin(); orderedPairA != orderedIndices.end(); orderedPairA += 2) {
			if (*(linePairA + 1) == (*orderedPairA)) { //pair fits before this pair
				orderedIndices.insert(orderedPairA, linePairA, linePairA+ 2);
				inserted = true;
				break;
			}

			if (*(linePairA) == (*orderedPairA)) { //try to fit pair by reversing it
				auto it = orderedIndices.insert(orderedPairA, *linePairA);
				orderedIndices.insert(it, *(linePairA + 1));
				inserted = true;
				break;
			}

			if (*(linePairA+ 1) == *(orderedPairA + 1)) { //try to fit pair by reversing and adding at end
				auto it = orderedIndices.insert(orderedPairA + 2, *(linePairA + 1));
				orderedIndices.insert(it + 1, *linePairA);
				inserted = true;
				break;
			}

		}
		if (!inserted)
			orderedIndices.insert(orderedIndices.end(), linePairA, linePairA+ 2);
	}

	indices = orderedIndices;
}

