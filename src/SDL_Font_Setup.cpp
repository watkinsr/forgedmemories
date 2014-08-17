#include "SDL_Font_Setup.h"


CSDLFont_Setup::CSDLFont_Setup(CSDL_Setup* passed_csdlsetup)
{
	csdl_setup = passed_csdlsetup;
	//inits TTF
	if (TTF_Init() != 0){
		std::cout << TTF_GetError() << std::endl;
		cout << "TTF ERROR" << endl;
	}

	SDL_Color color = { 255,255,255 };

	blend = false;
	shade = false;

	SDL_Surface *surf = NULL;
}

CSDLFont_Setup::~CSDLFont_Setup()
{

}

void CSDLFont_Setup::getText(string message, string fontFile, int passed_x, int passed_y, 
							 SDL_Color color, int fontSize, bool passed_blend, bool passed_shade, SDL_Renderer *renderer){
	SDL_Texture *image = renderText(message, fontFile,
		color, fontSize, passed_blend, passed_shade, csdl_setup->GetRenderer());
	if (image == nullptr){
		cout << "image is null" << endl;
	}
	//Get the texture w/h so we can center it in the screen
	int iW, iH;
	iW = 0;
	iH = 0;
	SDL_QueryTexture(image, NULL, NULL, &iW, &iH);
	int SCREEN_WIDTH = 640;
	int SCREEN_HEIGHT = 480;
	//centers text
	int x = passed_x;
	int y = passed_y;

	renderTexture(image, csdl_setup->GetRenderer(), x, y,message.size() * 8, fontSize * 2);
	SDL_DestroyTexture(image);
	/*SDL_RenderPresent(csdl_setup->GetRenderer());*/
	//delete image;
}

SDL_Texture* CSDLFont_Setup::renderText(const std::string message, const std::string &fontFile,
										SDL_Color color, int fontSize, bool blend, bool shade, SDL_Renderer *renderer)
{
	//Open the font
	TTF_Font *font = TTF_OpenFont(fontFile.c_str(), fontSize);
	if (font == nullptr){
		cout << "Could not find " << fontFile.c_str() << endl;
		return nullptr;
	}	
	//We need to first render to a surface as that's what TTF_RenderText
	//returns, then load that surface into a texture
	SDL_Surface *surf = TTF_RenderText_Blended(font, message.c_str(), color);
	 if (blend == true)
        {
                surf = TTF_RenderText_Blended(font, message.c_str(), color);
        }

	if (shade == true)
	{
		SDL_Color bg = {0,0,0};
		surf = TTF_RenderText_Shaded(font, message.c_str(), color, bg);
	}

	if (surf == nullptr){
		TTF_CloseFont(font);
		return nullptr;
	}
	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surf);

	//Clean up the surface and font
	SDL_FreeSurface(surf);
	TTF_CloseFont(font);
	return texture;
}



void CSDLFont_Setup::renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y, int w, int h){
	//Setup the destination rectangle to be at the position we want
	SDL_Rect dst;
	dst.x = x;
	dst.y = y;
	//lol wrong func
	dst.w = w;
	dst.h = h;
	SDL_RenderCopy(ren, tex, NULL, &dst);
}

SDL_Color CSDLFont_Setup::getColor()
{
	return color;
}

SDL_Color CSDLFont_Setup::setColor(int r, int g, int b)
{
	SDL_Color color = { r,g,b };
	return color;
}

