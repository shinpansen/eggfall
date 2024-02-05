#ifndef grass_HPP
#define grass_HPP

#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <Box2D\Box2D.h>
#include "../tools/Box2DTools.hpp"
#include "../tools/utils.hpp"
#include "../tools/files.hpp"
#include "../effects/colorScheme.hpp"

using namespace std;
using namespace sf;

class grass
{
public:
	grass(b2World* world, Sprite* tile, Texture * grassTexture, Texture * clumpsTexture, int groundType, bool addGrass = true, 
		bool addClumps = true, bool leftGrass = false, bool rightGrass = false);
	void Step(float deltaTime);
	void DisplayGrass(RenderWindow * window);
	void DisplayGrassClumps(RenderWindow * window);
private:
	//Variables
	bool _drawGrassClumps;
	bool _drawLeftGrass;
	bool _drawRightGrass;
	float _angle;
	float _angleToReach;

	//objects
	b2World* _world;
	Sprite* _tile;
	Sprite _grass;
	Sprite _leftGrass;
	Sprite _rightGrass;
	Sprite _grassClumps;
	Texture * _groundTexture;
	Texture * _clumpsTexture;
};
#endif
