#include "sound.h"

#include <stdlib.h> 
#include <unordered_map>

#include "SDL.h"
#include "SDL_mixer.h"

std::unordered_map<int, Mix_Chunk*> samples;
int sampleId = 1;
std::unordered_map<std::string, Mix_Chunk*> samplesStr;

namespace snd {

	bool onSpawn(int frequency) {
		if (SDL_Init(SDL_INIT_AUDIO) < 0)
			return false;
		atexit(quit);
		return Mix_OpenAudio(frequency, MIX_DEFAULT_FORMAT, 2, 512) == 0;
	}

	void quit() {
#ifndef _DEBUG //purely to avoid annoying debug message on exit
		Mix_CloseAudio();
		SDL_Quit();
#endif
	}

	int loadSample(const std::string& filename) {
		Mix_Chunk* sample = Mix_LoadWAV(filename.c_str());

		if (sample != NULL) {
			samples[sampleId] = sample;
			return sampleId++;
		}
		return 0;
	}

	int loadSample(const std::string& name, const std::string& filename) {
		Mix_Chunk* sample = Mix_LoadWAV(filename.c_str());

		if (sample != NULL) {
			samplesStr[name] = sample;
			return 1;
		}
		return 0;
	}




	void play(int id) {
		Mix_PlayChannel(-1, samples[id], 0);
	}

	void play(const std::string name) {
		Mix_PlayChannel(-1, samplesStr[name], 0);
	}

	void setVolume(int volume) {
		Mix_Volume(-1, volume);
	}


}