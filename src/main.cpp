#include "../include/EventLoop.h"

int main()
{
	EventLoop* event_loop = new EventLoop(640, 480);
    event_loop->Start();
	return 0;
}