#include "scalablePlatform.hpp"

using namespace std;
using namespace sf;

boxResize::boxResize()
{
	enabled = false;
	left = right = top = bottom = 0.f;
}

boxResize::boxResize(float left, float right, float top, float bottom)
{
	this->enabled = true;
	this->left = left;
	this->right = right;
	this->top = top;
	this->bottom = bottom;
}

scalablePlatform::~scalablePlatform()
{
	//Destroying Box2D bodies
	for (int i = 0; i < _platformBodies.size(); i++)
		if(_platformBodies[i] != NULL)
			_world->DestroyBody(_platformBodies[i]);

	//Destroying SFML sprites
	for (int i = 0; i < _platformSprites.size(); i++)
		if (_platformSprites[i] != NULL)
			delete _platformSprites[i];
}

scalablePlatform::scalablePlatform(b2World* world, Texture* texture, Vector2f pos, int maxNbBlocks, int nbBlocks, int maxColorID, int colorID, short bodyEntity, 
								   IntRect firstRectTile, IntRect lastRectTile, PLATFORM_ALIGN align, boxResize bodyResize, int * fixedMiddleTiles, bool disablePhysics)
{
	//Variable - objects
	_pos = pos;
	_nbBlocks = nbBlocks >= 3 ? nbBlocks : 3;
	_maxNbBlocks = maxNbBlocks;
	_nbBlocks = _nbBlocks <= _maxNbBlocks ? _nbBlocks : _maxNbBlocks;
	_maxColorID = maxColorID;
	_colorID = colorID >= 0 ? colorID : 0;
	_colorID = colorID <= _maxColorID ? colorID : _maxColorID;
	_bodyEntity = bodyEntity;
	_firstRectTile = firstRectTile;
	_lastRectTile = lastRectTile;
	_align = align;
	_world = world;
	_ladderPlatformTexture = texture;
	_bodyResize = bodyResize;
	_fixedMiddleTiles = fixedMiddleTiles;

	//Sprites initialization
	_tileSize = Vector2f(_firstRectTile.width, _firstRectTile.height);
	for (int i = 0 ; i < _nbBlocks ; i++)
	{
		//Tile rect
		Vector2f tileRect = GetIntRect(i);

		//Sprite creation
		_platformSprites.push_back(new Sprite());
		_platformSprites[i]->setTexture(*_ladderPlatformTexture);
		_platformSprites[i]->setTextureRect(IntRect(tileRect.x, tileRect.y, _tileSize.x, _tileSize.y));
		if (_align == PLATFORM_HORIZONTAL)
			_platformSprites[i]->setPosition(pos.x - (_nbBlocks * _tileSize.x) / 2 + (i * _tileSize.x), pos.y - _tileSize.y/2.f);
		else
			_platformSprites[i]->setPosition(pos.x - _tileSize.x/2.f, pos.y - (_nbBlocks * _tileSize.y)/2 + (i * _tileSize.y));
	}
	SetPosition(_pos, disablePhysics);
}

int scalablePlatform::GetMaxColorID()
{
	return _maxColorID;
}

int scalablePlatform::GetColorID()
{
	return _colorID;
}

int scalablePlatform::GetMaxNbBlocks()
{
	return _maxNbBlocks;
}

int scalablePlatform::GetNbBlocks()
{
	return _nbBlocks;
}

int * scalablePlatform::GetMiddleTiles()
{
	int * middleTiles = new int[_nbBlocks - 2];
	for (int i = 0; i < _nbBlocks - 2; i++)
	{
		IntRect rect = _platformSprites[i + 1]->getTextureRect();
		if(_align == PLATFORM_HORIZONTAL)
			middleTiles[i] = (rect.left - _firstRectTile.left - _tileSize.x) / _tileSize.x;
		else 
			middleTiles[i] = (rect.top - _firstRectTile.top - _tileSize.y) / _tileSize.y;
	}
	return middleTiles;
}

string scalablePlatform::GetMiddleTilesString()
{
	_fixedMiddleTiles = GetMiddleTiles();
	string middleTiles = "";
	for (int i = 0; i < _nbBlocks - 2; i++)
	{
		stringstream stream;
		stream << _fixedMiddleTiles[i];
		middleTiles += stream.str() + ",";
	}
	return middleTiles;
}

Vector2f scalablePlatform::GetPosition()
{
	return _pos;
}

Vector2f scalablePlatform::GetIntRect(int i)
{
	//First tile
	int xRect = _firstRectTile.left;
	xRect += _align == PLATFORM_VERTICAL ? _colorID * _tileSize.x : 0;
	int yRect = _firstRectTile.top;
	yRect += _align == PLATFORM_HORIZONTAL ? _colorID * _tileSize.y : 0;
	bool useOtherMidBlocks = utils::RandomNumber(3, 0) == 0;

	//Middle and last tile
	if (_align == PLATFORM_HORIZONTAL && i > 0)
	{
		int midBlocks = (_lastRectTile.left - _firstRectTile.left - _tileSize.x) / _tileSize.x;
		if (i == _nbBlocks - 1)
			xRect = _lastRectTile.left;
		else
		{
			if(_fixedMiddleTiles != NULL && i-1 >= 0 && i-1 < _nbBlocks-2)
				xRect = _fixedMiddleTiles[i-1] * _tileSize.x + _firstRectTile.left + _tileSize.x;
			else if(useOtherMidBlocks)
				xRect = utils::RandomNumber(midBlocks - 1, 0) * _tileSize.x + _firstRectTile.left + _tileSize.x;
			else
				xRect = _firstRectTile.left + _tileSize.x;
		}
	}
	else if (_align == PLATFORM_VERTICAL && i > 0)
	{
		int midBlocks = (_lastRectTile.top - _firstRectTile.top - _tileSize.y) / _tileSize.y;
		if (i == _nbBlocks - 1)
			yRect = _lastRectTile.top;
		else
		{
			if (_fixedMiddleTiles != NULL && i - 1 >= 0 && i - 1 < _nbBlocks - 2)
				yRect = _fixedMiddleTiles[i-1] * _tileSize.y + _firstRectTile.top + _tileSize.y;
			else if (useOtherMidBlocks)
				yRect = utils::RandomNumber(midBlocks - 1, 0) * _tileSize.y + _firstRectTile.top + _tileSize.y;
			else
				yRect = _firstRectTile.top + _tileSize.y;
		}
	}
	return Vector2f(xRect, yRect);
}

vector<FloatRect> scalablePlatform::GetGlobalBounds(bool grabBounds)
{
	//Default rect
	FloatRect rect;
	FloatRect windowRect = FloatRect(0, 0, utils::VIEW_WIDTH, utils::VIEW_HEIGHT);
	if (_align == PLATFORM_HORIZONTAL)
		rect = FloatRect(_pos.x - (_nbBlocks * _tileSize.x) / 2.f, _pos.y - _tileSize.y / 2.f, _tileSize.x * _nbBlocks, _tileSize.y);
	else
		rect = FloatRect(_pos.x - _tileSize.x / 2.f, _pos.y - (_nbBlocks * _tileSize.y) / 2.f, _tileSize.x, _tileSize.y * _nbBlocks);

	//Resize if necessary
	if (_bodyResize.enabled)
	{
		rect.width -= grabBounds && _align == PLATFORM_VERTICAL ? 0 : _bodyResize.left + _bodyResize.right;
		rect.left += grabBounds && _align == PLATFORM_VERTICAL ? 0 : _bodyResize.left;
		rect.height -= grabBounds && _align == PLATFORM_HORIZONTAL ? 0 : _bodyResize.top + _bodyResize.bottom;
		rect.top += grabBounds && _align == PLATFORM_HORIZONTAL ? 0 : _bodyResize.top;
	}
	vector<FloatRect> bounds;
	bounds.push_back(rect);

	//Calculating other bounds
	FloatRect rectLeftRight = rect;
	rectLeftRight.left += rect.left + rect.width / 2 > utils::VIEW_WIDTH / 2 ? -utils::VIEW_WIDTH : utils::VIEW_WIDTH;
	FloatRect rectTopBottom = rect;
	rectTopBottom.top += rect.top + rect.height / 2 > utils::VIEW_HEIGHT / 2 ? -utils::VIEW_HEIGHT : utils::VIEW_HEIGHT;
	FloatRect rectFullSide = rectLeftRight;
	rectFullSide.top = rectTopBottom.top;

	//Adding other bounds if necessary
	if (rectLeftRight.intersects(windowRect))
		bounds.push_back(rectLeftRight);
	if (rectTopBottom.intersects(windowRect))
		bounds.push_back(rectTopBottom);
	if (rectFullSide.intersects(windowRect))
		bounds.push_back(rectFullSide);
	return bounds;
}

void scalablePlatform::SetPosition(Vector2f pos, bool disablePhysics)
{
	//Moving sprite
	if (pos != _pos)
	{
		Vector2f moveVect = Vector2f(pos.x - _pos.x, pos.y - _pos.y);
		for (int i = 0; i < _platformSprites.size(); i++)
			_platformSprites[i]->move(moveVect);
		_pos = pos;
	}

	//Delete all Box2D bodies
	for (int i = 0; i < _platformBodies.size(); i++)
		if (_platformBodies[i] != NULL)
			_world->DestroyBody(_platformBodies[i]);
	_platformBodies.clear();

	//For editor mode - physics is useless here
	if (disablePhysics)
		return;

	float platformWidth = _align == PLATFORM_HORIZONTAL ? _tileSize.x * (float)_nbBlocks : _tileSize.x;
	float platformHeight = _align == PLATFORM_VERTICAL ? _tileSize.y * (float)_nbBlocks : _tileSize.y;
	if (_bodyResize.enabled)
	{
		platformWidth -= _bodyResize.left + _bodyResize.right;
		pos.x += _bodyResize.left / 2.f;
		pos.x -= _bodyResize.right / 2.f;
		platformHeight -= _bodyResize.top + _bodyResize.bottom;
		pos.y += _bodyResize.top / 2.f;
		pos.y -= _bodyResize.bottom / 2.f;
	}

	//Adding first body
	_platformBodies.push_back(Box2DTools::CreateRectangleBox(_world, pos, platformWidth, platformHeight, 0, 0, 0, _bodyEntity));

	//Manage loop
	if (_platformSprites[0]->getPosition().y < 0)
		_platformBodies.push_back(Box2DTools::CreateRectangleBox(_world, Vector2f(pos.x, pos.y + utils::VIEW_HEIGHT), platformWidth, platformHeight, 0, 0, 0, _bodyEntity));
	else if (_platformSprites[_platformSprites.size() - 1]->getPosition().y + _tileSize.y > utils::VIEW_HEIGHT)
		_platformBodies.push_back(Box2DTools::CreateRectangleBox(_world, Vector2f(pos.x, pos.y - utils::VIEW_HEIGHT), platformWidth, platformHeight, 0, 0, 0, _bodyEntity));
	if (_platformSprites[0]->getPosition().x < 0)
		_platformBodies.push_back(Box2DTools::CreateRectangleBox(_world, Vector2f(pos.x + utils::VIEW_WIDTH, pos.y), platformWidth, platformHeight, 0, 0, 0, _bodyEntity));
	else if (_platformSprites[_platformSprites.size() - 1]->getPosition().x + _tileSize.x > utils::VIEW_WIDTH)
		_platformBodies.push_back(Box2DTools::CreateRectangleBox(_world, Vector2f(pos.x - utils::VIEW_WIDTH, pos.y), platformWidth, platformHeight, 0, 0, 0, _bodyEntity));
	if (_platformBodies.size() == 3)
	{
		float signeX = _platformSprites[0]->getPosition().x < 0 ? 1 : -1;
		float signeY = _platformSprites[0]->getPosition().y < 0 ? 1 : -1;
		_platformBodies.push_back(Box2DTools::CreateRectangleBox(_world, Vector2f(pos.x + utils::VIEW_WIDTH*signeX,
			pos.y + utils::VIEW_HEIGHT*signeY), platformWidth, platformHeight, 0, 0, 0, _bodyEntity));
	}
}

void scalablePlatform::Move(float x, float y, bool disablePhysics)
{
	SetPosition(Vector2f(_pos.x + x, _pos.y + y), disablePhysics);
}

void scalablePlatform::SetColor(Color color)
{
	for (int i = 0; i < _platformSprites.size(); i++)
		_platformSprites[i]->setColor(color);
}

Color scalablePlatform::GetColor()
{
	return _platformSprites[0]->getColor();
}

void scalablePlatform::SetTexture(Texture * texture)
{
	_ladderPlatformTexture = texture;
	for (int i = 0; i < _platformSprites.size(); i++)
		_platformSprites[i]->setTexture(*_ladderPlatformTexture);
}

void scalablePlatform::SetColorID(int colorID)
{
	_colorID = colorID >= 0 ? colorID : 0;
	_colorID = colorID <= _maxColorID ? colorID : _maxColorID;
	_fixedMiddleTiles = GetMiddleTiles();
	for (int i = 0; i < _nbBlocks; i++)
	{
		Vector2f tileRect = GetIntRect(i);
		_platformSprites[i]->setTextureRect(IntRect(tileRect.x, tileRect.y, _tileSize.x, _tileSize.y));
	}
}

void scalablePlatform::Step(float deltaTime)
{
}

void scalablePlatform::Display(RenderWindow * window)
{
	for (int i = 0; i < _platformSprites.size(); i++)
		utils::DrawLoopSprite(window, _platformSprites[i]);
}

string scalablePlatform::GetType()
{
	return string();
}
