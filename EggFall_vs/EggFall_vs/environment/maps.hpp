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
#include "../effects/effects.hpp"
#include "../characters/AI.hpp"
#include "walkable/scalablePlatform.hpp"
#include "walkable/ladder.hpp"
#include "walkable/platform.hpp"
#include "walkable/bridge.hpp"
#include "flourish/teeter.hpp"
#include "flourish/grass.hpp"
#include "flourish/tree/firShort.hpp"
#include "flourish/tree/firLong.hpp"
#include "flourish/tree/cutTree.hpp"
#include "flourish/tree/roundTree.hpp"
#include "flourish/tree/oak.hpp"
#include "flourish/tree/shrub.hpp"
#include "flourish/tree/shrubSmall.hpp"
#include "flourish/tree/sakura.hpp"
#include "flourish/lamp/conicalLampA.hpp"
#include "flourish/lamp/conicalLampB.hpp"
#include "flourish/lamp/conicalLampC.hpp"
#include "flourish/lamp/pipeLamp.hpp"
#include "flourish/lamp/hatLamp.hpp"
#include "../editor/editor.hpp"

using namespace std;
using namespace sf;

class AI;
class editor;

//Thread
void LoadMapThread(string * mapFile,
	bool * isLoading,
	b2World* world,
	editor * editor,
	Texture * groundTileTexture,
	Texture * grassClumpsTexture,
	Texture * ladderPlatformTexture,
	vector<b2Body*> * mapBodies,
	vector<pair<Sprite*, int>> * tiles,
	vector<pair<Sprite*, int>> * wrappingTiles,
	vector<grass*> * grassBlocks,
	vector<pair<scalablePlatform*, int>> * scalablePlatforms,
	vector<bridge*> * bridges,
	vector<AI*> * AIs);

class maps
{
public:
	//Constructor - Methods
    maps();
    maps(b2World* world, effects* effectBox, input* input, vector<AI*> * AIs);
	bool LoadMap(string mapFile);
	bool IsMapLoading();
	void ShowBackgroundEffect(bool show);
	void AnimateNature(float deltaTime);
	string GetContactGroundType(Vector2f pos);
	string GetGroundType(int type);
	editor * GetEditor();
    void RenderMap(float deltaTime);
	void EditMap(RenderWindow* window, float deltaTime, bool step);
	void Display(RenderWindow* window, float deltaTime, bool playersLoading);

	//Environment properties
	vector<Color> dirtAColors;
	vector<Color> dirtBColors;
	vector<Color> rockAColors;
	vector<Color> rockBColors;
	vector<Color> woodAColors;
	vector<Color> woodBColors;

    //Resources
	Texture _groundTileTexture;
	Texture _grassClumpsTexture;
	Texture _ladderPlatformTexture;
	Texture _treeTexture;
	Texture _lampTexture;
	Texture _lampOffTexture;
	vector<pair<Sprite*, int>> _tiles;
	vector<pair<Sprite*, int>> _wrappingTiles;
	vector<grass*> _grassBlocks;
	vector<pair<scalablePlatform*, int>> _scalablePlatforms;
	vector<bridge*> _bridges;

	//Box2d
	b2World * _world;
	effects * _effectBox;
	vector<b2Body*> _mapBodies;

private:
	//Variables
	string _mapFile;
	bool _isLoading;
	bool _showBackgroundEffectShape;
	float _cloudsMove;
	float _cloudsMove2;

    //Objects
	input * _input;
	editor * _editor;
	Thread * _threadLoadMap;
	vector<AI*> * _AIs;

    //Graphics
	RectangleShape _rectLoad;
	RectangleShape _backgroundShape;
	RectangleShape _backgroundEffectShape;
    Texture _backgroundTexture;
    Texture _cloudsTexture;
    Texture _clouds2Texture;
    Texture _fogTexture;
    Sprite _background;
    Sprite _clouds;
    Sprite _clouds2;
    Sprite _fog;

	//TEST
	Texture textureUsineTest;
	Sprite usineTest;

	vector<teeter*> teetersTest;
};
#endif
