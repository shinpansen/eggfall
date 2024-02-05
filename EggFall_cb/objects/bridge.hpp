#ifndef bridge_HPP
#define bridge_HPP

#include <iostream>
#include <vector>
#include <math.h>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <Box2D/Box2D.h>
#include "../tools/utils.hpp"
#include "../tools/Box2DTools.hpp"

using namespace std;
using namespace sf;

class bridge
{
public:
	~bridge();
	bridge(b2World* world, Texture* texture, int colorID, Vector2f posLeft, Vector2f posRight);
	FloatRect GetGlobalBounds();
	string GetType();
	void BreakBridgeBlocks(Vector2f pos);
	void Step(float deltaTime);
	void Display(RenderWindow* window);
private:
	//Variables
	int _colorID;
	Vector2f _posLeft;
	Vector2f _posRight;

	//Objects
	b2World * _world;
	Sprite _leftBridge;
	Sprite _rightBridge;
	b2Body * _leftBody;
	b2Body * _rightBody;
	vector<b2Body*> _bridgeBodies;
	vector<Sprite*> _bridgeSprites;
	vector<b2Joint*> _revoluteJoints;
	Texture * _ladderPlatformTexture;
};
#endif
