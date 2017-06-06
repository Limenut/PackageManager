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
	currentFrame = 0;
	animForward = true;
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
	currentFrame = 0;
	animForward = true;
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

	SDL_RenderCopy(window->ren, sprites->frames[currentFrame], NULL, &rc);
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
	case DOWN: angle = 90; rc.x--;	break;
	case LEFT: angle = 180; rc.y--;	break;
	case UP: angle = 270;	break;
	default: angle = 0; break;
	}

	SDL_RenderCopyEx(window->ren, sprites->frames[currentFrame], NULL, &rc, angle, NULL, SDL_FLIP_NONE);
}

void Entity::animateLoop()
{
	if (counter) counter--;
	else
	{
		counter = frameDelay;

		currentFrame++;
		if (currentFrame == sprites->frames.size()) currentFrame = 0;
	}
}

void Entity::animatePong()
{
	if (counter) counter--;
	else
	{
		counter = frameDelay;

		if (currentFrame == 0) animForward = true;
		if (currentFrame == sprites->frames.size() - 1)  animForward = false;

		if (animForward) currentFrame++;
		else currentFrame--;
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
	if (x % sprites->tileRes + y % sprites->tileRes == 0) { return isAligned = true;}
	else { return isAligned = false;}
	//else return chaotic evil
}

int Entity::distance(Entity target)
{
	int deltaX = target.x - x;
	int deltaY = target.y - y;

	return int(sqrt(deltaX*deltaX + deltaY*deltaY) + 0.5);
}

void Entity::setStart(int x, int y)
{
	startX = x;
	startY = y;
	move(x, y);
}

void Entity::resetPos()
{
	move(startX, startY);
}

void Ghost::setTarget(Entity target)
{
	targetX = target.x;
	targetY = target.y;
	//mode = CHASE;
}

void Ghost::setTarget(int x, int y)
{
	targetX = x;
	targetY = y;
	//mode = CHASE;
}

void Ghost::navigate(Direction directions)
{
	Direction opposite = NONE;
	if (direction)
	{
		switch (direction)
		{
		case UP:	opposite = DOWN;	break;
		case DOWN:	opposite = UP;		break;
		case LEFT:	opposite = RIGHT;	break;
		case RIGHT:	opposite = LEFT;	break;
		}

		directions = (Direction)(directions & ~opposite);

		if (directions == NONE) directions = opposite;
		else if (letsReverse)
		{
			directions = opposite;
			letsReverse = false;
		}
	}

	switch (directions)
	{
	case UP:	direction = UP;		return;
	case DOWN:	direction = DOWN;	return;
	case LEFT:	direction = LEFT;	return;
	case RIGHT:	direction = RIGHT;	return;
	default: break;
	}

	

	switch (mode)
	{
	case INACTIVE:														break;
	case CHASE:		direction = chase(targetX, targetY, directions);	break;
	case SCATTER:	direction = chase(homeX, homeY, directions);		break;
	case AFRAID:	direction = flee(directions);						break;
	case DEAD:		direction = home();									break;
	}
}

Direction Ghost::chase(int _x, int _y, Direction directions)
{
	int deltaX = _x - x;
	int deltaY = _y - y;

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

Direction Ghost::flee(Direction directions)
{
	vector<Direction> choices;
	if (directions & UP)	choices.push_back(UP);
	if (directions & DOWN)	choices.push_back(DOWN);
	if (directions & LEFT)	choices.push_back(LEFT);
	if (directions & RIGHT)	choices.push_back(RIGHT);

	if (choices.size() == 1) return choices.front();
	else return choices[rand() % choices.size()];
}

Direction Ghost::home()
{
	if (x < startX) return RIGHT;
	else if (x > startX) return LEFT;
	else if (y < startY) return DOWN;
	else return UP;
}