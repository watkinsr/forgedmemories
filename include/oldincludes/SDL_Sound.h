#pragma once
#include "stdafx.h"
#include <stdio.h>
#include <string>
#include <vector>

using namespace std;

class SDL_Sound
{
public:
	SDL_Sound(void);
	~SDL_Sound(void);
	void Close();
	bool LoadMedia(const char *media_loc, bool isBGMusic);
	void PlayMedia(bool loop);
private:
	bool success;
	Mix_Chunk *gBGMusic;
	Mix_Chunk *gMusic;
	//Mix_Music *gBGMusic;
};

