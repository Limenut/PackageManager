#include <fstream>
#include <algorithm>  
#include <string>
#include "Window.h"
#include "Tilemap.h"
#include "Timer.h"
#include "Entity.h"

#pragma comment (lib, "SDL2_image.lib")

#ifdef main
#undef main
#endif

const int SCREEN_WIDTH = 896;
const int SCREEN_HEIGHT = 960;

Window mainWindow;
Entity pacman;
Tilemap gameMap;

bool quit;

using namespace std;

bool init()
{
	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		return false;
	}

	//Set texture filtering to linear
	if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
	{
		printf("Warning: Linear texture filtering not enabled!");
	}

	if (!mainWindow.init("PACMAN", SCREEN_WIDTH, SCREEN_HEIGHT)) return false;

	//Initialize PNG loading
	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) & imgFlags))
	{
		printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
		return false;
	}

	return true;
}

void close()
{
		IMG_Quit();
		SDL_Quit();
}

SDL_Texture* loadBitmap(const string &_file)
{
	if (!mainWindow.ren)
	{
		cout << "No renderer! Can't load texture!" << endl;
		return nullptr;
	}

	cout << "Loading " << _file.c_str() << "... ";

	SDL_Surface *surf = IMG_Load(_file.c_str());
	if (!surf)
	{
		cout << "Loading failed" << endl;
		return nullptr;
	}

	SDL_Rect rect;
	SDL_Texture *tex;

	rect.w = surf->w;
	rect.h = surf->h;

	tex = SDL_CreateTextureFromSurface(mainWindow.ren, surf);
	SDL_FreeSurface(surf);
	if (!tex)
	{
		cout << "Creating texture failed" << endl;
		return nullptr;
	}

	cout << "Ok" << endl;

	return tex;
}


bool testFile(const string &_file)
{
	std::ifstream infile(_file.c_str());
	return infile.good();
}

bool checkDirection(Entity entity, const Tilemap& map, Direction direction)
{
	unsigned x;
	unsigned y;

	switch (direction)
	{
	case LEFT:
	{
		x = entity.x - 1;
		y = entity.y;
		break;
	}
	case RIGHT:
	{
		x = entity.x + map.tileRes;
		y = entity.y;
		break;
	}
	case UP:
	{
		x = entity.x;
		y = entity.y - 1;
		break;
	}
	case DOWN:
	{
		x = entity.x;
		y = entity.y + map.tileRes;
		break;
	}
	default: return false;
	}
	
	x /= map.tileRes;
	y /= map.tileRes;

	if (x >= map.horiTiles || y >= map.vertiTiles) return false;

	switch (map.tiles[y*map.horiTiles + x])
	{
	case 0:
	case 1:
	case 2: return true;
	default: return false;
	}
}

bool checkAlignment(const Entity &entity, int tileRes)
{
	if (entity.x % tileRes + entity.y % tileRes == 0) return true;
	else return false;
	//else return chaotic evil
}

void handleEvents()
{
	SDL_Event e;
	const Uint8 *keystate = SDL_GetKeyboardState(NULL);

	while (SDL_PollEvent(&e))
	{
		if (e.type == SDL_WINDOWEVENT)
		{
			//windowMap[e.window.windowID]->handleEvents(&e);
		}
		if (e.type == SDL_QUIT)
		{
			quit = true;
		}
		if (e.type == SDL_KEYDOWN)
		{
			switch (e.key.keysym.sym)
			{
			case SDLK_UP:		pacman.requestDirection(UP);
				break;
			case SDLK_DOWN:		pacman.requestDirection(DOWN);
				break;
			case SDLK_LEFT:		pacman.requestDirection(LEFT);
				break;
			case SDLK_RIGHT:	pacman.requestDirection(RIGHT);
				break;
			default: break;
			}
		}
	}
}

int main()
{

	printf("Initializing SDL... ");
	if (init())
	{
		printf("success\n");
	}
	else
	{
		close();
		return 1;
	}

	Spritesheet pacSprite;
	pacSprite.makeSheet("pacSprites12.png", 32, &mainWindow);

	Spritesheet levelSprites;
	levelSprites.makeSheet("pacTiles.png", 32, &mainWindow);

	gameMap.sprites = &levelSprites;
	gameMap.loadFile("pacMap.map");

	gameMap.update(&mainWindow);
	//gameMap.changeTile(5, 5, 3, &mainWindow);

	pacman.sprites = &pacSprite;
	pacman.frameDelay = 5;
	pacman.speed = 1;
	pacman.direction = RIGHT;
	pacman.move(32, 32);


	//timer.start(START_CYCLE_TIME);
	while (!quit)
	{
		handleEvents();
		if (checkAlignment(pacman, 32) && checkDirection(pacman, gameMap, pacman.nextDirection))
		{
			pacman.updateDirection();
		}

		//timer.startCycle();
		SDL_SetRenderDrawColor(mainWindow.ren, 0, 0, 0, 255);
		SDL_RenderFillRect(mainWindow.ren, NULL);

		gameMap.render(&mainWindow);
		pacman.render(&mainWindow);	

		if (checkDirection(pacman, gameMap, pacman.direction)) 
		{ 
			pacman.move(); 
			pacman.animatePong();
		}
		

		SDL_RenderPresent(mainWindow.ren);

		SDL_Delay(6);

		/*if (timer.finish())
		{
			timer.reset();
		}*/
	}

	close();
}