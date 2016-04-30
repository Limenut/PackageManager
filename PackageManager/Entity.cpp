#include "Entity.h"

Entity::Entity()
{
	x = 0;
	y = 0;
	sprites = nullptr;
	frameDelay = 0;
	direction = NONE;
	nextDirection = NONE;
	speed = 0;
	counter = 0;
}

Entity::Entity(Spritesheet *_sprites, unsigned _frameDelay)
{
	x = 0;
	y = 0;
	sprites = _sprites;
	frameDelay = _frameDelay;
	direction = NONE;
	speed = 0;
	counter = _frameDelay;
}

Entity::~Entity()
{

}

void Entity::move()
{
	switch (direction)
	{
	case UP: y -= speed;	break;
	case DOWN: y += speed;	break;
	case LEFT: x -= speed;	break;
	case RIGHT: x += speed; break;
	default: break;
	}
}

void Entity::move(int _x, int _y)
{
	x = _x;
	y = _y;
}

void Entity::render(Window *window)
{
	SDL_Rect rc;
	rc.x = x;
	rc.y = y;
	rc.h = rc.w = sprites->tileRes;

	SDL_RenderCopy(window->ren, sprites->frames[sprites->currentFrame], NULL, &rc);
}

void Entity::renderRotated(Window *window)
{
	SDL_Rect rc;
	rc.x = x;
	rc.y = y;
	rc.h = rc.w = sprites->tileRes;

	int angle;
	switch (direction)
	{
	case RIGHT:	angle = 0;	break;
	case DOWN: angle = 90;	break;
	case LEFT: angle = 180;	break;
	case UP: angle = 270;	break;
	default: angle = 0; break;
	}

	SDL_RenderCopyEx(window->ren, sprites->frames[sprites->currentFrame], NULL, &rc, angle, NULL, SDL_FLIP_NONE);
}

void Entity::animateLoop()
{
	if (counter) counter--;
	else
	{
		counter = frameDelay;
		sprites->nextFrameLoop();
	}
}

void Entity::animatePong()
{
	if (counter) counter--;
	else
	{
		counter = frameDelay;
		sprites->nextFramePong();
	}
}

void Entity::requestDirection(Direction _direction)
{
	nextDirection = _direction;
	//if current and intended direction are exactly opposite, turn immediately
	if (((direction | _direction) == (UP | DOWN )) | ((direction | _direction) == (LEFT | RIGHT)))
	{
		updateDirection();
	}
}

void Entity::updateDirection()
{
	if (nextDirection == NONE) return;
	else
	{
		direction = nextDirection;
		nextDirection = NONE;
	}	
}

void Entity::updateTile()
{
	tileX = x / sprites->tileRes;
	tileY = y / sprites->tileRes;
}

bool Entity::checkAlignment()
{
	if (x % sprites->tileRes + y % sprites->tileRes == 0) return true;
	else return false;
	//else return chaotic evil
}
