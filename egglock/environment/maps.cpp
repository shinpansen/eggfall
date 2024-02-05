#include "maps.hpp"

using namespace std;
using namespace sf;

maps::maps()
{
}

maps::maps(b2World* world, ladder* ladderBox, effects* effectBox)
{
    _world = world;
    _ladderBox = ladderBox;
    _effectBox = effectBox;
    _cloudsMove = 0;
    _cloudsMove2 = 0;

    //Resources
    if (!_layer1.loadFromFile(_files.layer1))
    {
        cout << "Unable to find map texture. Stopping execution." << endl;
        exit(1);
    }
    if(!_treeTexture.loadFromFile(_files.trees))
    {
        cout << "Unable to find tree texture. Stopping execution." << endl;
        exit(1);
    }

    CreateLayers();

    //Graphics
    _backgroundShape.setSize(Vector2f(800,600));
    _backgroundShape.setFillColor(Color(50,0,0,60));
    _backgroundShape.setPosition(0,0);

    //Background
    _backgroundTexture.loadFromFile("resources//backgrounds//mountains_sun3.png");
    _cloudsTexture.loadFromFile("resources//backgrounds//clouds.png");
    _clouds2Texture.loadFromFile("resources//backgrounds//clouds2.png");
    _fogTexture.loadFromFile("resources//backgrounds//fog.png");
    _cloudsTexture.setRepeated(true);
    _clouds2Texture.setRepeated(true);
    _background.setTexture(_backgroundTexture);
    _clouds.setTexture(_cloudsTexture);
    _clouds2.setTexture(_clouds2Texture);
    _fog.setTexture(_fogTexture);
    _clouds.setPosition(0,50);
    _clouds2.setPosition(0,50);
    _clouds.setTextureRect(IntRect(0,0,800,212));
    _clouds2.setTextureRect(IntRect(0,0,800,212));
    _fog.setTextureRect(IntRect(0,0,800,600));
    _fog.setColor(Color(255,255,255,100));

    //Collisions - ladders
    CreateCollisions();
}

void maps::CreateLayers()
{
    try
    {
        //Layer 0
        string line;
        ifstream layerMap0(_files.layer0File.c_str(), ios::in);
        while(getline(layerMap0, line))
        {
            vector<float> param = files::readLine(line);
            _layer0Tiles.push_back(new Sprite());
            int i = _layer0Tiles.size() - 1;
            _layer0Tiles[i]->setTexture(_layer1);
            _layer0Tiles[i]->setPosition(param[0],param[1]+2);
            _layer0Tiles[i]->setTextureRect(IntRect(param[2],param[3],param[4],param[5]));
        }
        layerMap0.close();

        //Layer 1
        ifstream layerMap1(_files.layer1File.c_str(), ios::in);
        while(getline(layerMap1, line))
        {
            vector<float> param = files::readLine(line);
            _layer1Tiles.push_back(new Sprite());
            int i = _layer1Tiles.size() - 1;
            _layer1Tiles[i]->setTexture(_layer1);
            _layer1Tiles[i]->setPosition(param[0],param[1]+2);
            _layer1Tiles[i]->setTextureRect(IntRect(param[2],param[3],param[4],param[5]));
        }
        layerMap1.close();

        //Trees
        ifstream layerTrees(_files.treeFile.c_str(), ios::in);
        while(getline(layerTrees, line))
        {
            vector<float> param = files::readLine(line);
            _treeList.push_back(new tree(Vector2f(param[0],param[1]+2), &_treeTexture, param[2], param[3], _world, _effectBox));
        }
        layerTrees.close();
    }
    catch(exception e)
    {
        cout << e.what() << endl;
    }
}

void maps::CreateCollisions()
{
    try
    {
        //Collisions
        string line;
        ifstream collisionFile(_files.collisionFile.c_str(), ios::in);
        while(getline(collisionFile, line))
        {
            vector<float> param = files::readLine(line);
            Box2DTools::CreateRectangleBox(_world, Vector2i(param[0], param[1]), param[2], param[3], 0.1, 0, 0, param[4]);
        }

        //Ladders
        ifstream ladderFile(_files.ladderFile.c_str(), ios::in);
        while(getline(ladderFile, line))
        {
            vector<float> param = files::readLine(line);
            _ladderBox->AddLadder(Vector2f(param[0], param[1]), param[2], param[3]);
        }
    }
    catch(exception e)
    {
        cout << e.what() << endl;
    }
}

void maps::RenderMap()
{
    //Trees teeter
    for(int i=0 ; i<_treeList.size() ; i++)
        _treeList[i]->Step();
}

void maps::MoveClouds()
{
    //Step
    if(_cloudsMove < 1)
        _cloudsMove += 0.2;
    else
        _cloudsMove = 0;
    if(_cloudsMove2 < 1)
        _cloudsMove2 += 0.1;
    else
        _cloudsMove2 = 0;

    //TextureRect
    if(_clouds.getTextureRect().left < 800)
        _clouds.setTextureRect(IntRect(_clouds.getTextureRect().left+_cloudsMove,0,800,212));
    else
        _clouds.setTextureRect(IntRect(0,0,800,212));

    if(_clouds2.getTextureRect().left < 800)
        _clouds2.setTextureRect(IntRect(_clouds2.getTextureRect().left+_cloudsMove2,0,800,212));
    else
        _clouds2.setTextureRect(IntRect(0,0,800,212));

    if(_fog.getTextureRect().left < 800)
        _fog.setTextureRect(IntRect(_fog.getTextureRect().left+_cloudsMove2,0,800,600));
    else
        _fog.setTextureRect(IntRect(0,0,800,600));
}

void maps::DisplayMap(RenderWindow* window)
{
    //Background
    MoveClouds();
    window->draw(_background);
    window->draw(_clouds2);
    window->draw(_clouds);
    window->draw(_fog);

    //Layer 0
    for(int i=0 ; i<_layer0Tiles.size() ; i++)
        window->draw(*_layer0Tiles[i]);

    //Background opacity
    //window->draw(_backgroundShape);

    //Trees
    for(int i=0 ; i<_treeList.size() ; i++)
        _treeList[i]->DrawTree(window);

    //Layer 1
    for(int i=0 ; i<_layer1Tiles.size() ; i++)
        window->draw(*_layer1Tiles[i]);
}
