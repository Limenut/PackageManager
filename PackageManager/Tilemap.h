#pragma once

#include <iostream>
#include <SDL2/SDL.h>
#include "Spritesheet.h"

using namespace std;

//this contains the types (indices) of tiles in the game level
//is analogous to the .map format
class Tilemap
{
public:
	Tilemap();
	Tilemap(Spritesheet *_sprites);
	~Tilemap();

	char tileRes;			//vertical and horizontal resolution of tiles
	int vertiTiles;			//size of map in tiles
	int horiTiles;
	string bitMapName;		//which image the tile textures are fetched from
	vector<char> tiles;		//type of tile
	SDL_Texture* fullTex;	//texture to be rendered
	Spritesheet *sprites;

	void loadFile(const string &_file);
	void saveFile(const string &_file);
	void create(char _tileRes, unsigned _vertiTiles, unsigned _horiTiles, const string& _bitMapName);
	void render(Window *window);
	void update(Window *window);
	void changeTile(unsigned x, unsigned y, char type, Window *window);
	char getTile(unsigned x, unsigned y);
};