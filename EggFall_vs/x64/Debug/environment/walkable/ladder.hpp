#ifndef ladder_HPP
#define ladder_HPP

#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <Box2D/Box2D.h>
#include "scalablePlatform.hpp"

using namespace std;
using namespace sf;

class ladder : public scalablePlatform
{
public:
	ladder(
		b2World* world, 
		Texture* texture, 
		Vector2f pos, 
		int nbBlocks, 
		int colorID, 
		int * middleTiles = NULL, 
		bool disablePhysics = false) :
	scalablePlatform(
		world, 
		texture, 
		pos, 
		24,
		nbBlocks, 
		1, 
		colorID, 
		Box2DTools::LADDER_ENTITY, 
		IntRect(0, 0, 32, 32), 
		IntRect(0, 160, 32, 32), 
		PLATFORM_VERTICAL, 
		boxResize(6, 6, 36, 36), 
		middleTiles, 
		disablePhysics)
	{
	};

	using scalablePlatform::GetType;
	string GetType()
	{
		return "ladder";
	}
};
#endif
