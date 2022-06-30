#ifndef AUDIO_H
#define AUDIO_H

#include "utils.h"
#include <bass.h>
#include <map>

class Audio
{
public:
	static std::map<std::string, Audio*> sLoadedAudios; //para nuestro manager
	HSAMPLE sample; //aqui guardamos el handler del sample que retorna BASS_SampleLoad

	bool isPlaying;

	Audio(); //importante poner sample a cero aqui
	~Audio(); //aqui deberiamos liberar el sample con BASS_SampleFree

	HCHANNEL play(float volume); //lanza el audio y retorna el channel donde suena

	static void Stop(HCHANNEL channel); //para parar un audio necesitamos su channel
	static Audio* Find(const char* filename);
	static Audio* Get(const char* filename, bool isLooped); //manager de audios 
	static HCHANNEL* Play(const char* filename); //version estática para ir mas rapido

	static HSAMPLE LoadSample(const char* fileName, bool isLooped);
	bool load(const char* filename, bool isLooped);
	void PlayGameSound(const char* fileName, bool isLooped);
};

#endif