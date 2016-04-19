#pragma once

#include <SDL2/SDL_image.h>
#include <vector>
#include "Window.h"

//this contains the textures of all the possible tiles
class Spritesheet
{
public:
	~Spritesheet();
	void makeSheet(const string &_file, int _tileRes, Window *window);	//load image and chop it into tiles of requested size
	SDL_Texture* nextFramePong();

	vector<SDL_Texture*> frames;	//individual tiles/frames
	int tileRes;
	int currentFrame;
	bool forward;
};

