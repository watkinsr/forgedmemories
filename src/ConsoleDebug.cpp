#include "stdafx.h"
#include "ConsoleDebug.h"


ConsoleDebug::ConsoleDebug(void)
{
	time_init = false;
}


ConsoleDebug::~ConsoleDebug(void)
{
}

void ConsoleDebug::PrintLoop(){
	for(auto i : items_to_print) {
    // process i
	cout << i << " "; 
	} 
}
//Flush the console's strings out.
void ConsoleDebug::ClearConsole(float time_delay){

	//if we haven't obtained the time, get the time.
	if (!time_init){
		orig_time = (float)SDL_GetTicks();
		time_init = true;
	}
	//is the current time more than the previous and the delay. If so, let's clear the console.
	float current_time = (float)SDL_GetTicks();
	if (current_time > orig_time + time_delay){
		for (int i = 0; i < 20; i++)
		{
			cout << "\n" << endl;
		}
		//get time again.
		time_init = false;
	}

}

void ConsoleDebug::AddStatementToPrint(string debug_string){
	items_to_print.push_back(debug_string);
}