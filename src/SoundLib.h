#pragma once

#include <SFML/Audio.hpp>
#include <string>

//NB - the SFML audio and system /static/ libs are by default compiled to link /dynamically/(MtD) to the various VC runtime DLLs.
//So it doesn't seem possible to use these in a statically linked app.
//I have compiled my own libs that are statically linked with the libs they need (/Mt) . They are slightly larger, but I can link
//to them statically and get an .exe that doesn't need the VC runtime DLLs - which were giving me compatibility problems.
//Best solution is not to depend on SFML at all.

//2016 25 June - compiled static versions of SFML  1.6 in MSVC 2010 and that worked

class CSound
{
public:
	sf::Sound Sound; ///<Device for playing sound.
	int ID;
};

class CMusic {
public:
	sf::Music Music; ///<Represents sound data streamed from a file.
	int ID;
};


typedef std::vector<CSound> CSoundList; ///<Defines a vector type for holding sounds.
typedef std::vector<sf::SoundBuffer* > CSoundBufferList; ///<Defines a vector type for holding pointers to sound buffers.
typedef std::vector<CMusic*> CMusicList; ///<Defines a vector type for holding music.


class CSoundLib
{
public:
	CSoundLib(void);
	~CSoundLib(void);

	CSoundList SoundList;
	CSoundBufferList SoundBufferList;
	CMusicList MusicList;

	int LoadSound(char* Filename);
	int LoadSound(const std::string& Filename,float Volume);
	void playSound(int SoundNo);
	void SetSoundVolume(int SoundNo, float Volume);
	void stopSound(int SoundNo);
	void LoopSound(int SoundNo, bool Loop);
	int LoadMusic(char* Filename);
	int LoadMusic(const std::string& filename, float volume);
	void PlayMusic(int MusicNo);
	void StopMusic(int MusicNo);
	void PauseMusic(int MusicNo);
	void SetMusicVolume(int MusicNo, float Volume);

private:
	int SoundID;
	int MusicID;
};
