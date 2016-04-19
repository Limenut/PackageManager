#pragma once

#include <SDL2/SDL.h>
#include <iostream>
#pragma comment (lib, "SDL2.lib")

using namespace std;

//each window has its own renderer and event handler for window-related events
class Window
{
public:
	Window();
	~Window();
	bool init(const string &_title, int _width, int _height);

	void handleEvents(SDL_Event *e);

	string title;
	Uint32 id;

	SDL_Window* win;
	SDL_Renderer* ren;
	SDL_Rect area;

	int offsetX;
	int offsetY;

	bool mouseFocus;
	bool keyFocus;
};

