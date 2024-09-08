#pragma once
#include <string>
#include <vector>

using namespace std;

class ConsoleDebug
{
public:
	ConsoleDebug(void);
	~ConsoleDebug(void);
	void PrintLoop();
	void ClearConsole(float time_delay);
	void AddStatementToPrint(string debug_string);
private:
	vector<string>items_to_print;
	float orig_time;
	bool time_init;
};

