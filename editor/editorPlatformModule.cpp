#include "editorPlatformModule.hpp"

editorPlatformModule::editorPlatformModule(editor * editor, maps * map, input * input, effects * effectBox)
{
	//Variables - objects
	_colorID = 0;
	_nbBlocksPlatform = 5;
	_addingPlatformEffectFrames = 0.f;
	_platformType = PLATFORM_LADDER;
	_editor = editor;
	_map = map;
	_input = input;
	_effectBox = effectBox;

	//Texuture coloration
	_miscelleneousTextureColor.loadFromImage(utils::ColorizeImage(files::miscellaneous, Color::White));

	//Initialization platform mockup
	_platformMockup = new ladder(_map->_world, &_map->_miscelleneousTexture, Vector2f(0, 0), _nbBlocksPlatform, _colorID);
	_platformMockup->SetPosition(Vector2f(0, 0), true);
	_platformMockupToMove = new ladder(_map->_world, &_miscelleneousTextureColor, Vector2f(0, 0), _nbBlocksPlatform, _colorID);
	_platformMockupToMove->SetColor(Color::Transparent);
	_platformMockupToMove->SetPosition(Vector2f(0, 0), true);
}

bool editorPlatformModule::MockupPosValid()
{
	vector<FloatRect> mockupBounds = _platformMockup->GetGlobalBounds();
	for (int b = 0; b < mockupBounds.size(); b++)
	{
		//Scalable platforms intersect
		for (int i = 0; i < _map->_scalablePlatforms.size(); i++)
		{
			vector<FloatRect> platformBounds = _map->_scalablePlatforms[i].first->GetGlobalBounds();
			for (int p = 0; p < platformBounds.size(); p++)
				if (mockupBounds[b].intersects(platformBounds[p]))
					return false;
		}

		//Ground tiles intersect
		vector<pair<Sprite*, GRASS_COLOR_SCHEME>> tiles = _editor->GetGroundTiles();
		for (int i = 0; i < tiles.size(); i++)
			if (mockupBounds[b].intersects(tiles[i].first->getGlobalBounds()))
				return false;
	}
	return true;
}

void editorPlatformModule::RecreateMockup()
{
	//Deallocate memory before reallocate
	delete _platformMockup;
	delete _platformMockupToMove;

	Vector2f pos = Vector2f(_editor->_cursor.getPosition().x + 16, _editor->_cursor.getPosition().y + 16);
	switch (_platformType)
	{
		case PLATFORM_LADDER:
			_platformMockup = new ladder(_map->_world, &_map->_miscelleneousTexture, pos, _nbBlocksPlatform, _colorID);
			_platformMockupToMove = new ladder(_map->_world, &_miscelleneousTextureColor, pos, _nbBlocksPlatform, _colorID, _platformMockup->GetMiddleTiles());
			break;
		case PLATFORM_A:
			break;
		case PLATFORM_B:
			break;
	}
	_platformMockupToMove->SetColor(Color::Transparent);
}

void editorPlatformModule::PushCurrentMockup()
{
	if (!MockupPosValid() || _addingPlatformEffectFrames != 0.f)
		return;

	//Creating platform depending type
	scalablePlatform * newPlatform = NULL;
	if (_platformType == PLATFORM_LADDER)
		newPlatform = new ladder(_map->_world, &_map->_miscelleneousTexture, _platformMockup->GetPosition(), _nbBlocksPlatform, _colorID, _platformMockup->GetMiddleTiles(), true);

	//Pushing new platform at the end of the list
	pair<scalablePlatform*, PLATFORM_TYPE> platformPair;
	platformPair.first = newPlatform;
	platformPair.second = _platformType;
	_map->_scalablePlatforms.push_back(platformPair);

	//Adding animation
	_addingPlatformEffectFrames = 20.f;
}

void editorPlatformModule::Step(float deltaTime)
{
	if (_editor->_editorMode == EDITOR_ADD)
	{
		//Moving mockup
		_platformMockup->SetPosition(Vector2f(_editor->_cursor.getPosition().x + 16, _editor->_cursor.getPosition().y + 16), true);
		if (_addingPlatformEffectFrames == 0.f)
			_platformMockupToMove->SetPosition(_platformMockup->GetPosition(), true);

		//Platform mockup color regarding valid position
		if (_addingPlatformEffectFrames == 0.f)
		{
			Color invalidColor = Color(255, 0, 0, 150);
			_platformMockupToMove->SetColor(!MockupPosValid() ? invalidColor : Color::Transparent);
		}
		else
		{
			int alpha = _addingPlatformEffectFrames*20.f > 255 ? 255 : _addingPlatformEffectFrames*20.f;
			_platformMockupToMove->SetColor(Color(255, 255, 255, alpha));
		}
		_addingPlatformEffectFrames = utils::StepCooldown(_addingPlatformEffectFrames, 1.f, deltaTime);
	}

	//EDITOR_ADD - Adding/changing platform - randomize tiles
	if (!_editor->_keyPressed && _editor->_editorMode == EDITOR_ADD)
	{
		_editor->_keyPressed = true;

		//Changing platform size
		if ((Keyboard::isKeyPressed(Keyboard::PageUp) || Keyboard::isKeyPressed(Keyboard::PageDown)))
		{
			if (Keyboard::isKeyPressed(Keyboard::PageUp))
				_nbBlocksPlatform += _nbBlocksPlatform < _platformMockup->GetMaxNbBlocks() ? 1 : 0;
			else
				_nbBlocksPlatform -= _nbBlocksPlatform > 3 ? 1 : 0;

			//Creating new platform with new size
			RecreateMockup();
		}

		//Switching color ID
		if ((Keyboard::isKeyPressed(Keyboard::Left) || Keyboard::isKeyPressed(Keyboard::Right)))
		{
			if (Keyboard::isKeyPressed(Keyboard::Left))
				_colorID -= _colorID > 0 ? 1 : -_platformMockup->GetMaxColorID();
			else
				_colorID += _colorID < _platformMockup->GetMaxColorID() ? 1 : -_colorID;
			_platformMockup->SetColorID(_colorID);
		}

		//Adding platform
		if (Mouse::isButtonPressed(Mouse::Left))
			PushCurrentMockup();

		//Platform randomization
		if(Mouse::isButtonPressed(Mouse::Right))
			RecreateMockup();
	}
}

void editorPlatformModule::Display(RenderWindow * window, bool showMockup)
{
	//Drawing all platforms
	for (int i = 0; i < _map->_scalablePlatforms.size(); i++)
		if (_map->_scalablePlatforms[i].first != NULL)
			_map->_scalablePlatforms[i].first->Display(window);

	//Drawing mockup
	if (_editor->_editorMode == EDITOR_ADD && showMockup)
	{
		if(_addingPlatformEffectFrames == 0.f)
			_platformMockup->Display(window);
		_platformMockupToMove->Display(window);
	}
}

string editorPlatformModule::SavePlatforms()
{
	//Writing file
	stringstream mapFile;
	mapFile << "<platform>" << endl;

	//Saving platforms
	for (int i = 0; i < _map->_scalablePlatforms.size(); i++)
	{
		if (_map->_scalablePlatforms[i].first == NULL)
			continue;
		switch (_map->_scalablePlatforms[i].second)
		{
			case PLATFORM_LADDER:
				mapFile << "\t<ladder x=\"" << _map->_scalablePlatforms[i].first->GetPosition().x << "\" ";
				mapFile << "y=\"" << _map->_scalablePlatforms[i].first->GetPosition().y << "\" ";
				mapFile << "blocks=\"" << _map->_scalablePlatforms[i].first->GetNbBlocks() << "\" ";
				mapFile << "colorID=\"" << _map->_scalablePlatforms[i].first->GetColorID() << "\" ";
				mapFile << "tiles=\"" << _map->_scalablePlatforms[i].first->GetMiddleTilesString() << "\"></ladder>" << endl;
				break;
		}
	}

	mapFile << "</platform>" << endl;
	return mapFile.str();
}
