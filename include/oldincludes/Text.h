#pragma once

#include <vector>

using namespace std;

class Text
{
public:
	Text(int x1, int y1, string msg);
	Text(void);
	~Text(void);

	void setX(int x1){x = x1;};
	void setY(int y1){y = y1;};

	void setMessage(string msg){message = msg;};

	int getX(){return x;};
	int getY(){return y;};

	void editAllMessagesInVector(int amountOfSlotsToEdit, vector<string>StringMessages, vector<int>yValues);

	string getMessage(){return message;};

	void addToTextVector(Text text){textVect.push_back(text);};
	void eraseAllMessagesInVector();
	void ResizeVector(int size);

	vector<Text>getTextVect(){return textVect;};


private:
	int x;
	int y;
	string message;
	vector<Text>textVect;
};

