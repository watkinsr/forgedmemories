#pragma once
#include "stdafx.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdexcept>
#include <string>
#include <iostream>
#include "SDL_Setup.h"

using namespace std;

class CSDLFont_Setup
{
public:
	CSDLFont_Setup(CSDL_Setup* passed_renderer);
	~CSDLFont_Setup();

	//perhaps because it returns on a public function, it fucks up
	SDL_Texture* renderText(const std::string message, const std::string &fontFile,
		SDL_Color color, int fontSize, bool blend, bool shade, SDL_Renderer *renderer);

	void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y, int w, int h);
	
	void getText(string message, string fontFile, int passed_x, int passed_y, 
							 SDL_Color color, int fontSize, bool passed_blend, bool passed_shade, SDL_Renderer *renderer);

	bool quit;
	SDL_Color getColor();
	SDL_Color setColor(int r, int g, int b);


private:
	/*SDL_Texture *texture;*/
	bool blend;
	bool shade; 

	SDL_Color color;
	TTF_Font *font;
	SDL_Texture *image;
	CSDL_Setup* csdl_setup;
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Surface *surf;
};

