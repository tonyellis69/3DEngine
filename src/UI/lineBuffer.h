#pragma once

#include <vector>



enum TNewLine { no, wordwrap, newline };
/** A partial or complete line of text, with its rendered (x) dimensions. */
struct TLineFragment {
	int textObj;
	int textPos;
	int textLength;
	int renderStartX;
	int renderStartY;
	int height;
	int renderEndX;
	TNewLine causesNewLine;
	bool finalFrag;
};

struct TLine {
	int height;
	std::vector<int> fragments;
};

struct TFragPos {
	int fragId;
	int lineNo;
};

/** A data structure for maintaining line fragments, referenced by index. */
class CLineBuffer {
public:
	CLineBuffer();
	void clear();
	void addLine();
	void appendFragment(int  fragId);
	bool isEmpty();
	const TLineFragment& finalFrag();
	void updateFragmentPositions();
	void setYoffset(int value);
	void adjustYoffset(int adjust);
	int getFinalRow() {
		return lines.size() - 1;
	}
	int getLineBase(int lineNo);
	int getLineTop(int lineNo);
	void removeLine(int lineNo);
	TLine& getLine(int lineNo);
	void insert(int slot, TLine& line);
	void initFrags();
	int getFreeFragment();
	int copyFragment(TLineFragment& fragment);
	TLineFragment& getFragment(int id) {
		return frags[id];
	}
	int getYextent();
	TLineFragment& trimTop(int dist);
	const std::vector<TLineFragment>& getFrags() {
		return frags;
	}
	TFragPos& getFirstFrag(int objNo);
	TFragPos& getLastFrag(int objNo);

	std::vector<TLine> lines;

	int yOffset; ///<Vertical adjustment for fragments.

	std::vector<TLineFragment> frags;
	std::vector<int> freeFrags;
};