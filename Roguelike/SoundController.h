#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <stdio.h>
#include <string>
#include <vector>

using namespace std;

class SoundController final
{
private:
	SoundController() {}
	~SoundController() {}
	SoundController(SoundController& gc) {}
	SoundController& operator = (SoundController& gc) {}
	vector<Mix_Chunk*> playingSounds;
public:
	int soundsVolume = 64;

	static SoundController& Instance()
	{
		static SoundController instance;
		return instance;
	}

	void RecalculateSounds()
	{
		for (int i = 0; i < playingSounds.size();)
			if (!Mix_Playing(i))
			{
				Mix_FreeChunk(playingSounds[i]);
				playingSounds.erase(playingSounds.begin() + i);
			}
			else i++;
	}

	void PlaySoundOnce(string soundPath)
	{
		Mix_Chunk* sound = Mix_LoadWAV(soundPath.c_str());
		if (sound != nullptr)
		{
			sound->volume = soundsVolume;
			Mix_PlayChannel(playingSounds.size(), sound, 0);
			playingSounds.push_back(sound);
		}
	}
};