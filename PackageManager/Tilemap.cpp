#include "Tilemap.h"
#include <fstream>
#include <string>


using namespace std;

Tilemap::Tilemap()
{

}
Tilemap::Tilemap(Spritesheet *_sprites)
{
	sprites = _sprites;
	tileRes = 0;
	vertiTiles = 0;
	horiTiles = 0;

	fullTex = nullptr;
}

Tilemap::~Tilemap()
{
	SDL_DestroyTexture(fullTex);
}

void Tilemap::loadFile(const string &_file)
{
	fstream file;
	file.open(_file.c_str(), ios::in | ios::binary);

	file.read((char *)&tileRes, sizeof(tileRes));
	file.read((char *)&vertiTiles, sizeof(vertiTiles));
	file.read((char *)&horiTiles, sizeof(horiTiles));

	char tempChar;

	file.read((char *)&tempChar, sizeof(tempChar));
	while (tempChar)
	{
		bitMapName += tempChar;
		file.read((char *)&tempChar, sizeof(tempChar));
	}

	for (int i = 0; i < vertiTiles; i++)
	{
		for (int j = 0; j < horiTiles; j++)
		{
			file.read((char *)&tempChar, sizeof(tempChar));
			tiles.push_back(tempChar);
		}
	}

	file.close();
}

void Tilemap::saveFile(const string &_file)
{
	fstream file;
	file.open(_file.c_str(), ios::out | ios::binary);

	file.write((char *)&tileRes, sizeof(tileRes));
	file.write((char *)&vertiTiles, sizeof(vertiTiles));
	file.write((char *)&horiTiles, sizeof(horiTiles));
	file << bitMapName << '\0';

	for (auto it = begin(tiles); it != end(tiles); ++it)
	{
		file.write((char *)&*it, sizeof(char));
	}

	file.close();
}

void Tilemap::update(Window *window)
{	
	if (fullTex) SDL_DestroyTexture(fullTex); 
	
	//render to tempTex instead of window
	//tempTex is needed because it can't be rendered??
	SDL_Texture *tempTex = SDL_CreateTexture(window->ren, SDL_PIXELFORMAT_RGBX8888, SDL_TEXTUREACCESS_TARGET, window->area.w, window->area.h);
	SDL_SetRenderTarget(window->ren, tempTex);

	//clear texture
	SDL_SetRenderDrawColor(window->ren, 0, 0, 0, 255);
	SDL_RenderClear(window->ren);

	//copy tiles to texture according to window offset
	for (int i = 0; i < window->area.h / tileRes; i++)
	{
		int y = i + window->offsetY;
		if (y < 0 || y >= vertiTiles) continue;
		for (int j = 0; j < window->area.w / tileRes; j++)
		{
			
			int x = j + window->offsetX;
			if (x < 0 || x >= horiTiles) continue;

			int in = y*horiTiles + x;

			SDL_Texture *tex;
			tex = sprites->frames[tiles[in]];

			SDL_Rect rect;
			rect.y = i*tileRes;
			rect.x = j*tileRes;
			rect.w = rect.h = tileRes;
			SDL_RenderCopy(window->ren, tex, NULL, &rect);
		}
	}
	fullTex = tempTex;
	SDL_SetRenderTarget(window->ren, NULL);
}

void Tilemap::render(Window *window)
{
	SDL_RenderCopy(window->ren, fullTex, NULL, NULL);
}

void Tilemap::create(char _tileRes, unsigned _vertiTiles, unsigned _horiTiles, const string& _bitMapName)
{
	tileRes = _tileRes;
	vertiTiles = _vertiTiles;
	horiTiles = _horiTiles;
	bitMapName = _bitMapName;
	tiles.resize(_vertiTiles*_horiTiles);
}

void Tilemap::changeTile(unsigned x, unsigned y, char type, Window *window)
{
	tiles[y*horiTiles + x] = type;
	if (fullTex) SDL_DestroyTexture(fullTex);

	//render to tempTex instead of window
	//tempTex is needed because it can't be rendered??
	SDL_Texture *tempTex = SDL_CreateTexture(window->ren, SDL_PIXELFORMAT_RGBX8888, SDL_TEXTUREACCESS_TARGET, window->area.w, window->area.h);
	SDL_SetRenderTarget(window->ren, tempTex);

	SDL_RenderCopy(window->ren, fullTex, NULL, NULL);


	SDL_Texture *tex;
	tex = sprites->frames[type];

	SDL_Rect rect;
	rect.y = y*tileRes;
	rect.x = x*tileRes;
	rect.w = rect.h = tileRes;
	SDL_RenderCopy(window->ren, tex, NULL, &rect);

	SDL_DestroyTexture(tex);

	fullTex = tempTex;
	SDL_SetRenderTarget(window->ren, NULL);
}

char Tilemap::getTile(unsigned x, unsigned y)
{
	return tiles[y*horiTiles + x];
}