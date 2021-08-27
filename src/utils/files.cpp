#include "files.h"


std::string fileDataPath;

void file::setDataPath(std::string& path) {
	fileDataPath = path;
}

std::string file::getDataPath() {
	return fileDataPath;
}
