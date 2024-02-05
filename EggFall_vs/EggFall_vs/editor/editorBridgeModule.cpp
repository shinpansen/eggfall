#include "editorBridgeModule.hpp"

editorBridgeModule::editorBridgeModule(editor * editor, maps * map, input * input, effects * effectBox)
{
	//Variables - objects
	_colorID = 0;
	_nbBlocks = 0;
	_creationTimeout = 0.f;
	_lastMouseClick = false;
	_initPosBridge = Vector2f();
	_editor = editor;
	_map = map;
	_input = input;
	_effectBox = effectBox;

	//Graphics
	_bridgeA.setTexture(_map->_ladderPlatformTexture);
	_bridgeB.setTexture(_map->_ladderPlatformTexture);
	_bridgeA.setOrigin(24, 64);
	_bridgeB.setOrigin(24, 64);
	_bridgeJointShape.setPointCount(0);
	_bridgeJointShape.setFillColor(Color(0, 255, 0, 140));
}

bool editorBridgeModule::BridgePosValid()
{
	if (_bridgeJointShape.getPointCount() == 0 || _bridgeA.getPosition().x < 24.f ||
		_bridgeA.getPosition().x > utils::VIEW_WIDTH - 24.f || _bridgeA.getPosition().y < 64.f ||
		_bridgeA.getPosition().y > utils::VIEW_HEIGHT || _bridgeB.getPosition().x < 24.f ||
		_bridgeB.getPosition().x > utils::VIEW_WIDTH - 24.f || _bridgeB.getPosition().y < 64.f ||
		_bridgeB.getPosition().y > utils::VIEW_HEIGHT)
		return false;
	else
	{
		for (int i = 0; i < _map->_bridges.size(); i++)
			if (_map->_bridges[i]->IntersectRect(_bridgeJointShape.getGlobalBounds()))
				return false;
		return true;
	}
}

pair<Vector2f, Vector2f> editorBridgeModule::GetLeftRightPos()
{
	pair<Vector2f, Vector2f> leftRightPos;
	leftRightPos.first = _bridgeA.getPosition().x < _bridgeB.getPosition().x ? _bridgeA.getPosition() : _bridgeB.getPosition();
	leftRightPos.first = Vector2f(leftRightPos.first.x + 20.f, leftRightPos.first.y - 12.f);
	leftRightPos.second = _bridgeB.getPosition().x > _bridgeA.getPosition().x ? _bridgeB.getPosition() : _bridgeA.getPosition();
	leftRightPos.second = Vector2f(leftRightPos.second.x - 20.f, leftRightPos.second.y - 12.f);
	return leftRightPos;
}

void editorBridgeModule::ShowDeleteEffect(int i)
{
	vector<Sprite*>* bridgeSprites = _map->_bridges[i]->GetBridgeSprites();
	//Blocks effet
	for (int i = 0; i < bridgeSprites->size(); i++)
	{
		Vector2f sfxPos = Vector2f((*bridgeSprites)[i]->getPosition().x, (*bridgeSprites)[i]->getPosition().y);
		_effectBox->AddCircleShape(drawOption(sfxPos, Vector2f(8, 8), Vector2f(8, 8), Color::Red),
			fadeOption(Vector2f(0, 0), 10.f, 0.f, 60.f, 0.f, Vector2f(-0.03f, -0.03f)), physics(false));
	}

	//Left and right bridge effect
	pair<Sprite*, Sprite*> leftRightBridge = _map->_bridges[i]->GetLeftRightBridge();
	Vector2f sfxPos = Vector2f(leftRightBridge.first->getPosition().x - 8.f, leftRightBridge.first->getPosition().y - 24.f);
	_effectBox->AddRectangleShape(drawOption(sfxPos, Vector2f(16, 48), Vector2f(8, 24), Color::Red),
		fadeOption(Vector2f(0, 0), 10.f, 0.f, 60.f, 0.f, Vector2f(-0.03f, -0.03f)), physics(false));
	sfxPos = Vector2f(leftRightBridge.second->getPosition().x + 8.f, leftRightBridge.second->getPosition().y - 24.f);
	_effectBox->AddRectangleShape(drawOption(sfxPos, Vector2f(16, 48), Vector2f(8, 24), Color::Red),
		fadeOption(Vector2f(0, 0), 10.f, 0.f, 60.f, 0.f, Vector2f(-0.03f, -0.03f)), physics(false));
}

void editorBridgeModule::Step(float deltaTime)
{
	//Bridge mockups texture rect + shape joint
	IntRect leftRect = IntRect(64 + 128 * _colorID, 96, 48, 64);
	_bridgeA.setTextureRect(leftRect);

	//Switching color ID
	if (!_editor->_keyPressed && (Keyboard::isKeyPressed(Keyboard::Left) || Keyboard::isKeyPressed(Keyboard::Right)))
	{
		_editor->_keyPressed = true;
		_colorID = _colorID == 0 ? 1 : 0;
	}

	//ADD BRIDGE
	if (_creationTimeout == 0.f && _map->_bridges.size() < 10)
	{
		if (Mouse::isButtonPressed(Mouse::Left) && !Mouse::isButtonPressed(Mouse::Right))
		{
			//Texture rect
			IntRect rightRect = IntRect(144 + 128 * _colorID, 96, 48, 64);
			_bridgeA.setTextureRect(_bridgeA.getPosition().x < _bridgeB.getPosition().x ? leftRect : rightRect);
			_bridgeB.setTextureRect(_bridgeB.getPosition().x > _bridgeA.getPosition().x ? rightRect : leftRect);

			//Joint
			if (fabs(_bridgeA.getPosition().x - _bridgeB.getPosition().x) > 80.f &&
				fabs(_bridgeA.getPosition().x - _bridgeB.getPosition().x)*0.4 > fabs(_bridgeA.getPosition().y - _bridgeB.getPosition().y))
			{
				//Blocks
				pair<Vector2f, Vector2f> leftRightPos = GetLeftRightPos();
				_nbBlocks = utils::DistanceBetween(leftRightPos.first, leftRightPos.second) / 16;
				float yDistance = leftRightPos.second.y - leftRightPos.first.y;
				for (int i = 0; i < _nbBlocks; i++)
				{
					if (_bridgeBlocksShapes.size() < i + 1)
					{
						_bridgeBlocksShapes.push_back(new CircleShape(8.f));
						_bridgeBlocksShapes[i]->setOrigin(8.f, 8.f);
					}
					Vector2f blockPos = Vector2f(leftRightPos.first.x + 8 + i * 16, leftRightPos.first.y + (yDistance / _nbBlocks * i));
					_bridgeBlocksShapes[i]->setPosition(blockPos);
					_bridgeBlocksShapes[i]->setFillColor(BridgePosValid() ? Color(0, 255, 0, 140) : Color(255, 0, 0, 140));
				}

				//Joint vector
				Vector2f jointDir = utils::GetVectorDirection(_bridgeA.getPosition(), _bridgeB.getPosition());
				jointDir = utils::RotateVector(jointDir, _bridgeA.getPosition().x < _bridgeB.getPosition().x ? 90.f : -90.f); // 90° rotation
				jointDir = Vector2f(jointDir.x / max(jointDir.x, jointDir.y), jointDir.y / max(jointDir.x, jointDir.y)); // 1 based reduction
				_bridgeJointShape.setPointCount(4);
				_bridgeJointShape.setPoint(0, _bridgeA.getPosition());
				_bridgeJointShape.setPoint(1, _bridgeB.getPosition());
				_bridgeJointShape.setPoint(2, _bridgeB.getPosition() - (jointDir * 15.f));
				_bridgeJointShape.setPoint(3, _bridgeA.getPosition() - (jointDir * 15.f));
			}
			else
				_bridgeJointShape.setPointCount(0);
		}
		else if (!Mouse::isButtonPressed(Mouse::Left))
		{
			if (_bridgeJointShape.getPointCount() > 0 && _initPosBridge != Vector2f())
			{
				_bridgeA.setPosition(_initPosBridge);
				if (BridgePosValid() && !Mouse::isButtonPressed(Mouse::Right)) //Adding bridge
				{
					pair<Vector2f, Vector2f> leftRightPos = GetLeftRightPos();
					_map->_bridges.push_back(new bridge(_map->_world, &_map->_ladderPlatformTexture, _colorID, leftRightPos.first, leftRightPos.second));
					_creationTimeout = 60.f;
				}
			}
			_bridgeJointShape.setPointCount(0);
			_initPosBridge = Vector2f();
		}
	}

	//Bridge mockups pos
	if (!Mouse::isButtonPressed(Mouse::Left) && _creationTimeout == 0.f)
		_bridgeA.setPosition(_editor->_cursor.getPosition().x, _editor->_cursor.getPosition().y + 32.f);
	if (!_lastMouseClick && Mouse::isButtonPressed(Mouse::Left) && _initPosBridge == Vector2f())
		_initPosBridge = _bridgeA.getPosition();
	_bridgeB.setPosition(_editor->_cursor.getPosition().x, _editor->_cursor.getPosition().y + 32.f);

	//Creation timeout - wait 1 sec between each bridge add
	_creationTimeout = utils::StepCooldown(_creationTimeout, 1.f, deltaTime);
	_lastMouseClick = Mouse::isButtonPressed(Mouse::Left);
}

void editorBridgeModule::Display(RenderWindow * window, bool step)
{
	if (step && _creationTimeout == 0.f && _map->_bridges.size() < 10 && !Mouse::isButtonPressed(Mouse::Right))
	{
		//Bridge mockup colors
		if (Mouse::isButtonPressed(Mouse::Left) && !BridgePosValid())
		{
			_bridgeA.setColor(Color::Red);
			_bridgeB.setColor(Color::Red);
		}
		else
		{
			_bridgeA.setColor(Color::White);
			_bridgeB.setColor(Color::White);
		}

		//Drawing bridges mockups
		if (Mouse::isButtonPressed(Mouse::Left))
		{
			if (_bridgeJointShape.getPointCount() > 0)
				for (int i = 0; i < _nbBlocks; i++)
					if (i < _bridgeBlocksShapes.size() && _bridgeBlocksShapes[i] != NULL)
						window->draw(*_bridgeBlocksShapes[i]);
			//window->draw(_bridgeJointShape);
			window->draw(_bridgeB);
		}
		window->draw(_bridgeA);
	}

	//DELETE BRIDGE
	//Delete operation in display thread to avoid access violation error, when drawing NULL objects in window
	if(step)
	{
		if (Mouse::isButtonPressed(Mouse::Right) && !Mouse::isButtonPressed(Mouse::Left))
		{
			//Highlight bridges
			for (int i = 0; i < _map->_bridges.size(); i++)
			{
				_map->_bridges[i]->SetColor(Color::White);
				if (_map->_bridges[i]->IntersectRect(_editor->_selectCursor.getGlobalBounds()))
					_map->_bridges[i]->SetColor(Color::Red);
			}
		}
		if (_lastSelectCursor.getFillColor().r > 0 && _editor->_selectCursor.getSize() == Vector2f(0, 0) &&
			_lastSelectCursor.getSize() != Vector2f(0, 0))
		{
			//Delete
			vector<bridge*> cleanBridges;
			for (int i = 0; i < _map->_bridges.size(); i++)
			{
				if (_map->_bridges[i]->GetColor() == Color::Red)
				{
					ShowDeleteEffect(i);
					delete _map->_bridges[i];
					_map->_bridges[i] = NULL;
				}
				else
					cleanBridges.push_back(_map->_bridges[i]);
			}
			_map->_bridges = cleanBridges;

			//Reset colors
			for (int i = 0; i < _map->_bridges.size(); i++)
				_map->_bridges[i]->SetColor(Color::White);
		}
	}

	//Drawing map bridges
	for (int i = 0; i < _map->_bridges.size(); i++)
		if (_map->_bridges[i] != NULL)
			_map->_bridges[i]->Display(window);
	
	//Last select cursor update
	_lastSelectCursor = _editor->_selectCursor;
}

string editorBridgeModule::SaveBridges()
{
	//Writing file - opening tag
	stringstream mapFile;
	mapFile << "<bridge>" << endl;

	//Saving bridges
	for (int i = 0; i < _map->_bridges.size(); i++)
	{
		if (_map->_bridges[i] != NULL)
		{
			mapFile << "\t<bridge left_x=\"" << _map->_bridges[i]->GetLeftPos().x << "\" ";
			mapFile << "left_y=\"" << _map->_bridges[i]->GetLeftPos().y << "\" ";
			mapFile << "right_x=\"" << _map->_bridges[i]->GetRightPos().x << "\" ";
			mapFile << "right_y=\"" << _map->_bridges[i]->GetRightPos().y << "\" ";
			mapFile << "colorID=\"" << _map->_bridges[i]->GetColorID() << "\"></bridge>" << endl;
		}
	}

	//Closing tag
	mapFile << "</bridge>" << endl;
	return mapFile.str();
}
