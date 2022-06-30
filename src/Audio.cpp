#include "Audio.h"
#include "utils.h"

Audio::Audio()
{
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

Audio* Audio::Get(const char* filename)
{
	return nullptr;
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

void Audio::PlayGameSound(const char* fileName, bool isLooped) {

	HSAMPLE hSample = LoadSample(fileName, isLooped);

	HCHANNEL hSampleChannel;
	hSampleChannel = BASS_SampleGetChannel(hSample, false);

	//Lanzamos un sample
	BASS_ChannelPlay(hSampleChannel, true);

}