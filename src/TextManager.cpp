#include "TextManager.h"

TextManager::TextManager(CSDL_Setup* passed_SDL_Setup){
	csdl_setup = passed_SDL_Setup;
	font = new CSDLFont_Setup(csdl_setup);
}

TextManager::~TextManager(void){
	delete font;
}

//should print text
void TextManager::DrawDialog(vector<Text>textWithCoordAndStringMsg, string fontFile, CSDLFont_Setup* text, int fontSize){
	text->setColor(255,255,255);
	for (unsigned int i = 0; i < textWithCoordAndStringMsg.size(); i++){
		text->getText(textWithCoordAndStringMsg[i].getMessage(), fontFile, textWithCoordAndStringMsg[i].getX(), 
		textWithCoordAndStringMsg[i].getY(), text->getColor(), fontSize, true, false , csdl_setup->GetRenderer());
	}
}