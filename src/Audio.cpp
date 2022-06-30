#include "Audio.h"
#include <cassert>

Audio::Audio()
{
	this->isPlaying = false;
	//Inicializamos BASS al arrancar el juego (id_del_device, muestras por segundo, ...)
	if (BASS_Init(-1, 44100, 0, 0, NULL) == false) //-1 significa usar el por defecto del sistema operativo
	{
		std::cout << "Error inicializando audio" << std::endl;
	}
}

Audio::~Audio()
{
}

HCHANNEL Audio::play(float volume)
{
	return HCHANNEL();
}

void Audio::Stop(HCHANNEL channel)
{
}

Audio* Audio::Find(const char* filename)
{
	assert(filename);
	auto it = sLoadedAudios.find(filename);
	if (it != sLoadedAudios.end())
		return it->second;
	return NULL;
}

Audio* Audio::Get(const char* filename, bool isLooped)
{
	//load it
	Audio* audio = Find(filename);
	if (audio)
		return audio;

	audio = new Audio();
	if (!audio->load(filename, isLooped))
	{
		delete audio;
		return NULL;
	}

	return audio;
}

HCHANNEL* Audio::Play(const char* filename)
{
	return nullptr;
}

HSAMPLE Audio::LoadSample(const char* fileName, bool isLooped) {
	//use BASS_SAMPLE_LOOP in the last param to have a looped sound
	HSAMPLE hSample;
	if (isLooped) {
		hSample = BASS_SampleLoad(false, fileName, 0, 0, 3, BASS_SAMPLE_LOOP);
	}
	else {
		hSample = BASS_SampleLoad(false, fileName, 0, 0, 3, 0);
	}
	if (hSample == 0)
	{
		std::cout << "ERROR load" << fileName << std::endl;
	}
	std::cout << " + AUDIO load " << fileName << std::endl;
	return hSample;
}

bool Audio::load(const char* filename, bool isLooped) {
	std::string str = filename;
	std::string ext = str.substr(str.size() - 4, 4);
	HSAMPLE sample = NULL;
	double time = getTime();

	std::cout << " + Audio loading: " << filename << " ... ";
	sample = LoadSample(filename, isLooped);
	if (sample == 0) {
		return false;
	}
	this->sample = sample;

	std::cout << "[OK] Time: " << (getTime() - time) * 0.001 << "sec" << std::endl;
	return true;

}

void Audio::PlayGameSound(const char* fileName, bool isLooped) {
	
	Audio* audio = Audio::Get(fileName, isLooped);

	HCHANNEL hSampleChannel;
	hSampleChannel = BASS_SampleGetChannel(audio->sample, false);

	//Lanzamos un sample
	BASS_ChannelPlay(hSampleChannel, true);

}