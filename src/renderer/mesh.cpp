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


CBuf2 CMesh::exportToBuffer() {
	CBuf2 buf;
	buf.storeVerts(vertices, indices, 3);
	return buf;
}


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

	for (auto& v : vertices) {
		int newIndex;

		auto& merged = uniqueVertsIndexed.find(v);
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
	for (auto& v : uniqueVertsIndexed) {
		uniqueVerts[v.second] = v.first;
	}

	vertices = uniqueVerts;
}

/** Convert a bunch of individual lines to line strips. Merge them
	together if they share vertexes, and separate them with the
	primitive restart value if not. */
void CMesh::linesToLineStrip() {
//	orderLines();

	unsigned int primitiveRestart = 0xFFFF;
	if (vertices.size() > 0xFFFE)
		primitiveRestart = 0xFFFFFFFF;

	//int stripCount = 1;
	////run through index line pairs
	//std::vector<unsigned int> stripIndices;
	//for (unsigned int i = 0; i < indices.size(); i++) {
	//	stripIndices.push_back(indices[i]);
	//	if (i % 2 && i < indices.size() - 1) { //second vertex of a line pair
	//		if (indices[i] == indices[i + 1])
	//			i++; //skip next vertex
	//		else {
	//			stripIndices.push_back(primitiveRestart);
	//			stripCount++;
	//		}
	//	}
	//}

	//find all pairs
	struct TLinePair {
		unsigned int A;
		unsigned int B;
	};
	std::vector<TLinePair> pairs;
	for (unsigned int i = 0; i < indices.size(); i += 2) {
		TLinePair linePair = { indices[i], indices[i + 1] };
		pairs.push_back(linePair);
	}

	//for each pair...
	std::vector<std::vector<TLinePair>> lines;
	for (unsigned int p = 0; p < pairs.size(); p++) {
		TLinePair currentPair = pairs[p];
		bool found = false;
		for (int attempt = 0; attempt < 2; attempt++) {
			for (auto& line : lines) { //...can it be added to an existing line?
				if (currentPair.B == line.front().A) {
					line.insert(line.begin(), currentPair);
					found = true;
					break;
				}
				else if (currentPair.A == line.back().B) {
					line.push_back(currentPair);
					found = true;
					break;
				}
			}
			if (found)
				break;
			std::swap(currentPair.A, currentPair.B);
		}

		if (!found) { 	//...no? start a new line
			std::vector<TLinePair> newLine = { currentPair };
			lines.push_back(newLine);
		}
	}

	//combine lines if possible
		for (auto& line = lines.begin(); line != lines.end();) {
			bool found = false;
			for (auto& lineB = line + 1; lineB != lines.end(); lineB++) {
				if (line->front().A == lineB->back().B) {
					lineB->insert(lineB->end(), line->begin(), line->end());
					line = lines.erase(line);
					found = true;
					break;
				}
			}
			if (!found)
				line++;
		}

	std::vector<unsigned int> sortedIndices;
	for (auto& line : lines) {
		for (auto& pair : line) {
			sortedIndices.push_back(pair.A);
		}
		sortedIndices.push_back(line.back().B);
		sortedIndices.push_back(primitiveRestart);
	}
	sortedIndices.erase(sortedIndices.end()-1); //don't need a final restart


	//indices = stripIndices;
	indices = sortedIndices;
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

	//TO DO: can do all this much more cleanly if I break the indices back into lines
	//check each to see if it loops, create adjacency verts accordingly. 

	std::vector<std::vector<unsigned int>> lines(1);
	for (unsigned int i = 0; i < indices.size(); i++) {
		if (indices[i] == primitiveRestart) {
			lines.resize(lines.size() + 1);
		} 
		else
			lines.back().push_back(indices[i]);
	}


	for (auto& line : lines) {
		if (line.front() == line.back()) { //this line loops
			line.insert(line.begin(), *(line.end() -2) );
			line.push_back( *(line.begin() + 2) );
		}
		else {
			line.push_back(makeAdjacencyVert(vertices[line.back()], vertices[*(line.end() - 2)])); //end adjacency vert
			line.insert(line.begin(), makeAdjacencyVert(vertices[line[0]], vertices[line[1]]) ); //leading adjacency vert
		}
	}

	//convert lines back to one stream of indices
	for (auto& line : lines) {
		adjacencyIndices.insert(adjacencyIndices.end(), line.begin(), line.end());
		adjacencyIndices.push_back(primitiveRestart);
	}
	//adjacencyIndices.erase(adjacencyIndices.end() - 1); //don't need a final restart

	indices = adjacencyIndices;

	return;



	adjacencyIndices.push_back(makeAdjacencyVert(vertices[indices[0]],vertices[indices[1]]));

	auto lineStart = indices.begin(); //position in the indices vector of the line's starting index
	unsigned int lineAdjacencyA = 0;

	for (auto& linePos = indices.begin(); linePos != indices.end(); linePos++) {
		if (*linePos == primitiveRestart) {
			if (*(linePos - 1) == *lineStart) { //this line loops
				adjacencyIndices[lineAdjacencyA] = *(linePos - 2);
				adjacencyIndices.push_back(*(lineStart + 1));
				//vertices.erase(vertices.end() - 2, vertices.end());
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
		//vertices.erase(vertices.end() - 2, vertices.end());
	}
	else
		adjacencyIndices.push_back(makeAdjacencyVert(vertices[indices.back()], vertices[indices.end()[-2]]));


	//remove reduntant adjacency verts
	for (unsigned int v = 0; v < vertices.size(); v++) {
		if (std::find(adjacencyIndices.begin(), adjacencyIndices.end(), v) == adjacencyIndices.end()) {
			vertices.erase(vertices.begin() + v);
			for (auto& i : adjacencyIndices) {
				if (i != primitiveRestart && i > v)
					i--;
			}

		}
	}


	indices = adjacencyIndices;
}

unsigned int CMesh::makeAdjacencyVert(glm::vec3& A, glm::vec3& B) {
	glm::vec3 adjacencyVert = 2.0f * A - B;
	vertices.push_back(adjacencyVert);
	return vertices.size() - 1;
}





/** Append the given mesh, updating its index data in the process. */
TMeshRec CMesh::add(CMesh& mesh) {
	unsigned int primitiveRestart = 0xFFFF;
	if (vertices.size() > 0xFFFE)
		primitiveRestart = 0xFFFFFFFF;

	TMeshRec meshRec;
	meshRec.indexStart = indices.size();
	meshRec.indexSize = mesh.indices.size();
	meshRec.vertStart = vertices.size();

	vertices.insert(vertices.end(), mesh.vertices.begin(), mesh.vertices.end());
	for (auto& index : mesh.indices) {
		if (index == primitiveRestart)
			indices.push_back(primitiveRestart);
		else
			indices.push_back(index + meshRec.vertStart);
	}

	return meshRec;
}




/** Sort line-pair vertexes to form as long a sequence as possible. This is
	to prep them for converting into line strips. */
void CMesh::orderLines() {
	std::vector<unsigned int> orderedIndices; std::vector<unsigned int> rejects;
	for (auto& linePairA = indices.begin(); linePairA != indices.end(); linePairA += 2) {
		bool inserted = false;
		for (auto& orderedPairA = orderedIndices.begin(); orderedPairA != orderedIndices.end(); orderedPairA += 2) {
			if (*(linePairA + 1) == (*orderedPairA)) { //pair fits before this pair
				orderedIndices.insert(orderedPairA, linePairA, linePairA+ 2);
				inserted = true;
				break;
			}

			if (*(linePairA) == (*orderedPairA)) { //try to fit pair by reversing it
				auto& it = orderedIndices.insert(orderedPairA, *linePairA);
				orderedIndices.insert(it, *(linePairA + 1));
				inserted = true;
				break;
			}

			if (*(linePairA+ 1) == *(orderedPairA + 1)) { //try to fit pair by reversing and adding at end
				auto& it = orderedIndices.insert(orderedPairA + 2, *(linePairA + 1));
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

