#pragma once

#include "Spritesheet.h"

enum Direction
{
	NONE = 0,
	UP = 1,
	DOWN = 2,
	LEFT = 4,
	RIGHT = 8
};

class Entity
{
public:

	Entity();
	Entity(Spritesheet *_sprites, unsigned _frameDelay);
	~Entity();

	void move();				//move forward
	void move(int _x, int _y);
	void render(Window *window);
	void renderRotated(Window *window);
	void animateLoop();
	void animatePong();
	void requestDirection(Direction _direction);
	void updateDirection();		//change to requested direction
	void updateTile();			//update tileX and tileY
	bool checkAlignment();		//perfect alignment with tile grid

	//position in pixels
	int x;
	int y;
	//position in tiles
	int tileX;
	int tileY;
	Spritesheet *sprites;
	Direction direction;
	Direction nextDirection;
	int speed;
	unsigned frameDelay;	//frameDelay + 1 == for how many frames a single animation frame lasts for
	unsigned counter;		//used for animation
};