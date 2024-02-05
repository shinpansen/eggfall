#include "grass.hpp"

static const float SCALE = 30.f;

using namespace std;
using namespace sf;

grass::grass(b2World * world, Sprite * tile, Texture * groundTexture, Texture * clumpsTexture, int groundType, bool addGrass, bool addClumps, bool leftGrass, bool rightGrass)
{
	//Variables - Objects
	_drawGrassClumps = false;
	_drawLeftGrass = leftGrass;
	_drawRightGrass = rightGrass;
	_angle = 0.f;
	_angleToReach = 0.f;
	_world = world;
	_tile = tile;
	_groundTexture = groundTexture;
	_clumpsTexture = clumpsTexture;

	if (!addGrass || _tile->getPosition().y < 0)
		return;

	//Grass drawing
	_grass.setTexture(*_groundTexture);
	_grass.setTextureRect(IntRect(tile->getTextureRect().left + 448, groundType*160, 32, 64));
	_grass.setPosition(tile->getPosition().x, tile->getPosition().y - 32);
	if (_drawLeftGrass)
	{
		_leftGrass.setTexture(*_groundTexture);
		_leftGrass.setTextureRect(IntRect(448, groundType * 160, 32, 64));
		_leftGrass.setPosition(tile->getPosition().x - 32, tile->getPosition().y - 32);
	}
	if (_drawRightGrass)
	{
		_rightGrass.setTexture(*_groundTexture);
		_rightGrass.setTextureRect(IntRect(640, groundType * 160, 32, 64));
		_rightGrass.setPosition(tile->getPosition().x + 32, tile->getPosition().y - 32);
	}

	//Drawing grass clumps
	int maxY = _tile->getPosition().y - 4;
	if (addClumps && _tile->getPosition().y > 0 && utils::RandomNumber(10, 1) != 1)
	{
		_drawGrassClumps = true;
		_grassClumps.setTexture(*_clumpsTexture);
		_grassClumps.setTextureRect(IntRect(utils::RandomNumber(7, 0) * 28, 24*groundType, 28, 24));
		_grassClumps.setOrigin(14, 24);
		if(_tile->getTextureRect().left == 32 || _tile->getTextureRect().left == 160)
			_grassClumps.setPosition(_tile->getPosition().x + utils::RandomNumber(22, 10), maxY);
		else
			_grassClumps.setPosition(_tile->getPosition().x + utils::RandomNumber(28, 4), maxY);
	}
}

void grass::Step(float deltaTime)
{
	try
	{
		//Calculating angle
		Vector2f position = _grassClumps.getPosition();
		for (b2Body* body = _world->GetBodyList(); body != 0; body = body->GetNext())
		{
			if (body->GetType() == b2_dynamicBody && (int)body->GetUserData() != UD_DASH &&
				//fabs(body->GetLinearVelocity().x) > 0.5 &&
				fabs(body->GetPosition().x*SCALE - position.x) < 12 &&
				fabs(body->GetPosition().y*SCALE - position.y) < 40 &&
				body->GetPosition().y*SCALE < position.y)
			{
				//Calculating initial angle
				int x = body->GetPosition().x*SCALE;
				int y = body->GetPosition().y*SCALE;
				float yDistance = fabs(body->GetPosition().y*SCALE - position.y);
				yDistance = yDistance < 1 ? 1 : 1.f - (yDistance / 100.f);
				_angleToReach = (body->GetPosition().x*SCALE - position.x)*(3.f * yDistance);
				_angleToReach = fabs(_angleToReach) > 80.f ? _angleToReach / fabs(_angleToReach)*80.f : _angleToReach;
				break;
			}
		}

		//Set angle + scale
		_grassClumps.setRotation(_angle);
		_grassClumps.setScale(1.f - (fabs(_angle) / 90.f), 1.f + (fabs(_angle) / 90.f));

		//Perform rotation
		if (fabs(_angle - _angleToReach) < 1 && fabs(_angleToReach) >= 1)
			_angleToReach = -_angleToReach / 2;
		else if (fabs(_angleToReach) >= 1)
			_angle += (float)((_angleToReach - _angle) / 2.5f)*deltaTime;
		else
			_angleToReach = 0;
	}
	catch (exception e)
	{
		cout << e.what() << endl;
	}

}

void grass::DisplayGrass(RenderWindow * window)
{
	window->draw(_grass);
	if(_drawLeftGrass)
		window->draw(_leftGrass);
	if (_drawRightGrass)
		window->draw(_rightGrass);
}

void grass::DisplayGrassClumps(RenderWindow * window)
{
	if (_drawGrassClumps)
		window->draw(_grassClumps);
}
