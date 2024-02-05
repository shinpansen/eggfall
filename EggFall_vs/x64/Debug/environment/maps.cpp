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
	vector<pair<scalablePlatform*, int>> * scalablePlatforms,
	vector<bridge*> * bridges,
	vector<AI*> * AIs)
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

	//Deleting bridges
	for (int i = 0; i < bridges->size(); i++)
		if ((*bridges)[i] != NULL)
			delete (*bridges)[i];
	bridges->clear();

	//Delete collisions
	for (int i = 0; i<mapBodies->size(); i++)
		if ((*mapBodies)[i] != NULL)
			world->DestroyBody((*mapBodies)[i]);
	mapBodies->clear();

	//AI obstacles lists
	vector<FloatRect> obstaclesAI;
	vector<Vector2f> listPointsToReachAI;

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
					{
						grassBlocks->push_back(new grass(world, ((*tiles)[tiles->size() - 1]).first, groundTileTexture, grassClumpsTexture, gtp.groundType, true, true, gtp.leftGrass, gtp.rightGrass));
						listPointsToReachAI.push_back(Vector2f(gtp.pos.x + 16.f, gtp.pos.y - 16.f));
					}
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
				scalablePlatformProperties spp = mapReader::GetScalablePlatformProperties(elem);
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
					for(int i=0 ; i<(spp.nbBlocks - 3) * 2 + 1 ; i++)
						listPointsToReachAI.push_back(Vector2f(spp.pos.x - (((spp.nbBlocks - 3) * 2 + 1) * 32.f) / 2.f + i * 32.f + 16.f, spp.pos.y - 32.f));
				}
				scalablePlatforms->push_back(platformPair);
			}
		}
		else if ((string)(root->Value()) == "bridge")
		{
			for (TiXmlElement* elem = root->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement())
			{
				bridgeProperties spp = mapReader::GetBridgeProperties(elem);
				if(spp.leftPos != spp.rightPos && fabs(spp.leftPos.x - spp.rightPos.x) > 80.f &&
					fabs(spp.leftPos.x - spp.rightPos.x)*0.8 > fabs(spp.leftPos.y - spp.rightPos.y))
					bridges->push_back(new bridge(world, ladderPlatformTexture, spp.colorID, spp.leftPos, spp.rightPos));
			}
		}
	}

	//Update obstacles list (AI)
	b2Transform defaultTransform(b2Vec2(0, 0), b2Rot(0));
	for (b2Body* body = world->GetBodyList(); body != 0; body = body->GetNext())
	{
		//Obstacles = ground or platforms
		if (body == NULL || !body->IsActive() ||
			(body->GetFixtureList()->GetFilterData().categoryBits != Box2DTools::GROUND_ENTITY &&
				body->GetFixtureList()->GetFilterData().categoryBits != Box2DTools::PLATFORM_ENTITY))
			continue;

		//Compute body hitbox size
		b2AABB aabb;
		for (b2Fixture* fixture = body->GetFixtureList(); fixture; fixture = fixture->GetNext())
		{
			aabb = fixture->GetAABB(0);
			fixture->GetShape()->ComputeAABB(&aabb, defaultTransform, 0);
		}
		float width = (aabb.upperBound.x*SCALE) - (aabb.lowerBound.x*SCALE);
		float height = (aabb.upperBound.y*SCALE) - (aabb.lowerBound.y*SCALE);

		//Add Float Rect
		float posX = body->GetPosition().x * SCALE;
		float posY = body->GetPosition().y * SCALE;
		if (height < 32)
			obstaclesAI.push_back(FloatRect(posX - width / 2.f, posY - height, width, height * 2));
		else
			obstaclesAI.push_back(FloatRect(posX - width / 2.f, posY - height / 2.f, width, height));
	}

	//Load AIs obstacles
	for (int i = 0; i < AIs->size(); i++)
		if ((*AIs)[i] != NULL)
			(*AIs)[i]->LoadObstaclesList(world, obstaclesAI, listPointsToReachAI);

	//Update objetc references
	editor->ReloadModules();
	*isLoading = false;
}

maps::maps()
{
}

maps::maps(b2World* world, effects* effectBox, input* input, vector<AI*> * AIs)
{
    //Variables - objects
    _world = world;
    _effectBox = effectBox;
	_input = input;
	_editor = new editor(this, input, effectBox);
	_isLoading = false;
    _cloudsMove = 0.f;
    _cloudsMove2 = 0.f;
	_AIs = AIs;

    //Resources
	!utils::LoadTextureSafely(files::groundTile, &_groundTileTexture);
	!utils::LoadTextureSafely(files::grassClumps, &_grassClumpsTexture);
	!utils::LoadTextureSafely(files::trees, &_treeTexture);
	!utils::LoadTextureSafely(files::ladderPlatform, &_ladderPlatformTexture);
	if (utils::LoadTextureSafely(files::streetLamp, &_lampTexture))
	{
		//Texture with lamps off (colorizing yellow into transparency white)
		vector<Color> sourceColors, targetColors;
		sourceColors.push_back(Color(231, 215, 34));
		sourceColors.push_back(Color(254, 254, 158));
		sourceColors.push_back(Color(195, 165, 23));
		targetColors.push_back(Color(255, 255, 255, 80));
		targetColors.push_back(Color(255, 255, 255, 120));
		targetColors.push_back(Color(255, 255, 255, 160));
		_lampOffTexture.loadFromImage(utils::GetImageReplaceColors(files::streetLamp, sourceColors, targetColors));
	}
	else
		utils::LoadTextureSafely(files::streetLamp, &_lampOffTexture);

	//Thread
	_threadLoadMap = new Thread(bind(&LoadMapThread, &_mapFile, &_isLoading, _world, _editor, &_groundTileTexture, &_grassClumpsTexture,
		&_ladderPlatformTexture, &_mapBodies, &_tiles, &_wrappingTiles, &_grassBlocks, &_scalablePlatforms, &_bridges, _AIs));

    //Graphics
    _backgroundShape.setSize(Vector2f(utils::VIEW_WIDTH,utils::VIEW_HEIGHT));
    _backgroundShape.setFillColor(Color(255, 255, 255, 20));
	_backgroundEffectShape.setSize(Vector2f(utils::VIEW_WIDTH, utils::VIEW_HEIGHT));
	_backgroundEffectShape.setFillColor(Color(0, 0, 0, 100));

	//Environment properties
	dirtAColors.push_back(Color(170, 85, 28));
	dirtAColors.push_back(Color(126, 42, 34));
	dirtBColors.push_back(Color(120, 78, 51));
	dirtBColors.push_back(Color(88, 47, 42));
	rockAColors.push_back(Color(193, 182, 162));
	rockAColors.push_back(Color(161, 136, 124));
	rockAColors.push_back(Color(91, 72, 70));
	rockBColors.push_back(Color(208, 201, 146));
	rockBColors.push_back(Color(101, 73, 59));
	rockBColors.push_back(Color(179, 147, 105));
	woodAColors.push_back(Color(190, 120, 57));
	woodAColors.push_back(Color(117, 67, 34));
	woodAColors.push_back(Color(79, 54, 39));
	woodBColors.push_back(Color(163, 132, 93));
	woodBColors.push_back(Color(128, 82, 55));
	woodBColors.push_back(Color(73, 49, 37));

    //Background
    _backgroundTexture.loadFromFile(files::backgroundA);
    _cloudsTexture.loadFromFile(files::cloudsA);
    _clouds2Texture.loadFromFile(files::cloudsB);
    _fogTexture.loadFromFile(files::fog);
    _cloudsTexture.setRepeated(true);
    _clouds2Texture.setRepeated(true);
    _background.setTexture(_backgroundTexture);
    _clouds.setTexture(_cloudsTexture);
    _clouds2.setTexture(_clouds2Texture);
    _fog.setTexture(_fogTexture);
    _clouds.setPosition(0,20);
    _clouds2.setPosition(0,35);
    _clouds.setTextureRect(IntRect(0, 0, utils::VIEW_WIDTH, 212));
    _clouds2.setTextureRect(IntRect(0, 0, utils::VIEW_WIDTH, 212));
	_clouds.setColor(Color(255, 255, 255, 70));
	_clouds2.setColor(Color(255, 255, 255, 70));
    _fog.setTextureRect(IntRect(0,0,utils::VIEW_WIDTH, utils::VIEW_HEIGHT));
    _fog.setColor(Color(255, 255, 255, 42));

	_rectLoad.setFillColor(Color::White);
	_rectLoad.setSize(Vector2f(40, 40));
	_rectLoad.setOrigin(Vector2f(20, 20));
	_rectLoad.setPosition(Vector2f(1220, 660));

	//TEST
	textureUsineTest.loadFromFile("resources//protos//usine.png");
	usineTest.setTexture(textureUsineTest);
	usineTest.setOrigin(0, usineTest.getLocalBounds().height);
	usineTest.setPosition(300, 444);

	teetersTest.push_back(new firShort(_world, _effectBox, &_treeTexture, Vector2f(300, 438), Vector2f(0, 0), 0));
	teetersTest.push_back(new firLong(_world, _effectBox, &_treeTexture, Vector2f(400, 438), Vector2f(0, 0), 0));
	teetersTest.push_back(new cutTree(_world, _effectBox, &_treeTexture, Vector2f(500, 438), Vector2f(0, 0), 0));
	//teetersTest.push_back(new roundTree(_world, _effectBox, &_treeTexture, Vector2f(620, 438), Vector2f(0, 0), 0));
	//teetersTest.push_back(new oak(_world, _effectBox, &treeTexture, Vector2f(740, 438), Vector2f(0, 0), 0));
	//teetersTest.push_back(new sakura(_world, _effectBox, &treeTexture, Vector2f(760, 438), Vector2f(0, 0), 0));
	//teetersTest.push_back(new pipeLamp(_world, _effectBox, &_lampTexture, &_lampOffTexture, Vector2f(620, 440), Vector2f(0, 0)));
	teetersTest.push_back(new conicalLampA(_world, _effectBox, &_lampTexture, &_lampOffTexture, Vector2f(620, 440), Vector2f(0, 0)));
	teetersTest.push_back(new hatLamp(_world, _effectBox, &_lampTexture, &_lampOffTexture, Vector2f(780, 440), Vector2f(0, 0)));
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

void maps::ShowBackgroundEffect(bool show)
{
	_showBackgroundEffectShape = show;
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

	for(int i=0 ; i<teetersTest.size() ; i++)
		teetersTest[i]->Step(deltaTime);
}

void maps::AnimateNature(float deltaTime)
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
	if (_clouds.getTextureRect().left < utils::VIEW_WIDTH)
		_clouds.setTextureRect(IntRect(_clouds.getTextureRect().left + _cloudsMove, 0, utils::VIEW_WIDTH, 212));
	else
		_clouds.setTextureRect(IntRect(0, 0, utils::VIEW_WIDTH, 212));

	if (_clouds2.getTextureRect().left < utils::VIEW_WIDTH)
		_clouds2.setTextureRect(IntRect(_clouds2.getTextureRect().left + _cloudsMove2, 0, utils::VIEW_WIDTH, 212));
	else
		_clouds2.setTextureRect(IntRect(0, 0, utils::VIEW_WIDTH, 212));
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
		case 0:
			return "dirt_a";
		case 1:
			return "dirt_b";
		case 2:
			return "rock_a";
		case 3:
			return "rock_b";
		default:
			return "";
	}
}

editor * maps::GetEditor()
{
	return _editor;
}



void maps::EditMap(RenderWindow * window, float deltaTime, bool step)
{
	//Waiting for thread loading map
	if (_isLoading)
		return;

	if(window != NULL)
		_editor->Display(window, deltaTime);
	if (step)
		_editor->Step(deltaTime);
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
		AnimateNature(deltaTime);
		window->draw(_background);
		window->draw(_backgroundShape);
		window->draw(_clouds2);
		window->draw(_clouds);
		Texture text;
		text.loadFromFile("resources//protos//usine.png");
		Sprite spr;
		spr.setTexture(text);
		spr.setPosition(280, 180);
		window->draw(spr);
		//window->draw(_fog);

		//Background dark effect
		if (_showBackgroundEffectShape)
			window->draw(_backgroundEffectShape);

		//window->draw(usineTest);
		for (int i = 0; i<teetersTest.size(); i++)
			teetersTest[i]->Display(window);

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
	/*RectangleShape rect = RectangleShape(Vector2f(200, 10));
	rect.setFillColor(Color::Blue);
	rect.setOrigin(100, 5);
	rect.setRotation(20);
	rect.setPosition(Vector2f(110, 110));
	window->draw(rect);*/
}
