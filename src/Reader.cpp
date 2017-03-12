#include "Reader.h"
#include <sstream>

CReader::CReader(void) {
}

/** Attempt to open the given file for reading. */
bool CReader::openFile(string filename) {
	bytes = NULL;
	file.open (filename.c_str(), ifstream::in);
	if (!file.is_open()) {
		return false;
	}
	lineNo = 1;
	return true;
}

/** Read the file line by line, parsing and processing each token. */
void CReader::readFile() {
	while(readLine()) {};
}

/** Read, parse and process the next line of the file. */
bool CReader::readLine() {
	if (parseNextLine()) {
		if ((assignment) && (token[0][0] != '.')) {
			assigneeVariable = createVariable(token[0]);
			token.erase(token.begin(),token.begin()+2);//remove assignee tokens.
		}

		//pass the instruction on to the user for processing
		if (token.size() > 0)
			processInstruction();
		return true;
	}
	return false;
}

/** Process the next line of the file. */
bool CReader::parseNextLine() {
	if (!getline(file,currentLine))
		return false;
	tokenise(currentLine);
	return true;
}

/** Break the line into tokens, deliminating by space, comma, =, @ */
void CReader::tokenise(string& line) {
	token.clear();
	assignment = false;
	const char* lineCopy = _strdup(line.c_str());
	const char* mem = lineCopy;

	do {
		const char *begin = lineCopy;

		while(((*lineCopy != ' ')&&(*lineCopy != ',')&&(*lineCopy != '=')) && *lineCopy) 
			lineCopy++;
		if (begin != lineCopy)
			token.push_back(string(begin, lineCopy));
		if (*lineCopy == '=') {
			token.push_back("=");
			assignment = true;
		}
	} while (0 != *lineCopy++);
	free((void*)mem);
}

CVariable* CReader::createVariable(string& varName) {
	//does variable already exist?
	for (size_t v=0; v < variables.size();v++) {
		if (variables[v].name.compare(varName) == 0) 
			return &variables[v];
	}
	CVariable newVar;
	newVar.name = varName;
	variables.push_back(newVar);
	return &variables.back();
}

//make the arg funcs all methods of token which can be further subclassed
//and that has state so that it can check if it's a variable when
//they'r called.
//consider using same class for variables and tokens.
void replaceVariables() {
	
}

/** Return the named variable, or NULL if not found.*/
CVariable* CReader::findVariable(string& name) {
	for (size_t v=0; v < variables.size();v++) {
		if (variables[v].name.compare(name) == 0) 
			return &variables[v];
	}
	return NULL;
}

/** Returns the indicated token as a string. */
string CReader::strArg(int tokenNo) {
	return token[tokenNo];
}

/** Returns the indicated token as an int. */
int CReader::intArg(int tokenNo) {
	//is this token a variable?
	//if so, return its contents.
	CVariable* var = findVariable(token[tokenNo]);
	if (var != NULL)
		return var->intVal;

	istringstream convert(token[tokenNo].c_str());
	int x;
	return convert >> x ? x : 0;
}

/** Returns indicated token as a float. */
float CReader::floatArg(int tokenNo) {
	istringstream convert(token[tokenNo].c_str());
	float x;
	return convert >> x ? x : 0;
}

/** Set the assignee variable of the current instruction to this returned value.*/
void CReader::returnVar(int value){
	assigneeVariable->intVal = value;
}

/** Reads a stream of comma-separated values into a single block of memory.*/
unsigned char*  CReader::readBytes(int noBytes) {
	if (bytes != NULL)
		delete bytes;
	bytes = new unsigned char[noBytes];
	int x;
	string nextByte;
	for (int b=0;b<noBytes;b++) {
		getline(file,nextByte,',');
		istringstream convert(nextByte.c_str());
		convert >> x;
		bytes[b] = (unsigned char) x;
	}
	return bytes;
}

/** Returns true if the first token of the currently parsed line is the given string.*/
bool CReader::instructionIs(string text) {
	if (token.size())
		return token[0].compare(text) == 0;
	return false;
}

/** Returns true if the first token of the currently parsed line is of the form .text = value. */
bool CReader::optionIs(string text) {
	if (token.size()) {
		string cat = "." + text;
		return token[0].compare(cat) == 0;
	}
	return false;
}



CReader::~CReader(void) {
	if (bytes != NULL)
		delete bytes;
}

/*
char *CToken::asStr() {


}


int CToken::asInt() {

}

float asFloat() {

} */