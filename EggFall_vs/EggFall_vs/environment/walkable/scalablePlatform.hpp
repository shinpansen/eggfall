#ifndef scalablePlatform_HPP
#define scalablePlatform_HPP

#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <Box2D/Box2D.h>
#include "../../tools/utils.hpp"
#include "../../tools/Box2DTools.hpp"

enum PLATFORM_ALIGN
{
	PLATFORM_HORIZONTAL,
	PLATFORM_VERTICAL
};

using namespace std;
using namespace sf;

class boxResize
{
public:
	boxResize();
	boxResize(float left, float right, float top, float bottom);
	float left;
	float right;
	float top;
	float bottom;
	bool enabled;
};

class scalablePlatform
{
public:
	~scalablePlatform();
	scalablePlatform(b2World* world, Texture* texture, Vector2f pos, int maxNbBlocks, int nbBlocks, int maxColorID, int colorID, 
					 short bodyEntity, IntRect firstRectTile, IntRect lastRectTile, PLATFORM_ALIGN align, boxResize bodyResize = boxResize(),
					 int * fixedMiddleTiles = NULL, bool disablePhysics = false);
	int GetMaxColorID();
	int GetColorID();
	int GetMaxNbBlocks();
	int GetNbBlocks();
	int * GetMiddleTiles();
	string GetMiddleTilesString();
	Vector2f GetPosition();
	Vector2f GetIntRect(int i);
	vector<FloatRect> GetGlobalBounds(bool grabBounds = false);
	void SetPosition(Vector2f pos, bool disablePhysics = false);
	void Move(float x, float y, bool disablePhysics = false);
	void SetColor(Color color);
	Color GetColor();
	void SetTexture(Texture* texture);
	void SetColorID(int colorID);
	void Step(float deltaTime);
	void Display(RenderWindow* window);
	virtual string GetType();
protected:
	//Variables
	Vector2f _pos;
	Vector2f _tileSize;
	int _maxNbBlocks;
	int _nbBlocks;
	int _maxColorID;
	int _colorID;
	short _bodyEntity;
	IntRect _firstRectTile;
	IntRect _lastRectTile;
	PLATFORM_ALIGN _align;
	boxResize _bodyResize;
	int * _fixedMiddleTiles;

	//Objects
	b2World* _world;
	vector<b2Body*> _platformBodies;
	vector<Sprite*> _platformSprites;
	Texture* _ladderPlatformTexture;
};
#endif
