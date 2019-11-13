#include "log.h"

CLog  sysLog;
CLog liveLog;


CLog::CLog() : callbackObj(NULL) {
	bootTime = system_clock::now();
	timeOn = false;
}

void CLog::setFile(std::string filename) {
	outFile.open(filename);
}

void CLog::setCallback(ILogCallback * callbackInstance) {
	callbackObj = callbackInstance;
}

void CLog::showTime(bool onOff) {
	timeOn = onOff;
}

/** Add the current elapsed time since boot-up to the stream. */
void CLog::insertTime() {
	duration<double> elapsed_seconds = system_clock::now() - bootTime;
	ss << elapsed_seconds.count() << "s ";
}

CLog::~CLog() {
	outFile.close();
}

CLog& operator<<(CLog& log, const TLogMsg& in) {
	if (in == timerOnMsg)
		log.timeOn = true;
	if (in == timerOffMsg)
		log.timeOn = false;
	if (in == alertMsg && log.callbackObj != NULL)
		log.callbackObj->logAlertCallback();
	if (in == startMsg) {
		time_t tt = system_clock::to_time_t(system_clock::now());
		struct std::tm * ptm = std::localtime(&tt);
		log.ss << std::put_time(ptm, "*** %A %B %d, %Y  time: %T ***") << "\n";

	}
	return log;
}

CLog& operator<<(CLog& log, const glm::vec3& in) {
	log.ss << "x " << in.x << " y " << in.y << " z " << in.z;
	return log;
}

CLog& operator<<(CLog& log, const glm::vec4& in) {
	log.ss << "x " << in.x << " y " << in.y << " z " << in.z << " w " << in.w;
	return log;
}

CLog& operator<<(CLog& log, const glm::i32vec3& in) {
	log.ss << "x " << in.x << " y " << in.y << " z " << in.z;
	return log;
}