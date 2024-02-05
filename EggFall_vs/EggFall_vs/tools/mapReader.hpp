#ifndef mapReader_HPP
#define mapReader_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <SFML\Graphics.hpp>
#include "../tinyXML/tinyxml.h"
#include "../effects/colorScheme.hpp"
#include "files.hpp"
#include "xmlReader.hpp"

using namespace std;
using namespace sf;

class groundTileProperties
{
public:
	IntRect rect;
	Vector2f pos;
	bool hook;
	bool grass;
	bool leftGrass;
	bool rightGrass;
	bool surrounded;
	int groundType;
};

class groundTileWRappingProperties
{
public:
	IntRect rect;
	Vector2f pos;
	int parent;
};

class scalablePlatformProperties
{
public:
	Vector2f pos;
	int nbBlocks;
	int colorID;
	int * fixedMiddleTiles;
};

class bridgeProperties
{
public:
	Vector2f leftPos;
	Vector2f rightPos;
	int colorID;
};

class mapReader
{
public:
	mapReader();
	static groundTileProperties GetGroundTileProperties(TiXmlElement* elem);
	static groundTileWRappingProperties GetGroundTileWrappingProperties(TiXmlElement* elem);
	static scalablePlatformProperties GetScalablePlatformProperties(TiXmlElement* elem);
	static bridgeProperties GetBridgeProperties(TiXmlElement* elem);
};
#endif
