#pragma once
#ifndef DIALOG_H
#define DIALOG_H
#include "SDL_Setup.h"
#include "SDL_Font_Setup.h"
#include <vector>
#include "Text.h"

class TextManager
{
public:
	TextManager(CSDL_Setup* passed_SDL_Setup);
	~TextManager(void);
	void DrawDialog(vector<Text>textWithCoordAndStringMsg, string fontFile, CSDLFont_Setup* text, int fontSize);




private:
	CSDL_Setup* csdl_setup;
	CSDLFont_Setup* font;
};

#endif



