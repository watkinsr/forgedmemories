#include "stdafx.h"
#include "Text.h"


Text::Text(int x1, int y1, string msg)
{
	x = x1;
	y = y1;
	message = msg;
}

Text::Text(void)
{
}

Text::~Text(void)
{
}

void Text::editAllMessagesInVector(int amountOfSlotsToEdit, vector<string>StringMessages, vector<int>yValues){
	for (int i = 0; i < amountOfSlotsToEdit; i++){

	textVect[i].setMessage(StringMessages[i]);
	textVect[i].setX(263);
	textVect[i].setY(yValues[i]);
	}
}

void Text::eraseAllMessagesInVector(){
	for (int i = 0; i < textVect.size(); i++){
		textVect.erase (textVect.begin(),textVect.begin()+textVect.size());
	}
}

void Text::ResizeVector(int size){
	eraseAllMessagesInVector();
	Text text1;
	Text text2;
	Text text3;
	Text text4;
	Text text5;
	textVect.push_back(text1);
	textVect.push_back(text2);
	textVect.push_back(text3);
	textVect.push_back(text4);
	textVect.push_back(text5);
}