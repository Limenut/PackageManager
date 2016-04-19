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
	void move();
	void move(int _x, int _y);
	void render(Window *window);
	void animatePong();
	void requestDirection(Direction _direction);
	void updateDirection();

	int x;
	int y;
	Spritesheet *sprites;
	Direction direction;
	Direction nextDirection;
	int speed;
	unsigned frameDelay;
	unsigned counter;
};