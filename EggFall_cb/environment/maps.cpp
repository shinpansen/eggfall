#include "maps.hpp"

using namespace std;
using namespace sf;

static const float SCALE = 30.f;

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
	vector<pair<scalablePlatform*, int>> * scalablePlatforms)
{
	//DELETE
	//Delete ground sprites
	for (int i = 0; i < tiles->size(); i++)
		if (((*tiles)[i]).first != NULL)
			delete ((*tiles)[i]).first;
	tiles->clear();
	for (int i = 0; i < wrappingTiles->size(); i++)
		if (((*wrappingTiles)[i]).first != NULL)
			delete ((*wrappingTiles)[i]).first;
	wrappingTiles->clear();

	//Deleting grass
	for (int i = 0; i < grassBlocks->size(); i++)
		if ((*grassBlocks)[i] != NULL)
			delete (*grassBlocks)[i];
	grassBlocks->clear();

	//Deleting scalable platforms
	for (int i = 0; i < scalablePlatforms->size(); i++)
		if (((*scalablePlatforms)[i]).first != NULL)
			delete ((*scalablePlatforms)[i]).first;
	scalablePlatforms->clear();

	//Delete collisions
	for (int i = 0; i<mapBodies->size(); i++)
		if ((*mapBodies)[i] != NULL)
			world->DestroyBody((*mapBodies)[i]);
	mapBodies->clear();

	//LOAD
	TiXmlDocument doc((*mapFile).c_str());
	doc.LoadFile();
	TiXmlElement* root = doc.FirstChildElement();
	for (TiXmlElement* root = doc.FirstChildElement(); root != NULL; root = root->NextSiblingElement())
	{
		cout << "Loading [" << root->Value() << "] ..." << endl;
		if ((string)(root->Value()) == "ground")
		{
			for (TiXmlElement* elem = root->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement())
			{
				if ((string)(elem->Value()) == "tile")
				{
					//POCO
					groundTileProperties gtp = mapReader::GetGroundTileProperties(elem);

					//Tile
					pair<Sprite*, int> tilePair;
					tilePair.first = new Sprite();
					tilePair.first->setTexture(*groundTileTexture);
					tilePair.first->setPosition(gtp.pos);
					tilePair.first->setTextureRect(gtp.rect);
					tilePair.second = gtp.groundType;
					tiles->push_back(tilePair);

					//Collision
					if (!gtp.surrounded)
					{
					    int voidUserData = UD_VOID;
					    int hookUserData = UD_HOOK;
						Vector2f pos = Vector2f(gtp.pos.x + gtp.rect.width / 2, gtp.pos.y + gtp.rect.height / 2);
						mapBodies->push_back(Box2DTools::CreateRectangleBox(world, pos, gtp.rect.width, gtp.rect.height, 0.1, 0, 0, Box2DTools::GROUND_ENTITY));
						(*mapBodies)[mapBodies->size() - 1]->SetUserData(gtp.hook ? (void*)hookUserData : (void*)voidUserData);

						//Manage loop issues
						if (gtp.pos.y >= 0 && gtp.pos.y <= utils::VIEW_HEIGHT - gtp.rect.width)
						{
							//Sides
							if (gtp.pos.x == 0)
								mapBodies->push_back(Box2DTools::CreateRectangleBox(world, Vector2f(pos.x + utils::VIEW_WIDTH, pos.y), gtp.rect.width, gtp.rect.height, 0.1, 0, 0, Box2DTools::GROUND_ENTITY));
							else if (gtp.pos.x + gtp.rect.width >= utils::VIEW_WIDTH)
								mapBodies->push_back(Box2DTools::CreateRectangleBox(world, Vector2f(-gtp.rect.width / 2, pos.y), gtp.rect.width, gtp.rect.height, 0.1, 0, 0, Box2DTools::GROUND_ENTITY));
							(*mapBodies)[mapBodies->size() - 1]->SetUserData(gtp.hook ? (void*)hookUserData : (void*)voidUserData);
						}
						if (gtp.pos.x >= 0 && gtp.pos.x <= utils::VIEW_WIDTH - gtp.rect.width)
						{
							//Top - down
							if (gtp.pos.y <= 16)
								mapBodies->push_back(Box2DTools::CreateRectangleBox(world, Vector2f(pos.x, pos.y + utils::VIEW_HEIGHT), gtp.rect.width, gtp.rect.height, 0.1, 0, 0, Box2DTools::GROUND_ENTITY));
							else if (gtp.pos.y + gtp.rect.height >= utils::VIEW_HEIGHT)
								mapBodies->push_back(Box2DTools::CreateRectangleBox(world, Vector2f(pos.x, -gtp.rect.height / 2), gtp.rect.width, gtp.rect.height, 0.1, 0, 0, Box2DTools::GROUND_ENTITY));
							(*mapBodies)[mapBodies->size() - 1]->SetUserData(gtp.hook ? (void*)hookUserData : (void*)voidUserData);
						}
					}

					//Adding grass
					if (gtp.grass)
						grassBlocks->push_back(new grass(world, ((*tiles)[tiles->size() - 1]).first, groundTileTexture, grassClumpsTexture, gtp.groundType, true, true, gtp.leftGrass, gtp.rightGrass));
				}
				else if ((string)(elem->Value()) == "wrapping")
				{
					//POCO
					groundTileWRappingProperties gtwp = mapReader::GetGroundTileWrappingProperties(elem);

					//Tile Wrapping
					pair<Sprite*, int> pair;
					pair.first = new Sprite();
					pair.first->setTexture(*groundTileTexture);
					pair.first->setPosition(gtwp.pos);
					pair.first->setTextureRect(gtwp.rect);
					pair.second = gtwp.parent;
					wrappingTiles->push_back(pair);
				}
			}
		}
		else if ((string)(root->Value()) == "platform")
		{
			for (TiXmlElement* elem = root->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement())
			{
				scalablePlatformProperties spp = mapReader::GetscalablePlatformProperties(elem);
				pair<scalablePlatform*, PLATFORM_TYPE> platformPair;
				if ((string)(elem->Value()) == "ladder")
				{
					ladder * newLadder = new ladder(world, ladderPlatformTexture, spp.pos, spp.nbBlocks, spp.colorID, spp.fixedMiddleTiles);
					platformPair.first = newLadder;
					platformPair.second = PLATFORM_LADDER;
				}
				else if ((string)(elem->Value()) == "platform")
				{
					platform * newPlatform = new platform(world, ladderPlatformTexture, spp.pos, spp.nbBlocks, spp.colorID, spp.fixedMiddleTiles);
					platformPair.first = newPlatform;
					platformPair.second = PLATFORM_A;
				}
				scalablePlatforms->push_back(platformPair);
			}
		}
	}

	//Update objetc references
	editor->ReloadModules();
	*isLoading = false;
}

maps::maps()
{
}

maps::maps(b2World* world, effects* effectBox, input* input)
{
    //Variables - objects
    _world = world;
    _effectBox = effectBox;
	_input = input;
	_editor = new editor(this, input, effectBox);
	_isLoading = false;
    _cloudsMove = 0;
    _cloudsMove2 = 0;

    //Resources
	if (!_groundTileTexture.loadFromFile(files::groundTile))
	{
		cout << "Unable to find groundTile texture. Stopping execution." << endl;
		exit(1);
	}
	if (!_grassClumpsTexture.loadFromFile(files::grassClumps))
	{
		cout << "Unable to find grass clumps texture. Stopping execution." << endl;
		exit(1);
	}
    if(!_treeTexture.loadFromFile(files::trees))
    {
        cout << "Unable to find tree texture. Stopping execution." << endl;
        exit(1);
    }
	if (!_ladderPlatformTexture.loadFromFile(files::ladderPlatform))
	{
		cout << "Unable to find miscellaneous texture. Stopping execution." << endl;
		exit(1);
	}

	//Thread
	_threadLoadMap = new Thread(bind(&LoadMapThread, &_mapFile, &_isLoading, _world, _editor, &_groundTileTexture, &_grassClumpsTexture,
		&_ladderPlatformTexture, &_mapBodies, &_tiles, &_wrappingTiles, &_grassBlocks, &_scalablePlatforms));

    //Graphics
    _backgroundShape.setSize(Vector2f(utils::VIEW_WIDTH,utils::VIEW_HEIGHT));
    _backgroundShape.setFillColor(Color(50,0,0,60));
    _backgroundShape.setPosition(0,0);

	//Environment properties
	dirtColors.push_back(Color(153, 89, 46));
	dirtColors.push_back(Color(115, 52, 46));
	rockColors.push_back(Color(193, 182, 162));
	rockColors.push_back(Color(161, 136, 124));
	rockColors.push_back(Color(91, 72, 70));
	woodAColors.push_back(Color(190, 120, 57));
	woodAColors.push_back(Color(117, 67, 34));
	woodAColors.push_back(Color(79, 54, 39));
	woodBColors.push_back(Color(163, 132, 93));
	woodBColors.push_back(Color(128, 82, 55));
	woodBColors.push_back(Color(73, 49, 37));

    //Background
    _backgroundTexture.loadFromFile(files::backgroundC);
    _cloudsTexture.loadFromFile(files::cloudsA);
    _clouds2Texture.loadFromFile(files::cloudsB);
    _fogTexture.loadFromFile(files::fog);
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
    _fog.setTextureRect(IntRect(0,0,utils::VIEW_WIDTH, utils::VIEW_HEIGHT));
    _fog.setColor(Color(255,255,255,42));

	_bridges.push_back(new bridge(_world, &_ladderPlatformTexture, 0, Vector2f(240, 96), Vector2f(672, 96)));

	_rectLoad.setFillColor(Color::White);
	_rectLoad.setSize(Vector2f(40, 40));
	_rectLoad.setOrigin(Vector2f(20, 20));
	_rectLoad.setPosition(Vector2f(1220, 660));
}

bool maps::LoadMap(string mapFile)
{
	//Reading file
	if (!files::FileExists(mapFile))
	{
		cout << "File '" << mapFile << "' not found" << endl;;
		return false;
	}
	else
	{
		_isLoading = true;
		_mapFile = mapFile;
		_threadLoadMap->launch();
		return true;
	}
}

bool maps::IsMapLoading()
{
	return _isLoading;
}

void maps::RenderMap(float deltaTime)
{
	//Waiting for thread loading map
	if (_isLoading)
		return;

    //Trees teeter
    /*for(int i=0 ; i<_treeList.size() ; i++)
        _treeList[i]->Step(deltaTime);*/
	for (int i = 0; i < _grassBlocks.size(); i++)
		if(_grassBlocks[i] != NULL)
			_grassBlocks[i]->Step(deltaTime);
	for (int i = 0; i < _scalablePlatforms.size(); i++)
		if (_scalablePlatforms[i].first != NULL)
			_scalablePlatforms[i].first->Step(deltaTime);
    for (int i = 0; i < _bridges.size(); i++)
		if(_bridges[i] != NULL)
			_bridges[i]->Step(deltaTime);
}

void maps::MoveClouds(float deltaTime)
{
	//Step
	if (_cloudsMove < 1)
		_cloudsMove += 0.2*deltaTime;
	else
		_cloudsMove = 0;
	if (_cloudsMove2 < 1)
		_cloudsMove2 += 0.1*deltaTime;
	else
		_cloudsMove2 = 0;

	//TextureRect
	if (_clouds.getTextureRect().left < 800)
		_clouds.setTextureRect(IntRect(_clouds.getTextureRect().left + _cloudsMove, 0, 800, 212));
	else
		_clouds.setTextureRect(IntRect(0, 0, 800, 212));

	if (_clouds2.getTextureRect().left < 800)
		_clouds2.setTextureRect(IntRect(_clouds2.getTextureRect().left + _cloudsMove2, 0, 800, 212));
	else
		_clouds2.setTextureRect(IntRect(0, 0, 800, 212));

	if (_fog.getTextureRect().left < utils::VIEW_WIDTH)
		_fog.setTextureRect(IntRect(_fog.getTextureRect().left + _cloudsMove2, 0, utils::VIEW_WIDTH, utils::VIEW_HEIGHT));
	else
		_fog.setTextureRect(IntRect(0, 0, utils::VIEW_WIDTH, utils::VIEW_HEIGHT));
}

string maps::GetContactGroundType(Vector2f pos)
{
	//Testing ground tiles
	for (int i = 0; i < _tiles.size(); i++)
	{
		if (_tiles[i].first != NULL)
		{
			//Increasing tile bound - Contact position is usually not exactly on the ground tile
			FloatRect groundRect = _tiles[i].first->getGlobalBounds();
			groundRect = FloatRect(groundRect.left - 8, groundRect.top - 8, groundRect.width + 16, groundRect.height + 16);
			if (groundRect.contains(pos))
				return GetGroundType((int)(_tiles[i].first->getTextureRect().top / 160.f));
		}
	}

	//Testing bridges
	for (int i = 0; i < _bridges.size(); i++)
	{
		if (_bridges[i] != NULL && _bridges[i]->GetGlobalBounds().contains(pos))
		{
			_bridges[i]->BreakBridgeBlocks(pos);
			return _bridges[i]->GetType();
		}
	}

	//Testing platforms
	for (int i = 0; i < _scalablePlatforms.size(); i++)
	{
		if (_scalablePlatforms[i].first != NULL && _scalablePlatforms[i].first->GetType() != "ladder")
		{
			vector<FloatRect> platformBounds = _scalablePlatforms[i].first->GetGlobalBounds(true);
			for (int j = 0; j < platformBounds.size(); j++)
				if (platformBounds[j].contains(pos))
					return _scalablePlatforms[i].first->GetType();
		}
	}

	//Nothing
	return "";
}

string maps::GetGroundType(int type)
{
	switch (type)
	{
		case 0:case 1:
			return "dirt";
		case 2:case 3:
			return "rock";
		default:
			return "";
	}
}



void maps::EditMap(RenderWindow * window, float deltaTime, bool step)
{
	//Waiting for thread loading map
	if (_isLoading)
		return;

	if (step)
		_editor->Step(deltaTime);
	_editor->Display(window);
}

void maps::Display(RenderWindow* window, float deltaTime, bool playersLoading)
{
	//Waiting for thread loading map
	if (_isLoading || playersLoading)
	{
		window->draw(_rectLoad);
		_rectLoad.setRotation(utils::StepRotation(_rectLoad.getRotation(), 5.f*deltaTime));
	}
	else
	{
		//Background
		/*MoveClouds(deltaTime);
		window->draw(_clouds2);
		window->draw(_clouds);*/
		window->draw(_background);
		window->draw(_fog);

		//Drawing ground tiles + grass
		for (int i = 0; i < _grassBlocks.size(); i++)
			if (_grassBlocks[i] != NULL)
				_grassBlocks[i]->DisplayGrassClumps(window);
		for (int i = 0; i < _grassBlocks.size(); i++)
			if (_grassBlocks[i] != NULL)
				_grassBlocks[i]->DisplayGrassClumps(window);
		for (int i = 0; i < _tiles.size(); i++)
			if (_tiles[i].first != NULL)
				window->draw(*_tiles[i].first);
		for (int i = 0; i < _wrappingTiles.size(); i++)
			if (_wrappingTiles[i].first != NULL)
				utils::DrawLoopSprite(window, _wrappingTiles[i].first);
		for (int i = 0; i < _grassBlocks.size(); i++)
			if (_grassBlocks[i] != NULL)
				_grassBlocks[i]->DisplayGrass(window);

		//Drawing bridges
		for (int i = 0; i < _bridges.size(); i++)
			if (_bridges[i] != NULL)
				_bridges[i]->Display(window);

		//Drawing scalable platforms
		for (int i = 0; i < _scalablePlatforms.size(); i++)
			if (_scalablePlatforms[i].first != NULL)
				_scalablePlatforms[i].first->Display(window);
	}

    //Background opacity
    //window->draw(_backgroundShape);
	/*RectangleShape rect = RectangleShape(Vector2f(200, 10));
	rect.setFillColor(Color::Blue);
	rect.setOrigin(100, 5);
	rect.setRotation(20);
	rect.setPosition(Vector2f(110, 110));
	window->draw(rect);*/
}
