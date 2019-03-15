#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include <chrono>
#include <ctime>
#include <iomanip> 

using namespace std::chrono;

class ILogCallback {
public:
	virtual void logCallback(std::stringstream& logStream) {};
	virtual void logAlertCallback() {};
};

enum TLogMsg { timerOnMsg, timerOffMsg, alertMsg, startMsg};


/** A class for logging to a text file or to a rich text window. */
//lass CLogManip;
class CLog {
public:
	CLog() ;
	~CLog();
	void setFile(std::string filename);
	void setCallback(ILogCallback* callbackInstance);
	void showTime(bool onOff);

	void insertTime();
	
template <typename T>
	friend CLog& operator<<(CLog& log, const T& in);
	friend CLog& operator<<(CLog& log, const TLogMsg& in);
 
	std::ofstream outFile;
	ILogCallback * callbackObj;
	std::stringstream ss;
	time_point<system_clock> bootTime; ///<When this logger was created.
	bool timeOn; ///<If true, prefix a timestamp.
};

template <typename T>
CLog& operator<<(CLog& log, const T& in) {
	if (log.timeOn)
		 log.insertTime();
	log.ss << in;

	if (log.outFile.is_open()) {
		log.outFile << log.ss.str();
	}
	
	if (log.callbackObj) {
		log.callbackObj->logCallback(log.ss);
	}
	log.ss.str("");
	return log;
}

extern CLog  sysLog;
extern CLog  liveLog;







