#include <fstream>
#include <algorithm>  
#include <string>
#include <chrono>
#include <thread>
#include <map>
#include "Window.h"
#include "Tilemap.h"
#include "Entity.h"

#pragma comment (lib, "SDL2_image.lib")

#ifdef main
#undef main
#endif

using namespace std::chrono;

const int SCREEN_WIDTH = 896;
const int SCREEN_HEIGHT = 960;
const microseconds frameTime = duration_cast<microseconds>(duration<long, ratio<1, 120>>{1});

Window mainWindow;
Entity pacman;
Tilemap gameMap;

int blinkyX = 0;
int blinkyY = 0;

bool quit;

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

bool checkDirection(const Entity &entity, const Tilemap& map, Direction direction)
{
	int x;
	int y;

	switch (direction)
	{
	case LEFT:
	{
		x = entity.x - entity.speed;
		y = entity.y;
		break;
	}
	case RIGHT:
	{
		x = entity.x + map.tileRes + entity.speed - 1;
		y = entity.y;
		break;
	}
	case UP:
	{
		x = entity.x;
		y = entity.y - entity.speed;
		break;
	}
	case DOWN:
	{
		x = entity.x;
		y = entity.y + map.tileRes + entity.speed - 1;
		break;
	}
	default: return false;
	}
	
	if (x < 0 || y < 0) return false;

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

Direction scanDirections(const Entity &entity, Tilemap& map)
{
	Direction directions = NONE;

	int x;
	int y;


	x = entity.tileX - 1;
	y = entity.tileY;
	if (x < map.horiTiles && y < map.vertiTiles && x > 0 && y > 0) 
	{
		if (map.getTile(x,y) < 3)
		{
			directions = (Direction)(directions | LEFT);
		}
	}

	x = entity.tileX + 1;
	y = entity.tileY;
	if (x < map.horiTiles && y < map.vertiTiles && x > 0 && y > 0)
	{
		if (map.getTile(x, y) < 3)
		{
			directions = (Direction)(directions | RIGHT);
		}
	}

	x = entity.tileX;
	y = entity.tileY - 1;
	if (x < map.horiTiles && y < map.vertiTiles && x > 0 && y > 0)
	{
		if (map.getTile(x, y) < 3)
		{
			directions = (Direction)(directions | UP);
		}
	}

	x = entity.tileX;
	y = entity.tileY + 1;
	if (x < map.horiTiles && y < map.vertiTiles && x > 0 && y > 0)
	{
		if (map.getTile(x, y) < 3)
		{
			directions = (Direction)(directions | DOWN);
		}
	}

	return directions;
}

Direction simpleChase2(const Ghost& chaser, const Entity& target, Direction directions)
{
	int deltaX = target.x - chaser.x;
	int deltaY = target.y - chaser.y;

	vector<Direction> choices;
	choices.push_back(RIGHT);
	choices.push_back(DOWN);
	choices.push_back(LEFT);
	choices.push_back(UP);

	//arrange directions from best to worst
	if (deltaX < 0)
		swap(choices[0], choices[2]);
	if (deltaY < 0)
		swap(choices[1], choices[3]);
	if (abs(deltaX) < abs(deltaY))
	{
		swap(choices[0], choices[1]);
		swap(choices[2], choices[3]);
	}

	//pick best one available
	for (auto &i : choices)
	{
		if (i & directions) return i;
	}
	cout << "error" << endl;
	return NONE;
}

Direction simpleChaseRetreat(const Ghost& chaser, const Entity& target, Direction directions)
{
	int deltaX = target.x - chaser.x;
	int deltaY = target.y - chaser.y;

	if (sqrt(deltaX*deltaX + deltaY*deltaY) > 8 * 32)
	{
		return simpleChase2(chaser, target, directions);
	}
	else
	{
		Entity dummy;
		dummy.x = chaser.homeX;
		dummy.y = chaser.homeY;

		return simpleChase2(chaser, dummy, directions);
	}
}

Direction randomChase(const Ghost& chaser, const Entity& target, Direction directions)
{

	vector<Direction> choices;
	if (directions & UP)	choices.push_back(UP);
	if (directions & DOWN)	choices.push_back(DOWN);
	if (directions & LEFT)	choices.push_back(LEFT);
	if (directions & RIGHT)	choices.push_back(RIGHT);

	if (choices.size() == 1) return choices.front();
	else return choices[rand() % choices.size()];
}

Direction predictChase(const Ghost& chaser, const Entity& target, Direction directions)
{
	Entity dummy;
	dummy.x = target.x;
	dummy.y = target.y;

	switch (target.direction)
	{
	case UP:	dummy.y -= 4 * 32; break;
	case DOWN:	dummy.y += 4 * 32; break;
	case LEFT:	dummy.x -= 4 * 32; break;
	case RIGHT:	dummy.x += 4 * 32; break;
	default:					   break;
	}

	return simpleChase2(chaser, dummy, directions);
}

Direction weirdChase(const Ghost& chaser, const Entity& target, Direction directions)
{
	Entity dummy;
	dummy.x = target.x;
	dummy.y = target.y;

	switch (target.direction)
	{
	case UP:	dummy.y -= 2 * 32; break;
	case DOWN:	dummy.y += 2 * 32; break;
	case LEFT:	dummy.x -= 2 * 32; break;
	case RIGHT:	dummy.x += 2 * 32; break;
	default:					   break;
	}

	//global -.-
	int deltaX = target.x - blinkyX;
	int deltaY = target.y - blinkyY;

	dummy.x -= deltaX;
	dummy.y -= deltaY;

	return simpleChase2(chaser, dummy, directions);
}

void navigate(Ghost& chaser, const Entity& target, Tilemap& map, Direction(*algorithm)(const Ghost&, const Entity&, Direction))
{
	Direction directions = scanDirections(chaser, map);

	//take opposite direction out, unless only option
	Direction opposite = NONE;
	if (chaser.direction)
	{
		switch (chaser.direction)
		{
		case UP:	opposite = DOWN;	break;
		case DOWN:	opposite = UP;		break;
		case LEFT:	opposite = RIGHT;	break;
		case RIGHT:	opposite = LEFT;	break;
		}

		directions = (Direction)(directions & ~opposite);

		if (directions == NONE) directions = opposite;
	}
	
	switch (directions)
	{
	case UP:	chaser.direction = UP;		break;
	case DOWN:	chaser.direction = DOWN;	break;
	case LEFT:	chaser.direction = LEFT;	break;
	case RIGHT:	chaser.direction = RIGHT;	break;
	default: chaser.direction = algorithm(chaser, target, directions); break;
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

	srand((int)time(NULL));

	unsigned dotsEaten = 0;

	Spritesheet pacSprite("Pac8frame.png", 32, &mainWindow);
	Spritesheet redSprite("ghost8red_eyes.png", 32, &mainWindow);
	Spritesheet cyanSprite("ghost8cya_eyes.png", 32, &mainWindow);
	Spritesheet pinkSprite("ghost8pin_eyes.png", 32, &mainWindow);
	Spritesheet orangeSprite("ghost8ora_eyes.png", 32, &mainWindow);
	Spritesheet levelSprites("pacTiles.png", 32, &mainWindow);

	gameMap.sprites = &levelSprites;
	gameMap.loadFile("pacMap.map");

	gameMap.update(&mainWindow);

	pacman.sprites = &pacSprite;
	pacman.frameDelay = 1;
	pacman.speed = 2;
	pacman.direction = RIGHT;
	pacman.move(432, 512);

	map<string, Ghost> ghosts;

	ghosts["red"] = Ghost();
	ghosts["red"].sprites = &redSprite;
	ghosts["red"].move(384, 416);
	ghosts["pink"].homeX = 800;
	ghosts["pink"].homeY = -96;

	ghosts["cyan"] = Ghost();
	ghosts["cyan"].sprites = &cyanSprite;
	ghosts["cyan"].move(416, 416);
	ghosts["cyan"].homeX = 864;
	ghosts["cyan"].homeY = 1024;

	ghosts["pink"] = Ghost();
	ghosts["pink"].sprites = &pinkSprite;
	ghosts["pink"].move(448, 416);
	ghosts["pink"].homeX = 64;
	ghosts["pink"].homeY = -96;

	ghosts["orange"] = Ghost();
	ghosts["orange"].sprites = &orangeSprite;
	ghosts["orange"].move(480, 416);
	ghosts["orange"].homeX = 0;
	ghosts["orange"].homeY = 1024;

	for (auto &g : ghosts)
	{
		g.second.frameDelay = 7;
		g.second.speed = 1;
		g.second.isActive = false;
	}
	ghosts["red"].activate();
	ghosts["pink"].activate();

	system_clock::time_point targetTime = system_clock::now();
	while (!quit)
	{

		handleEvents();
		if (pacman.checkAlignment())
		{
			pacman.updateTile();
			if (pacman.nextDirection & scanDirections(pacman, gameMap))
			{
				pacman.updateDirection();
			}
			char tile = gameMap.getTile(pacman.tileX, pacman.tileY);
			if (tile == 1 || tile == 2)	//eat
			{
				gameMap.changeTile(pacman.tileX, pacman.tileY, 0, &mainWindow);
				gameMap.update(&mainWindow);
				dotsEaten++;
				if (!ghosts["cyan"].isActive && dotsEaten >= 30) ghosts["cyan"].activate();
				else if (!ghosts["orange"].isActive && dotsEaten >= 60) ghosts["orange"].activate();
			}
		}
		
		if (ghosts["red"].isActive && ghosts["red"].checkAlignment())
		{
			ghosts["red"].updateTile();
			navigate(ghosts["red"], pacman, gameMap, simpleChase2);
		}
		if (ghosts["cyan"].isActive && ghosts["cyan"].checkAlignment())
		{
			ghosts["cyan"].updateTile();

			blinkyX = ghosts["red"].x;
			blinkyY = ghosts["red"].y;

			navigate(ghosts["cyan"], pacman, gameMap, weirdChase);
		}
		if (ghosts["pink"].isActive && ghosts["pink"].checkAlignment())
		{
			ghosts["pink"].updateTile();
			navigate(ghosts["pink"], pacman, gameMap, predictChase);
		}
		if (ghosts["orange"].isActive && ghosts["orange"].checkAlignment())
		{
			ghosts["orange"].updateTile();	
			//navigate(ghosts["orange"], pacman, gameMap, simpleChaseRetreat);
			ghosts["orange"].distance(pacman) > 8 * 32 ?
				ghosts["orange"].setTarget(pacman) : ghosts["orange"].setScatter();

			ghosts["orange"].navigate(scanDirections(ghosts["orange"], gameMap));
			
		}

		SDL_SetRenderDrawColor(mainWindow.ren, 0, 0, 0, 255);
		SDL_RenderFillRect(mainWindow.ren, NULL);

		gameMap.render(&mainWindow);
		pacman.renderRotated(&mainWindow);	

		for (auto &g : ghosts)
		{
			g.second.render(&mainWindow);
			g.second.animateLoop();
			if (g.second.isActive) g.second.move();		
		}

		SDL_RenderPresent(mainWindow.ren);

		if (checkDirection(pacman, gameMap, pacman.direction)) 
		{ 
			pacman.move(); 
			pacman.animatePong();
		}

		targetTime += frameTime;
		std::this_thread::sleep_until(targetTime);
	}

	close();
}