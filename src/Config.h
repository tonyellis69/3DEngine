#pragma once
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
using namespace std;

/** Used by the config class to store the values of variables. */
class CVar {
public:
	string Identifier;
	unsigned long Value;
};

#define CONFIG_MAX_OPERANDS 6

/** A simple class for parsing simple text-based config files. Simple. */

class CConfig {
public:
	CConfig();
	CConfig(char* Filename);
	~CConfig(void);
	void OpenFile(char* Filename);
	bool GetNextLine();
	void GetInt(string key,unsigned long& val);
	bool GetFloat(const string& key, float& val); 
	bool IsKey(string key);
	//template<class TYPE>
//	void GetCSV(TYPE* Data);
	template<class TYPE> void GetCSV(TYPE *Data);
	bool IsOp(string Operator);
	void SetVar(string& Ident, unsigned long value);
	bool IsVar(string& Ident);
	unsigned long GetVar(string& Ident);
	bool IsSection(const string& Section);
	void UpdateSection();
	bool CheckSection(const string& Section);
	void Exec( unsigned long Handle);

	std::ifstream infile; ///<Stream we read the config file into.
	bool open; ///<True as long as the file is open.
	char buff[256];  ///<Temporary storage for each read line.
	string currentLine; ///<Holds the latest line read from the config file.
	string OperandStr; ///<Holds the right-hand, operand part of a line successfuly searched for an operator.
	string Arg[CONFIG_MAX_OPERANDS];
	int NumArg[CONFIG_MAX_OPERANDS];
	vector<CVar> Variables;
	std::vector<CVar>::iterator itVar; ///<Handy iterator for variables.
	string CurrentSection;
	bool NewSection; ///<True if we've just entered a new [section].
};


string RemoveSpace(string str); /** Helper function to remove whitespace. */
string RemoveLeadingSpace(string str); /** Helper function to remove whitespace from start of line. */
bool IsInt(string str); /** Helper function that returns true if str is an integer. */

/** Read a list of comma separated values into the given array pointer. */
template<class TYPE>
void CConfig::GetCSV(TYPE *Data) {
	int n=0;
	char buf[256];
	stringstream ss;
	while (GetNextLine()) {
		if ( currentLine.find(",",0) == string::npos ) //no commas? 
			return;
		ss << currentLine;	

		while (ss.getline(buf,256,',')) {
			Data[n++] = atoi(buf);
		}
		ss.clear(); 
	}
}

