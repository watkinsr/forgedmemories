#pragma once
#include "SDLInitManager.h"

class EventLoop
{
public:
    EventLoop(int screen_width, int screen_height);
	~EventLoop(void);
	void Start();
private:
    SDLInitManager* m_sdl_init_manager;
	int m_screen_width;
	int m_screen_height;

	float m_camera_x;
	float m_camera_y;
};

