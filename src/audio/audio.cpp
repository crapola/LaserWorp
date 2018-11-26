// header
#include "audio.h"
// std
#include <cmath>
// libs
#include <SDL.h>
// local

static void MyAudioCallback(void* userdata,Uint8* stream,int len)
{
	for (ptrdiff_t i=0; i<len; ++i)
	{
		stream[i]=(i/32)%8;
	}
}
static SDL_AudioDeviceID device; // Uint32.

Sint8 Square(const int t,const int wavelen,const int amp)
{
	Sint8 s;
	s=t%wavelen-wavelen/2; // Upwards slope of size wavelen.
	s=(s>0)-(s<0); // Take the sign.
	return s*amp; // Apply amplitude.
}
Sint8 Sinusoidal(const int t,const int wavelen,const int amp)
{
	return cos((t%wavelen)*(2.0f*M_PI/wavelen))*amp;
}

namespace audio
{
void Beep(int frequency)
{
	//return;// disable for now
	int duration=125;//ms
	int sample_count=44100*duration/1000;
	int wave_length=44100/frequency;
	int volume=4;
	//SDL_Log("beep %d %d",sample_count,wave_length);
	Sint8* data=static_cast<Sint8*>(malloc(sample_count));
	for (ptrdiff_t i=0; i<sample_count; ++i)
	{
		data[i]=Square(i,wave_length,volume);
	}
	SDL_QueueAudio(device,data,sample_count);
	free(data);
}
void DeviceClose()
{
	SDL_CloseAudioDevice(device);
}
bool DeviceOpen()
{
	// Lazy audio init.
	if (!(SDL_WasInit(0)&SDL_INIT_AUDIO))
		SDL_InitSubSystem(SDL_INIT_AUDIO);
	// Code adapted from https://wiki.libsdl.org/SDL_OpenAudioDevice
	SDL_AudioSpec want,have;
	SDL_zero(want);
	want.freq=44100;
	want.format=AUDIO_S8;//AUDIO_F32;
	want.channels=1;
	want.samples=4096;
	want.callback=NULL;//MyAudioCallback;
	int count = SDL_GetNumAudioDevices(0);
	for (int i = 0; i < count; ++i)
	{
		SDL_Log("Audio device %d: %s", i, SDL_GetAudioDeviceName(i, 0));
	}
	device=SDL_OpenAudioDevice(NULL,0,&want,&have,0);//SDL_AUDIO_ALLOW_FORMAT_CHANGE);
	if (device==0)
	{
		SDL_Log("Failed to open audio: %s", SDL_GetError());
		return false;
	}
	else
	{
		if (have.format!=want.format)
		{
			SDL_Log("We didn't get wanted audio format. We got %i instead.",have.format);
		}
		SDL_PauseAudioDevice(device,0); /* start audio playing. */
		//SDL_Delay(1000); /* let the audio callback play some sound for 5 seconds. */
		//SDL_CloseAudioDevice(device);
		SDL_Log("Using audio device %d.",device);
		return true;
	}
}
}
