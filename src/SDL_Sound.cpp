#include "stdafx.h"
#include "SDL_Sound.h"


SDL_Sound::SDL_Sound(void)
{
	//The music that will be played
	gMusic = NULL;
	////Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO ) < 0 )
	{
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	//Initialize SDL_mixer
	if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 )
	{
		printf( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
		success = false;
	}
}

SDL_Sound::~SDL_Sound(void)
{
}

bool SDL_Sound::LoadMedia(const char *media_loc, bool isBGMusic)
{
	//Loading success flag
	bool success = true;

	//Load music, if it is a piece of background music, 
	//let's pass in that background music, otherwise, set a normal gameplay sound that doesn't last long or need to be looped.
	if (!isBGMusic) {
		gMusic = Mix_LoadWAV(media_loc);
		if( gMusic == NULL )
		{
			cout << "Failed to load " << media_loc << endl;
			success = false;
		}
	}
	else
		gBGMusic = Mix_LoadWAV(media_loc);

	return success;
}

void SDL_Sound::PlayMedia(bool loop){
	//Play the music
	if (loop)
		Mix_PlayChannel(1, gBGMusic, -1);
	else{
		Mix_PlayChannel(2, gMusic, 0);
		
	}
}

void SDL_Sound::Close()
{
    //Free the music
    Mix_FreeChunk( gMusic );
    gMusic = NULL;

    //Quit SDL subsystems
    Mix_Quit();
}
