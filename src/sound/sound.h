#pragma once

#include <string>


/** A hopefully universal header for a sound system.*/

namespace snd {
	bool onSpawn(int frequency);
	void quit();
	int loadSample(const std::string& filename);
	int loadSample(const std::string& name, const std::string& filename);
	void play(int id);
	void play(const std::string name);
	void setVolume(int volume);





}
