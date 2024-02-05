#include "mapReader.hpp"

using namespace std;
using namespace sf;

mapReader::mapReader()
{
}

groundTileProperties mapReader::GetGroundTileProperties(TiXmlElement * elem)
{
	//XML attributes
	int left = elem->Attribute("left") != NULL ? atoi(elem->Attribute("left")) : 0;
	int top = elem->Attribute("top") != NULL ? atoi(elem->Attribute("top")) : 0;
	int width = elem->Attribute("width") != NULL ? atoi(elem->Attribute("width")) : 0;
	int height = elem->Attribute("height") != NULL ? atoi(elem->Attribute("height")) : 0;
	int x = elem->Attribute("x") != NULL ? atoi(elem->Attribute("x")) : 0;
	int y = elem->Attribute("y") != NULL ? atoi(elem->Attribute("y")) : 0;
	int hook = elem->Attribute("hook") != NULL ? atoi(elem->Attribute("hook")) : 0;
	int grass = elem->Attribute("grass") != NULL ? atoi(elem->Attribute("grass")) : 0;
	int leftGrass = elem->Attribute("leftGrass") != NULL ? atoi(elem->Attribute("leftGrass")) : 0;
	int rightGrass = elem->Attribute("rightGrass") != NULL ? atoi(elem->Attribute("rightGrass")) : 0;
	int surrounded = elem->Attribute("surrounded") != NULL ? atoi(elem->Attribute("surrounded")) : 0;

	//Class object
	groundTileProperties gtp;
	gtp.rect = IntRect(left, top, width, height);
	gtp.pos = Vector2f(x, y);
	gtp.hook = hook == 1 ? true : false;
	gtp.grass = grass == 1 ? true : false;
	gtp.leftGrass = leftGrass == 1 ? true : false;
	gtp.rightGrass = rightGrass == 1 ? true : false;
	gtp.surrounded = surrounded == 1 ? true : false;
	gtp.groundType = elem->Attribute("type") != NULL ? atoi(elem->Attribute("type")) : 0;
	return gtp;
}

groundTileWRappingProperties mapReader::GetGroundTileWrappingProperties(TiXmlElement * elem)
{
	//XML attributes
	int left = elem->Attribute("left") != NULL ? atoi(elem->Attribute("left")) : 0;
	int top = elem->Attribute("top") != NULL ? atoi(elem->Attribute("top")) : 0;
	int width = elem->Attribute("width") != NULL ? atoi(elem->Attribute("width")) : 0;
	int height = elem->Attribute("height") != NULL ? atoi(elem->Attribute("height")) : 0;
	int x = elem->Attribute("x") != NULL ? atoi(elem->Attribute("x")) : 0;
	int y = elem->Attribute("y") != NULL ? atoi(elem->Attribute("y")) : 0;

	//Class object
	groundTileWRappingProperties gtwp;
	gtwp.rect = IntRect(left, top, width, height);
	gtwp.pos = Vector2f(x, y);
	gtwp.parent = elem->Attribute("parent") != NULL ? atoi(elem->Attribute("parent")) : 0;
	return gtwp;
}

scalablePlatformProperties mapReader::GetScalablePlatformProperties(TiXmlElement * elem)
{
	//XML attributes
	int x = elem->Attribute("x") != NULL ? atoi(elem->Attribute("x")) : 0;
	int y = elem->Attribute("y") != NULL ? atoi(elem->Attribute("y")) : 0;
	int nbBlocks = elem->Attribute("blocks") != NULL ? atoi(elem->Attribute("blocks")) : 3;
	int colorID = elem->Attribute("colorID") != NULL ? atoi(elem->Attribute("colorID")) : 0;

	//Middle tiles
	int * middleTiles = NULL;
	if (elem->Attribute("tiles") != NULL)
	{
		vector<float> middleTilesList = files::ReadLine((string)elem->Attribute("tiles"));
		middleTiles = new int[middleTilesList.size()];
		for (int i = 0; i < middleTilesList.size(); i++)
			middleTiles[i] = middleTilesList[i];
	}

	//Class object
	scalablePlatformProperties spp;
	spp.pos = Vector2f(x, y);
	spp.nbBlocks = nbBlocks;
	spp.colorID = colorID;
	spp.fixedMiddleTiles = middleTiles;
	return spp;
}

bridgeProperties mapReader::GetBridgeProperties(TiXmlElement * elem)
{
	//XML attributes
	int leftX = elem->Attribute("left_x") != NULL ? atoi(elem->Attribute("left_x")) : 0;
	int leftY = elem->Attribute("left_y") != NULL ? atoi(elem->Attribute("left_y")) : 0;
	int rightX = elem->Attribute("right_x") != NULL ? atoi(elem->Attribute("right_x")) : 0;
	int rightY = elem->Attribute("right_y") != NULL ? atoi(elem->Attribute("right_y")) : 0;
	int colorID = elem->Attribute("colorID") != NULL ? atoi(elem->Attribute("colorID")) : 0;

	//Class object
	bridgeProperties bp;
	bp.leftPos = Vector2f(leftX, leftY);
	bp.rightPos = Vector2f(rightX, rightY);
	bp.colorID = colorID;
	return bp;
}
