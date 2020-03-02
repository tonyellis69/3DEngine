#include <algorithm>
#include <numeric>


#include "..\utils\log.h"

#include "lineBuffer.h"

CLineBuffer::CLineBuffer() {
	clear();
	yOffset = 0;
	lineFadeIn = 1.0;
}

void CLineBuffer::clear() {
	lines.clear();
	initFrags();
	//addLine(); //starting line
}

/** Append a new, empty line. */
void CLineBuffer::addLine() {
	lines.push_back(TLine{ 0,0,{},lineFadeIn });
}

void CLineBuffer::appendLine(TLine& newline) {
	lines.push_back(newline);
	lines.back().fadeInX = lineFadeIn;
}

/** Add a fragment to the bottom line. */
void CLineBuffer::appendFragment(int fragId) {
	if (lines.empty())
		addLine();
	lines.back().fragments.push_back(fragId);
	lines.back().height = std::max(lines.back().height, frags[fragId].height);
	lines.back().width = std::max(lines.back().width, frags[fragId].renderEndX);
}

bool CLineBuffer::isEmpty() {
	return lines.empty();

	//TO DO is this all I need?


	if (lines.empty())
		return true;
	return lines.back().fragments.empty();
}

TLineFragment CLineBuffer::finalFrag() {
	
	if (lines.empty()) {
		return TLineFragment{ 0,0,0,0,0,0,no,0 };
	}
	
	return frags[lines.back().fragments.back()];
}


/** Update the positions of all fragments based on scroll offset etc */
void CLineBuffer::updateFragmentPositions() {
/*	int lineY = 0;
	for (auto &line : lines) {
		for (auto fragId : line.fragments) {
			frags[fragId].renderStartY = lineY + yOffset;
		}
		lineY += line.height;
	}
	*/
}

void CLineBuffer::setYoffset(int value) {
	yOffset = value;
}

void CLineBuffer::adjustYoffset(int adjust) {
	yOffset += adjust;
}

/** Return the lowest pixel extent of this line. */
int CLineBuffer::getLineBase(int lineNo) {
	if (lines.empty())
		return 0;
	int yPos = 0;
	for (int line = 0; line <= lineNo; line++)
		yPos += lines[line].height;
	return yPos + yOffset;
}

/** Return the upper pixel extent of this line. */
int CLineBuffer::getLineTop(int lineNo) {
	int yPos = 0;
	for (int line = 0; line < lineNo; line++)
		yPos += lines[line].height;
	return yPos + yOffset;
}

void CLineBuffer::removeLine(int lineNo) {
	if (lines.empty())
		return;
	lines.erase(lines.begin() + lineNo);
}

TLine& CLineBuffer::getLine(int lineNo) {
	return lines[lineNo];
}

void CLineBuffer::insert(int slot, TLine& line) {
	lines.insert(lines.begin() + slot, line);
}

/** Free and initialise our list of line fragments for use. */
void CLineBuffer::initFrags() {
	std::iota(begin(freeFrags), end(freeFrags), 0);
	frags.resize(freeFrags.size());
}

/** Return a free line fragment for use. */
int CLineBuffer::getFreeFragment() {
	if (freeFrags.empty()) {
		int nextIdx = frags.size();
		frags.resize(frags.size() + 5);
		freeFrags.resize(5);
		std::iota(begin(freeFrags), end(freeFrags), nextIdx);
	}

	int index = freeFrags.back();
	if (index > 49)
		int b = 0;
	freeFrags.pop_back();
	return index;
}

/** Store a copy of the given fragment and return its id. */
int CLineBuffer::copyFragment(TLineFragment& fragment) {
	int id = getFreeFragment();
	frags[id] = fragment;
	return id;
}

/** Return the total height in pixels our lines extend over.*/
int CLineBuffer::getYextent() {
	int height = 0;
	for (auto line : lines) {
		height += line.height;
	}
	return height + yOffset;
}

/** Add to the yOffset by dist, removing any lines that fall outside view. 
	Return the new first fragment. */
TLineFragment& CLineBuffer::trimTop(int dist) {
	yOffset += -dist;
	if (abs(yOffset) >= lines[0].height) {
		yOffset += lines[0].height;
		lines.erase(lines.begin());
	}
	return frags[lines[0].fragments[0]];
}

/** Return the frag id and line number of the first fragment of this text obj. */
TFragPos CLineBuffer::getFirstFrag(int objNo) {
	int lineNo = -1;
	for (auto line : lines) {
		lineNo++;
		for (auto frag : line.fragments) {
			if (frags[frag].textObj == objNo)
				return TFragPos{ frag,lineNo };
		}
	}
	return TFragPos{ -1,-1 };
}

/** Return the frag id and line number of the last fragment of this text obj. */
TFragPos CLineBuffer::getLastFrag(int objNo) {
	TFragPos fragPos = { -1,-1 };
	int lineNo = -1;
	for (auto line : lines) {
		lineNo++;
		for (auto frag : line.fragments) {
			if (frags[frag].textObj == objNo)
				fragPos = { frag,lineNo };
		}
	}
	return fragPos;
}

/** Remove any line entirely composed of fragments from the given text obj. */
void CLineBuffer::removeObjLine(int objNo) {
	for (auto line = lines.begin(); line != lines.end();) {
		int firstFrag = line->fragments.front();
		int lastFrag = line->fragments.back();
		if (frags[firstFrag].textObj == objNo && frags[lastFrag].textObj == objNo)
			line = lines.erase(line);
		else
			line++;
	}
}
