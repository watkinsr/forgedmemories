#include "../include/EventLoop.h"

EventLoop::EventLoop(int screen_width, int screen_height) {
	m_screen_width = screen_width;
	m_screen_height = screen_height;
	
	m_sdl_init_manager = new SDLInitManager();
}

EventLoop::~EventLoop(void){}

void EventLoop::Start(){
	while (m_sdl_init_manager->GetMainSDLEvent()->type != SDL_QUIT){
		m_sdl_init_manager->Begin();
	}
}