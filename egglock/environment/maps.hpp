#ifndef maps_HPP
#define maps_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <Box2D/Box2D.h>
#include "../tools/files.hpp"
#include "../tools/Box2DTools.hpp"
#include "../objects/ladder.hpp"
#include "../effects/effects.hpp"
#include "tree.hpp"

using namespace std;
using namespace sf;

class maps
{
public:
    maps();
    maps(b2World* world, ladder* ladderBox, effects* effectBox);
    void CreateLayers();
    void CreateCollisions();
    void RenderMap();
    void DisplayMap(RenderWindow* window);
    void MoveClouds();

    //Resources
    Texture _layer1;
    Texture _treeTexture;
    vector<tree*> _treeList;
    vector<Sprite*> _layer0Tiles;
    vector<Sprite*> _layer1Tiles;
private:
    //Objects
    b2World* _world;
    files _files;
    ladder* _ladderBox;
    effects* _effectBox;

    //Variables
    float _cloudsMove;
    float _cloudsMove2;

    //Graphics
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
