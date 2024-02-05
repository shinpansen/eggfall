#ifndef maps_HPP
#define maps_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>
#include <string>
#include <functional>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <Box2D/Box2D.h>
#include "../tools/files.hpp"
#include "../tools/Box2DTools.hpp"
#include "../tools/mapReader.hpp"
#include "../objects/scalablePlatform.hpp"
#include "../objects/ladder.hpp"
#include "../objects/platform.hpp"
#include "../objects/bridge.hpp"
#include "../effects/effects.hpp"
#include "../editor/editor.hpp"
#include "tree.hpp"
#include "grass.hpp"

using namespace std;
using namespace sf;

class editor;

//Thread
void LoadMapThread(string * mapFile,
				   bool * isLoading,
				   b2World* world,
				   editor *	editor,
			       Texture * groundTileTexture,
				   Texture * grassClumpsTexture,
				   Texture * ladderPlatformTexture,
				   vector<b2Body*> * mapBodies,
				   vector<pair<Sprite*, int>> * tiles,
				   vector<pair<Sprite*, int>> * wrappingTiles,
				   vector<grass*> * grassBlocks,
				   vector<pair<scalablePlatform*, int>> * scalablePlatforms);

class maps
{
public:
	//Constructor - Methods
    maps();
    maps(b2World* world, effects* effectBox, input* input);
	bool LoadMap(string mapFile);
	bool IsMapLoading();
	void MoveClouds(float deltaTime);
	string GetContactGroundType(Vector2f pos);
	string GetGroundType(int type);
    void RenderMap(float deltaTime);
	void EditMap(RenderWindow* window, float deltaTime, bool step);
	void Display(RenderWindow* window, float deltaTime, bool playersLoading);

	//Environment properties
	vector<Color> dirtColors;
	vector<Color> rockColors;
	vector<Color> woodAColors;
	vector<Color> woodBColors;

    //Resources
    Texture _treeTexture;
	Texture _groundTileTexture;
	Texture _grassClumpsTexture;
	Texture _ladderPlatformTexture;
	vector<pair<Sprite*, int>> _tiles;
	vector<pair<Sprite*, int>> _wrappingTiles;
	vector<grass*> _grassBlocks;
	vector<pair<scalablePlatform*, int>> _scalablePlatforms;
	vector<bridge*> _bridges;

	//Box2d
	b2World* _world;
	effects* _effectBox;
	vector<b2Body*> _mapBodies;

private:
    //Objects
	input* _input;
	editor* _editor;
	Thread* _threadLoadMap;

    //Variables
	string _mapFile;
	bool _isLoading;
    float _cloudsMove;
    float _cloudsMove2;

    //Graphics
	RectangleShape _rectLoad;
    RectangleShape _backgroundShape;
    Texture _backgroundTexture;
    Texture _cloudsTexture;
    Texture _clouds2Texture;
    Texture _fogTexture;
    Sprite _background;
    Sprite _clouds;
    Sprite _clouds2;
    Sprite _fog;
};
#endif
