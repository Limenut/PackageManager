#include "Window.h"

Window::Window()
{
	SDL_Window* win = nullptr;
	SDL_Renderer* ren = nullptr;
	area.x = 0;
	area.y = 0;
	area.w = 0;
	area.h = 0;
	offsetX = 0;
	offsetY = 0;
	mouseFocus = false;
	keyFocus = false;
}
Window::~Window()
{
	if (ren)
	{
		SDL_DestroyRenderer(ren);
	}

	if (win)
	{
		SDL_DestroyWindow(win);
	}
}
bool Window::init(const string &_title, int _width, int _height)
{
	title = _title;
	area.w = _width;
	area.h = _height;

	//Create window
	win = SDL_CreateWindow(_title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, area.w, area.h, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if (win == NULL)
	{
		printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
		return false;
	}

	//Create renderer for window
	ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
	if (ren == NULL)
	{
		printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
		return false;
	}

	//store window in windowMap
	id = SDL_GetWindowID(win);

	//Initialize renderer color
	SDL_SetRenderDrawColor(ren, 0xFF, 0xFF, 0xFF, 0xFF);
	return true;
}

void Window::handleEvents(SDL_Event *e)
{
	switch (e->window.event)
	{
	case SDL_WINDOWEVENT_ENTER:
		SDL_RaiseWindow(win);
		mouseFocus = true;
		break;
	case SDL_WINDOWEVENT_LEAVE:
		mouseFocus = false;
		break;
	case SDL_WINDOWEVENT_FOCUS_GAINED:
		keyFocus = true;
		break;
	case SDL_WINDOWEVENT_FOCUS_LOST:
		keyFocus = false;
		break;
	case SDL_WINDOWEVENT_CLOSE: 
		//quit = true; 
		break;
	}
}