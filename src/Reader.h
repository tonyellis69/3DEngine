#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

class CVariable {
public:
	string name; ///<Name of this variable.
	int intVal; ///<Integer value storage.
	//TO DO: can make this tailor-made with virtual functions to set and return values of various types.
	//Base class doesn't need to know the type.
};
/*
class CToken {
public:
	char* asStr();
	int asInt();
	float asFloat();

		string text; ///<The token as a string.

};
*/


/** A base class for reading text-based instruction files. Parses them into separate tokens
	and keeps track of any variables declared. */
class CReader
{
public:
	CReader(void);
	~CReader(void);
	bool openFile(string filename);
	void readFile();
	bool readLine() ;
	bool parseNextLine();
	void tokenise(string& line);
	CVariable* createVariable(string& varName);
	CVariable* findVariable(string& name);
	virtual void processInstruction() {};
	string strArg(int token);
	int intArg(int token);
	float floatArg(int token);
	void returnVar(int value); 
	unsigned char* readBytes(int noBytes);
	bool instructionIs(string text);
	bool optionIs(string text);

	ifstream file; ///<The text file we're reading.
	string currentLine; ///A copy of the line we're currently processing.

	int lineNo; ///<Keeps track of what line of the file we're on.
	vector<string> token; ///<Holds the separate tokens of the last tokenises line.
	vector<CVariable> variables; ///<List of variables encountered in processing a file.
	bool assignment; ///<True if the current tokenised line assigned a value to a variable.
	CVariable* assigneeVariable; ///<Pointer to the variable in the current assignment operation.
	unsigned char* bytes; ///<Points to the most recently read block of raw bytes, if any.

	
};
