#ifndef platform_HPP
#define platform_HPP

#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <Box2D/Box2D.h>
#include "../tools/utils.hpp"
#include "../tools/Box2DTools.hpp"
#include "../objects/scalablePlatform.hpp"

using namespace std;
using namespace sf;

class platform : public scalablePlatform
{
public:
	platform(
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
		21,
		nbBlocks, 
		1, 
		colorID, 
		Box2DTools::PLATFORM_ENTITY, 
		IntRect(64, 0, 64, 48), 
		IntRect(448, 0, 64, 48),
		PLATFORM_HORIZONTAL, 
		boxResize(56, 56, 8, 32), 
		middleTiles, 
		disablePhysics)
	{
	};

	using scalablePlatform::GetType;
	string GetType()
	{
		if (_colorID == 0)
			return "wood_a";
		else
			return "wood_b";
	}
};
#endif
