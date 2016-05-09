#include "Spritesheet.h"

using namespace std;

Spritesheet::Spritesheet(const string &_file, int _tileRes, Window *window)
{
	makeSheet(_file, _tileRes, window);
}

Spritesheet::~Spritesheet()
{
	for (auto &i : frames)
	{
		SDL_DestroyTexture(i);
	}
	frames.clear();
}

void Spritesheet::makeSheet(const string &_file, int _tileRes, Window *window)
{
	tileRes = _tileRes;
	frames.clear();

	cout << "Loading " << _file.c_str() << "... ";

	SDL_Surface *fullSurf = IMG_Load(_file.c_str());
	if (!fullSurf)
	{
		cout << "Loading failed" << endl;
		return;
	}


	SDL_Rect fullRect;
	fullRect.x = fullRect.y = 0;
	fullRect.w = fullSurf->w;
	fullRect.h = fullSurf->h;

	SDL_Texture *tex;
	SDL_Surface *surf = SDL_CreateRGBSurface(0, tileRes, tileRes, 24, 0, 0, 0, 0);
	SDL_Rect recto;
	recto.w = recto.h = tileRes;
	recto.x = recto.y = 0;

	//chop into tiles
	for (recto.y = 0; recto.y < fullRect.h; recto.y += tileRes)
	{
		for (recto.x = 0; recto.x < fullRect.w; recto.x += tileRes)
		{
			SDL_BlitSurface(fullSurf, &recto, surf, NULL);
			tex = SDL_CreateTextureFromSurface(window->ren, surf);
			frames.push_back(tex);
		}
	}


	SDL_FreeSurface(surf);
	SDL_FreeSurface(fullSurf);

	cout << "Ok" << endl;
}

SDL_Texture* Spritesheet::rotateFrameCW(unsigned index, Window *window)
{
	SDL_Surface* target = SDL_CreateRGBSurface(0, tileRes, tileRes, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);

	Uint8 *srcPixels;


	SDL_Texture* source = frames[index];

	return nullptr;
}