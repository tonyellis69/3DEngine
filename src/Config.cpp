#include "Config.h"
#include <fstream>
#include <iostream>
#include <algorithm> 
#include <sstream>


CConfig::CConfig() {}

/** Create a config file reader for the given file. */
CConfig::CConfig(char* Filename){
	OpenFile(Filename);
}

/** Open the given config file for reading. */
void CConfig::OpenFile(char* Filename) {
	infile.open (Filename, ifstream::in);
	if (infile.is_open())
		open = true;
	else
	{ cerr << "\nUnable to open file: " << Filename;	exit(0); }
	GetNextLine();
}

/** Read the next line of the file. Returns false if end of file. */
bool CConfig::GetNextLine() {
	//infile.getline(buff, 256);
	bool result = getline(infile,currentLine)!=0;
	//RemoveSpace(str);
	//if this line is a section header, change the current section
	NewSection = false;
	UpdateSection();
	 
	return result;
}

/*
bool CCongfig::IfKey(string& key) {

} */

/** Return the next value associated with the given key in the file, as an integer. */
void CConfig::GetInt(string key, unsigned long& val) {
	string srch = RemoveSpace(key) + "=";
	string str = RemoveSpace(currentLine);
	if ( str.compare(0,srch.length(),srch) == 0 ) {
		string valStr = str.substr(srch.length(),256);
		//is the value actually a variable?
		if (IsVar(valStr)) {
			val = GetVar(valStr);
		}
		else {
		stringstream ss(str.substr(srch.length(),256));
		ss >> val;
		}
	}
}

bool CConfig::GetFloat(const string& key, float& val) {
	string srch = RemoveSpace(key) + "=";
	string str = RemoveSpace(currentLine);
	if ( str.compare(0,srch.length(),srch) == 0 ) {
		string valStr = str.substr(srch.length(),256);
		//is the value actually a variable?
		if (IsVar(valStr)) {
			val = (float)GetVar(valStr);
		}
		else {
			stringstream ss(str.substr(srch.length(),256));
			ss >> val;
		}
		return true;
	}
	return false;

}

/** Checks for the given key, but does not eat the line. */
bool CConfig::IsKey(string key) {
	string srch = RemoveSpace(key) + "=";
	string str = RemoveSpace(currentLine);
	return  !str.compare(0,srch.length(),srch);
}

/** 
bool CConfig::IsSection(string sectionName) {

}
*/

/** Returns true if the current line uses the given operator. The operands are parsed into Arg[n] or NumArg[n].*/
bool CConfig::IsOp(string Operator) {
	string srch = RemoveSpace(Operator); 
	string str = RemoveLeadingSpace(currentLine);
	OperandStr.clear();
	bool result = !str.compare(0,srch.length(),srch);
	if (result) {//detected an operator? Great, let's get its operands. 
		OperandStr = RemoveLeadingSpace(str.substr(srch.length(),string::npos )); //get the rest of the string
		stringstream ss; string item;
		ss << OperandStr;	
		int n =0;
		while (getline(ss,item,',')) {
			item = RemoveLeadingSpace(item);
			if (IsInt(item))
				NumArg[n] = atoi(item.c_str());
			else
				Arg[n] = item;
			n++;
		}
	}
	return  result;
}

/** Set a config file variable to the given value. If the variable doesn't exist it will be created. */
void CConfig::SetVar(string& Ident, unsigned long value) {
	//is this variable in our list of variables?
	for (itVar = Variables.begin();itVar != Variables.end();itVar++) {
		if (itVar->Identifier.compare(Ident) ==0) {
			itVar->Value = value; //set it to the given value
			return;
		}
	}
	//not found? then create it
	CVar New; 
	New.Identifier = Ident;
	New.Value = value;
	Variables.push_back(New);
}

/** Return the integer value of the given variable. */
unsigned long CConfig::GetVar(string& Ident) {
	
	//itVar = find( Variables.begin(),Variables.end(),Ident);
	for (itVar = Variables.begin();itVar != Variables.end();itVar++) {
		if (itVar->Identifier.compare(Ident ) ==0)
			return itVar->Value;
	}
		cerr << "variable not found in config file: " << Ident << "\n";
		return -1;
}

bool CConfig::IsVar(string& Ident) {
	for (itVar = Variables.begin();itVar != Variables.end();itVar++) {
		if (itVar->Identifier.compare(Ident ) == 0)
			return true;
	}
}

/** Returns true if the current line is the given section header. Eg: '[section]'. */

bool CConfig::IsSection(const string& Section) {
	string srch;
	srch = "[" + Section + "]";
	if (currentLine.find(srch,0) == string.npos)
		return false;
	else
		return true;
}

/** Return true if the current section is the given one.*/
bool CConfig::CheckSection(const string& Section) {
	string srch;
	srch = "[" + Section + "]";
	return !CurrentSection.compare(0,srch.length(),srch);
}


void CConfig::UpdateSection(){
	string str = RemoveLeadingSpace(currentLine);
	if (str[0] == '[') { //found a section header
		CurrentSection = str;
		NewSection = true;
	}	
}
	

void CConfig::Exec(unsigned long Handle) {
	SetVar(Arg[0],Handle);
}



CConfig::~CConfig(void){
}

string RemoveSpace(string str) {
	str.erase (std::remove (str.begin(), str.end(), ' '), str.end());
	return str;
}

string RemoveLeadingSpace(string str) {
	str.erase (0, str.find_first_not_of(" "));
	return str;
}

bool IsInt(string str) {
	char* p;
	strtol(str.c_str(), &p, 10);
	return *p == 0;
}