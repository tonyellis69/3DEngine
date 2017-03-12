#include "SoundLib.h"

///Implementation of the Soundlib class.


CSoundLib::CSoundLib(void) {
	SoundID = 0;
	MusicID = 0;
}

/** Load a soundfile, eg an Ogg file, and turn it into a sound effect. Store it on the soundlist.
    */
int CSoundLib::LoadSound(char* Filename) {

	CSound NewSound;
//	sf::SoundBuffer Buffer; //create a buffer for the actual sound data
	sf::SoundBuffer* Buffer = new sf::SoundBuffer();
	if (!Buffer->LoadFromFile(Filename))	{
		fprintf(stderr,"\nFailed to create a sound object from %s.",Filename);
		exit(EXIT_FAILURE );
	}
	SoundBufferList.push_back(Buffer); //which we store separately.

	NewSound.Sound.SetBuffer(*Buffer);
	NewSound.ID = SoundID++;

	SoundList.push_back(NewSound);

	return SoundID;
}

int CSoundLib::LoadSound(const std::string& Filename,float Volume) {
	int Snd = LoadSound((char*)Filename.c_str());
	SetSoundVolume(Snd,Volume);
	return Snd;
}



/** Finds the given sound on the sound list and plays it back.*/
void CSoundLib::playSound(int SoundNo) {
		SoundList[SoundNo-1].Sound.Play();

}

/** Sets the volume of the given sound on the sound list.*/
void CSoundLib::SetSoundVolume(int SoundNo, float Volume) {
	SoundList[SoundNo-1].Sound.SetVolume(Volume);
}

void CSoundLib::stopSound(int SoundNo) {
	SoundList[SoundNo-1].Sound.Stop();
}

void CSoundLib::LoopSound(int SoundNo, bool Loop) {
	SoundList[SoundNo-1].Sound.SetLoop(Loop);
}

/** Music file is not actually loaded, but attached to a CMusic object for later streaming. 
	The CMusic object goes on the music list. */
int CSoundLib::LoadMusic(char* Filename) {
	CMusic* Music = new CMusic;
	if (!Music->Music.OpenFromFile(Filename))	{
		fprintf(stderr,"\nFailed to create music object from %s.",Filename);
		exit(EXIT_FAILURE );
	}
	
	//TO DO - default tune should go in slot 0
	Music->Music.SetLoop(true);	
	Music->ID = MusicID++;
	MusicList.push_back(Music);
	return MusicID-1;
}

/** Load an audio file as a music stream and set its volume. */
int CSoundLib::LoadMusic(const std::string& filename, float volume) {
	int SoundNo = LoadMusic((char*)filename.c_str());
	SetMusicVolume(SoundNo,volume);
	return SoundNo;
}

/** Locates the given music object on the music list and plays it back.*/
void CSoundLib::PlayMusic(int MusicNo) {
	MusicList[MusicNo]->Music.Play();
}

/*Stops playing the given music object. */
void CSoundLib::StopMusic(int MusicNo) {
	MusicList[MusicNo]->Music.Stop();
}

/* Pauses the playing of the given music object.*/
void CSoundLib::PauseMusic(int MusicNo) {
	MusicList[MusicNo]->Music.Pause();
}

void CSoundLib::SetMusicVolume(int MusicNo, float Volume) {
	MusicList[MusicNo]->Music.SetVolume(Volume);
}


CSoundLib::~CSoundLib(void){
	int ListSize = MusicList.size();
	for (int i=0; i<ListSize; ++i)
		delete MusicList[i]; 
	ListSize =SoundBufferList.size();
	for (int i=0; i<ListSize; ++i) {
		sf::SoundBuffer* Buffer = SoundBufferList[i];
		delete Buffer;
	}
}
