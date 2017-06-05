#pragma once

#include <SDL2/SDL_image.h>
#pragma comment (lib, "SDL2_image.lib")
#include <vector>
#include "Window.h"

//this contains the textures of all the possible tiles
class Spritesheet
{
public:
	Spritesheet(const string &_file, int _tileRes, Window *window);
	~Spritesheet();
	void makeSheet(const string &_file, int _tileRes, Window *window);	//load image and chop it into tiles of requested size
	SDL_Texture* rotateFrameCW(unsigned index, Window *window);

	vector<SDL_Texture*> frames;	//individual tiles/frames
	int tileRes;
};

