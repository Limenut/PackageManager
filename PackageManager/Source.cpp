#include <fstream>
#include <algorithm>  
#include <string>
#include <chrono>
#include <thread>
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
const int FPS = 120;
const microseconds frameTime = microseconds(8333);

Window mainWindow;
Entity pacman;
Tilemap gameMap;

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

	if (x < 0 || y < 0) return false;
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

Direction simpleChase(const Entity& chaser, const Entity& target, Direction directions)
{
	int deltaX = target.x - chaser.x;
	int deltaY = target.y - chaser.y;

	if (directions & UP && directions & DOWN)		//pick best vertical direction
	{ 
		if (deltaY > 0) directions = Direction(directions & ~UP);
		else directions = Direction(directions & ~DOWN);
	}
	if (directions & LEFT && directions & RIGHT)	//pic best horizontal direction
	{
		if (deltaX > 0) directions = Direction(directions & ~LEFT);
		else directions = Direction(directions & ~RIGHT);
	}

	//1. return if direction is determined
	if (directions == UP || directions == DOWN || directions == LEFT || directions == RIGHT) return directions;
	
	//2. check if one and only one direction is good
	if (directions & DOWN && deltaY > 0 || directions & UP && deltaY <= 0)
	{
		if (directions & RIGHT && deltaX > 0 || directions & LEFT && deltaX <= 0)
		{
		}
		else 
		{ 
			cout << "sdf";
			return Direction(directions & (UP | DOWN)); 
			
		}
	}
	else
	{
		if (directions & RIGHT && deltaX > 0 || directions & LEFT && deltaX <= 0)
		{	
			return Direction(directions & (LEFT | RIGHT));
		}
		else 
		{
		}
	}

	
	//3. choose the better direction based on distance
	if (abs(deltaY) > abs(deltaX))
	{
		return Direction(directions & (LEFT | RIGHT));
	}
	else 
	{ 
		return Direction(directions & (UP | DOWN)); 
	}
}

Direction simpleChase2(const Entity& chaser, const Entity& target, Direction directions)
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

Direction simpleChaseRetreat(const Entity& chaser, const Entity& target, Direction directions)
{
	int deltaX = target.x - chaser.x;
	int deltaY = target.y - chaser.y;

	if (sqrt(deltaX*deltaX + deltaY*deltaY) > 256)
	{
		return simpleChase2(chaser, target, directions);
	}
	else
	{
		Entity dummy;
		dummy.x = 32;
		dummy.y = 32;

		return simpleChase2(chaser, dummy, directions);
	}
}

Direction randomChase(const Entity& chaser, const Entity& target, Direction directions)
{

	vector<Direction> choices;
	if (directions & UP)	choices.push_back(UP);
	if (directions & DOWN)	choices.push_back(DOWN);
	if (directions & LEFT)	choices.push_back(LEFT);
	if (directions & RIGHT)	choices.push_back(RIGHT);

	if (choices.size() == 1) return choices.front();
	else return choices[rand() % choices.size()];
}

void navigate(Entity& chaser, const Entity& target, Tilemap& map, Direction(*algorithm)(const Entity&, const Entity&, Direction))
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

	Spritesheet pacSprite;
	Spritesheet redSprite;
	Spritesheet cyanSprite;
	Spritesheet orangeSprite;
	pacSprite.makeSheet("Pac8frame.png", 32, &mainWindow);
	redSprite.makeSheet("ghost8.png", 32, &mainWindow);
	cyanSprite.makeSheet("ghost8cya.png", 32, &mainWindow);
	orangeSprite.makeSheet("ghost8ora.png", 32, &mainWindow);

	

	Spritesheet levelSprites;
	levelSprites.makeSheet("pacTiles.png", 32, &mainWindow);

	gameMap.sprites = &levelSprites;
	gameMap.loadFile("pacMap.map");

	gameMap.update(&mainWindow);

	pacman.sprites = &pacSprite;
	pacman.frameDelay = 1;
	pacman.speed = 2;
	pacman.direction = RIGHT;
	pacman.move(32, 32);

	Entity redGhost;
	redGhost.sprites = &redSprite;
	redGhost.frameDelay = 7;
	redGhost.speed = 1;
	redGhost.move(384, 416);

	Entity cyanGhost;
	cyanGhost.sprites = &cyanSprite;
	cyanGhost.frameDelay = 7;
	cyanGhost.speed = 1;
	cyanGhost.move(448, 416);

	Entity orangeGhost;
	orangeGhost.sprites = &orangeSprite;
	orangeGhost.frameDelay = 7;
	orangeGhost.speed = 1;
	orangeGhost.move(480, 416);

	system_clock::time_point targetTime = system_clock::now();
	while (!quit)
	{

		handleEvents();
		if (checkAlignment(pacman, 32))
		{
			pacman.updateTile();
			if (pacman.nextDirection & scanDirections(pacman, gameMap))
			{
				pacman.updateDirection();
			}
			char tile = gameMap.getTile(pacman.tileX, pacman.tileY);
			if (tile == 1 || tile == 2)
			{
				gameMap.changeTile(pacman.tileX, pacman.tileY, 0, &mainWindow);
				gameMap.update(&mainWindow);
			}
		}
		if (checkAlignment(redGhost, 32))
		{
			redGhost.updateTile();
			navigate(redGhost, pacman, gameMap, simpleChase2);
		}
		if (checkAlignment(cyanGhost, 32))
		{
			cyanGhost.updateTile();
			navigate(cyanGhost, pacman, gameMap, randomChase);
		}
		if (checkAlignment(orangeGhost, 32))
		{
			orangeGhost.updateTile();
			navigate(orangeGhost, pacman, gameMap, simpleChaseRetreat);
		}
		
		SDL_SetRenderDrawColor(mainWindow.ren, 0, 0, 0, 255);
		SDL_RenderFillRect(mainWindow.ren, NULL);

		gameMap.render(&mainWindow);
		pacman.renderRotated(&mainWindow);	
		redGhost.render(&mainWindow);
		cyanGhost.render(&mainWindow);
		orangeGhost.render(&mainWindow);

		SDL_RenderPresent(mainWindow.ren);

		if (checkDirection(pacman, gameMap, pacman.direction)) 
		{ 
			pacman.move(); 
			pacman.animatePong();
		}
		redGhost.move();
		redGhost.animateLoop();
		orangeGhost.move();
		orangeGhost.animateLoop();
		cyanGhost.move();
		cyanGhost.animateLoop();	

		targetTime += frameTime;
		//targetTime += duration_cast<ratio<1, FPS>>;
		std::this_thread::sleep_until(targetTime);
	}

	close();
}