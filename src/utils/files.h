#pragma once

#include <string>
#include <fstream>
#include <iostream>

/** Simple file handling in one place. */

namespace file {

	void setDataPath(std::string& path);
	std::string getDataPath();

	// Helper function to read a piece of data from a stream.
	template<class T, class S>
	void readObject(T& to_read, S& in) {
		in.read(reinterpret_cast<char*>(&to_read), sizeof(T));
	}

	// Helper function to write a piece of data to a stream.
	template<class T, class S>
	void writeObject(T& to_write, S& out) {
		out.write(reinterpret_cast<char*>(&to_write), sizeof(T));
	}


}