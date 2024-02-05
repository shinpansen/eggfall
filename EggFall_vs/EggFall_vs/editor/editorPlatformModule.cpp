#include "editorPlatformModule.hpp"

editorPlatformModule::editorPlatformModule(editor * editor, maps * map, input * input, effects * effectBox)
{
	//Variables - objects
	_colorID = 0;
	_nbBlocksPlatform = 5;
	_addingPlatformEffectFrames = 0.f;
	_deletingDelay = 0.f;
	_platformType = PLATFORM_LADDER;
	_editor = editor;
	_map = map;
	_input = input;
	_effectBox = effectBox;

	//Texuture coloration
	_ladderPlatformTextureColor.loadFromImage(utils::ColorizeImage(files::ladderPlatform, Color::White));

	//Initialization platform mockup
	_platformMockup = new ladder(_map->_world, &_map->_ladderPlatformTexture, Vector2f(0, 0), _nbBlocksPlatform, _colorID);
	_platformMockup->SetPosition(Vector2f(0, 0), true);
	_platformMockupToMove = new ladder(_map->_world, &_ladderPlatformTextureColor, Vector2f(0, 0), _nbBlocksPlatform, _colorID);
	_platformMockupToMove->SetColor(Color::Transparent);
	_platformMockupToMove->SetPosition(Vector2f(0, 0), true);
}

bool editorPlatformModule::PlatformPosValid(scalablePlatform* platformTest)
{
	vector<FloatRect> platformTestBounds = platformTest->GetGlobalBounds();
	for (int b = 0; b < platformTestBounds.size(); b++)
	{
		//Scalable platforms intersect
		for (int i = 0; i < _map->_scalablePlatforms.size(); i++)
		{
			//Transparent platform are moving - Different types of platforms can overlap
			if (_map->_scalablePlatforms[i].first->GetColor().a <= 140 ||
				(platformTest->GetType() == "ladder" &&
			    _map->_scalablePlatforms[i].first->GetType() != platformTest->GetType()))
				continue; 

			vector<FloatRect> platformBounds = _map->_scalablePlatforms[i].first->GetGlobalBounds();
			for (int p = 0; p < platformBounds.size(); p++)
				if (platformTestBounds[b].intersects(platformBounds[p]))
					return false;
		}

		//Ground tiles intersect
		/*vector<pair<Sprite*, int>> tiles = _editor->GetGroundTiles();
		for (int i = 0; i < tiles.size(); i++)
			if (platformTestBounds[b].intersects(tiles[i].first->getGlobalBounds()))
				return false;*/
	}
	return true;
}

void editorPlatformModule::RecreateMockup(bool changeType)
{
	//Deallocate memory before reallocate
	delete _platformMockup;
	delete _platformMockupToMove;

	Vector2f pos = Vector2f(_editor->_cursor.getPosition().x + 8.f, _editor->_cursor.getPosition().y + 8.f);
	if (changeType)
		_colorID = 0;
	switch (_platformType)
	{
		case PLATFORM_LADDER:
			_platformMockup = new ladder(_map->_world, &_map->_ladderPlatformTexture, pos, _nbBlocksPlatform, _colorID);
			_platformMockupToMove = new ladder(_map->_world, &_ladderPlatformTextureColor, pos, _nbBlocksPlatform, _colorID, _platformMockup->GetMiddleTiles());
			break;
		case PLATFORM_A:
			_platformMockup = new platform(_map->_world, &_map->_ladderPlatformTexture, pos, _nbBlocksPlatform, _colorID);
			_platformMockupToMove = new platform(_map->_world, &_ladderPlatformTextureColor, pos, _nbBlocksPlatform, _colorID, _platformMockup->GetMiddleTiles());
			break;
		case PLATFORM_B:
			break;
	}
	_platformMockupToMove->SetColor(Color::Transparent);
}

void editorPlatformModule::PushCurrentMockup()
{
	if (!PlatformPosValid(_platformMockup) || _addingPlatformEffectFrames != 0.f)
		return;

	//Creating platform depending type
	scalablePlatform * newPlatform = NULL;
	if (_platformType == PLATFORM_LADDER)
		newPlatform = new ladder(_map->_world, &_map->_ladderPlatformTexture, _platformMockup->GetPosition(), _nbBlocksPlatform, _colorID, _platformMockup->GetMiddleTiles(), true);
	else if (_platformType == PLATFORM_A)
		newPlatform = new platform(_map->_world, &_map->_ladderPlatformTexture, _platformMockup->GetPosition(), _nbBlocksPlatform, _colorID, _platformMockup->GetMiddleTiles(), true);

	//Pushing new platform at the end of the list
	pair<scalablePlatform*, PLATFORM_TYPE> platformPair;
	platformPair.first = newPlatform;
	platformPair.second = _platformType;
	_map->_scalablePlatforms.push_back(platformPair);

	//Adding animation
	_addingPlatformEffectFrames = 20.f;
}

void editorPlatformModule::DeleteMovingPlatforms(bool resetPlatformsColor)
{
	//Deleting moving platforms
	for (int i = 0; i < _platformsToMove.size(); i++)
	{
		if (_platformsToMove[i].first != NULL)
		{
			delete _platformsToMove[i].first;
			_platformsToMove[i].first = NULL;
		}
	}
	_platformsToMove.clear();

	//Reseting platforms color
	if (resetPlatformsColor)
	{
		for (int i = 0; i < _map->_scalablePlatforms.size(); i++)
		{
			_map->_scalablePlatforms[i].first->SetTexture(&_map->_ladderPlatformTexture);
			_map->_scalablePlatforms[i].first->SetColor(Color::White);
		}
	}
}

vector<int> editorPlatformModule::HighlightSelectedPlatforms(Color color)
{
	vector<int> selectedPlatforms;
	for (int i = 0; i < _map->_scalablePlatforms.size(); i++)
	{
		_map->_scalablePlatforms[i].first->SetColor(Color::White);
		vector<FloatRect> platformBounds = _map->_scalablePlatforms[i].first->GetGlobalBounds(true);
		for (int p = 0; p < platformBounds.size(); p++)
		{
			if (_editor->_selectCursor.getGlobalBounds().intersects(platformBounds[p]))
			{
				_map->_scalablePlatforms[i].first->SetColor(color);
				selectedPlatforms.push_back(i);
				continue;
			}
		}
	}
	return selectedPlatforms;
}

void editorPlatformModule::Step(float deltaTime)
{
	//Switch mode / object
	if (!_editor->_keyPressed && _editor->_editorMode != EDITOR_MOVE)
	{
		//Editor mode switch
		if (Keyboard::isKeyPressed(Keyboard::S))
		{
			EDITOR_MODE lastEditorMode = _editor->_editorMode;
			_editor->_editorMode = _editor->_editorMode == EDITOR_ADD ? EDITOR_SELECT : EDITOR_ADD;
			if (lastEditorMode == EDITOR_SELECT && _editor->_editorMode == EDITOR_ADD)
				DeleteMovingPlatforms(true);
			_editor->_keyPressed = true;
		}
		//Object type switch
		if (Keyboard::isKeyPressed(Keyboard::Up) || Keyboard::isKeyPressed(Keyboard::Down))
		{
			_platformType = _platformType == PLATFORM_LADDER ? PLATFORM_A : PLATFORM_LADDER;
			RecreateMockup(true);
			_editor->_keyPressed = true;
		}
	}

	//ADD - DELETE
	if (_editor->_editorMode == EDITOR_ADD)
	{
		//Moving mockup
		_platformMockup->SetPosition(Vector2f(_editor->_cursor.getPosition().x + 8.f, _editor->_cursor.getPosition().y + 8.f), true);
		if (_addingPlatformEffectFrames == 0.f)
			_platformMockupToMove->SetPosition(_platformMockup->GetPosition(), true);

		//Platform mockup color regarding valid position
		if (_addingPlatformEffectFrames == 0.f)
		{
			Color invalidColor = Color(255, 0, 0, 150);
			_platformMockupToMove->SetColor(!PlatformPosValid(_platformMockup) ? invalidColor : Color::Transparent);
		}
		else
		{
			int alpha = _addingPlatformEffectFrames*20.f > 255 ? 255 : _addingPlatformEffectFrames*20.f;
			_platformMockupToMove->SetColor(Color(255, 255, 255, alpha));
		}
		_addingPlatformEffectFrames = utils::StepCooldown(_addingPlatformEffectFrames, 1.f, deltaTime);

		// Adding/changing platform - randomize tiles
		if (!_editor->_keyPressed && _deletingDelay == 0.f)
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
			if (Keyboard::isKeyPressed(Keyboard::R))
				RecreateMockup();
		}

		//Deleting platform
		if (_lastSelectCursor.getFillColor().r > 0 && _editor->_selectCursor.getSize() == Vector2f(0, 0) &&
			_lastSelectCursor.getSize() != Vector2f(0, 0))
		{
			vector<pair<scalablePlatform*, int>> platformsClean;
			for (int i = 0; i < _map->_scalablePlatforms.size(); i++)
			{
				if (_map->_scalablePlatforms[i].first->GetColor() == Color::Red)
				{
					//Removing platform effect
					vector<FloatRect> platformBounds = _map->_scalablePlatforms[i].first->GetGlobalBounds();
					for(int p=0 ; p<platformBounds.size() ; p++)
						_effectBox->AddRectangleShape(drawOption(Vector2f(platformBounds[p].left + platformBounds[p].width / 2.f,
							platformBounds[p].top + platformBounds[p].height / 2.f), Vector2f(platformBounds[p].width, platformBounds[p].height), 
							Vector2f(platformBounds[p].width / 2.f, platformBounds[p].height / 2.f), Color::Red),
							fadeOption(Vector2f(0, 0), 10.f, 0.f, 60.f, 0.f, Vector2f(-0.03f, -0.03f)), physics(false));
					//Deallocate memory
					delete _map->_scalablePlatforms[i].first;
					_deletingDelay = 30.f;
				}
				else
					platformsClean.push_back(_map->_scalablePlatforms[i]);
			}
			_map->_scalablePlatforms = platformsClean;
		}
		_deletingDelay = utils::StepCooldown(_deletingDelay, 1.f, deltaTime);

		//Changing platform color if selecting/deleting
		if (_editor->_selectCursor.getFillColor().r > 0)
			HighlightSelectedPlatforms(Color::Red);
	}

	//SELECT
	if (_editor->_editorMode == EDITOR_SELECT)
	{
		if (Mouse::isButtonPressed(Mouse::Left) && !Mouse::isButtonPressed(Mouse::Right))
		{
			bool moving = false;
			if (!_editor->_keyPressed && _platformsToMove.size() > 0)
			{
				for (int i = 0; i < _platformsToMove.size(); i++)
				{
					vector<FloatRect> _platformBounds = _platformsToMove[i].first->GetGlobalBounds(true);
					for (int b = 0; b < _platformBounds.size(); b++)
					{
						if (_platformBounds[b].contains(_editor->_mousePos))
						{
							moving = true;
							_editor->_mouseInitPosMove = Vector2f(((int)(_editor->_mousePos.x / 8.f)) * 8.f, ((int)(_editor->_mousePos.y / 8.f)) * 8.f);
							_editor->_editorMode = EDITOR_MOVE;
							break;
						}
					}
				}
			}
			_editor->_keyPressed = true;
			if (!moving)
			{
				if (_editor->_selectCursor.getSize() != Vector2f(0, 0)) //Selection init
				{
					//Select - add to moving tiles list
					vector<int> selectedPlatforms = HighlightSelectedPlatforms(Color(200, 200, 200, 140));
					DeleteMovingPlatforms();
					for (int i = 0; i < selectedPlatforms.size(); i++)
					{
						pair<scalablePlatform*, PLATFORM_TYPE> platformToMovePair;
						scalablePlatform* platformToClone = _map->_scalablePlatforms[selectedPlatforms[i]].first;
						if((PLATFORM_TYPE)_map->_scalablePlatforms[selectedPlatforms[i]].second == PLATFORM_LADDER)
							platformToMovePair.first = new ladder(_map->_world, &_ladderPlatformTextureColor, platformToClone->GetPosition(), 
								platformToClone->GetNbBlocks(), platformToClone->GetColorID(), platformToClone->GetMiddleTiles(), true);
						else if ((PLATFORM_TYPE)_map->_scalablePlatforms[selectedPlatforms[i]].second == PLATFORM_A)
							platformToMovePair.first = new platform(_map->_world, &_ladderPlatformTextureColor, platformToClone->GetPosition(),
								platformToClone->GetNbBlocks(), platformToClone->GetColorID(), platformToClone->GetMiddleTiles(), true);
						platformToMovePair.first->SetColor(Color(0, 255, 0, 150));
						platformToMovePair.second = (PLATFORM_TYPE)_map->_scalablePlatforms[selectedPlatforms[i]].second;
						_platformsToMove.push_back(platformToMovePair);
					}
				}
			}
		}
	}

	//MOVE
	if (_editor->_editorMode == EDITOR_MOVE)
	{
		bool newPosValid = true;
		Vector2f platformNewPos = Vector2f(((int)(_editor->_mousePos.x / 8.f)) * 8.f, ((int)(_editor->_mousePos.y / 8.f)) * 8.f);
		if (platformNewPos != _editor->_mouseInitPosMove)
		{
			//Moving
			for (int i = 0; i < _platformsToMove.size(); i++)
				_platformsToMove[i].first->Move(platformNewPos.x - _editor->_mouseInitPosMove.x, platformNewPos.y - _editor->_mouseInitPosMove.y);
			_editor->_mouseInitPosMove = platformNewPos;
		}

		//Testing if new position is valid (i.e. doesn't intersect other tiles)
		for (int i = 0; i < _platformsToMove.size(); i++)
		{
			if (newPosValid && !PlatformPosValid(_platformsToMove[i].first))
			{
				newPosValid = false;
				break;
			}
		}

		//Coloration : green = move valid - red = move invalid
		Color colorMove = newPosValid ? Color(0, 255, 0, 150) : Color(255, 0, 0, 150);
		for (int i = 0; i < _platformsToMove.size(); i++)
			_platformsToMove[i].first->SetColor(colorMove);

		//Click released - move ends
		if (!_editor->_keyPressed)
		{
			if (newPosValid)
			{
				//Adding moved platforms in main list - ignoring old tiles
				for (int i = 0; i < _map->_scalablePlatforms.size(); i++)
				{
					if (_map->_scalablePlatforms[i].first->GetColor().a > 140)
						_platformsToMove.push_back(_map->_scalablePlatforms[i]);
					else if (_map->_scalablePlatforms[i].first != NULL)
					{
						delete _map->_scalablePlatforms[i].first;
						_map->_scalablePlatforms[i].first = NULL;
					}
				}
				_map->_scalablePlatforms = _platformsToMove;

				//Reset
				_platformsToMove.clear();
				for (int i = 0; i < _map->_scalablePlatforms.size(); i++)
				{
					_map->_scalablePlatforms[i].first->SetTexture(&_map->_ladderPlatformTexture);
					_map->_scalablePlatforms[i].first->SetColor(Color::White);
				}
			}
			else //Cancel move
				DeleteMovingPlatforms(true);
			
			//Go back to select mode
			_editor->_editorMode = EDITOR_SELECT;
		}
	}

	//Last select cursor update
	_lastSelectCursor = _editor->_selectCursor;
}

void editorPlatformModule::Display(RenderWindow * window, bool showMockup)
{
	//Drawing all platforms
	for (int i = 0; i < _map->_scalablePlatforms.size(); i++)
		if (_map->_scalablePlatforms[i].first != NULL)
			_map->_scalablePlatforms[i].first->Display(window);
	for (int i = 0; i < _platformsToMove.size(); i++)
		if (_platformsToMove[i].first != NULL)
			_platformsToMove[i].first->Display(window);

	//Drawing mockup
	if (_editor->_editorMode == EDITOR_ADD && !Mouse::isButtonPressed(Mouse::Right) && showMockup && _deletingDelay == 0.f)
	{
		if (_addingPlatformEffectFrames == 0.f)
			_platformMockup->Display(window);
		_platformMockupToMove->Display(window);
	}
}

string editorPlatformModule::SavePlatforms()
{
	//Writing file - opening tag
	stringstream mapFile;
	mapFile << "<platform>" << endl;

	//Saving platforms
	for (int i = 0; i < _map->_scalablePlatforms.size(); i++)
	{
		if (_map->_scalablePlatforms[i].first == NULL)
			continue;
		switch ((PLATFORM_TYPE)_map->_scalablePlatforms[i].second)
		{
			case PLATFORM_LADDER:
				mapFile << "\t<ladder x=\"" << _map->_scalablePlatforms[i].first->GetPosition().x << "\" ";
				mapFile << "y=\"" << _map->_scalablePlatforms[i].first->GetPosition().y << "\" ";
				mapFile << "blocks=\"" << _map->_scalablePlatforms[i].first->GetNbBlocks() << "\" ";
				mapFile << "colorID=\"" << _map->_scalablePlatforms[i].first->GetColorID() << "\" ";
				mapFile << "tiles=\"" << _map->_scalablePlatforms[i].first->GetMiddleTilesString() << "\"></ladder>" << endl;
				break;
			case PLATFORM_A:
				mapFile << "\t<platform x=\"" << _map->_scalablePlatforms[i].first->GetPosition().x << "\" ";
				mapFile << "y=\"" << _map->_scalablePlatforms[i].first->GetPosition().y << "\" ";
				mapFile << "blocks=\"" << _map->_scalablePlatforms[i].first->GetNbBlocks() << "\" ";
				mapFile << "colorID=\"" << _map->_scalablePlatforms[i].first->GetColorID() << "\" ";
				mapFile << "tiles=\"" << _map->_scalablePlatforms[i].first->GetMiddleTilesString() << "\"></platform>" << endl;
				break;
		}
	}

	//Closing tag
	mapFile << "</platform>" << endl;
	return mapFile.str();
}
