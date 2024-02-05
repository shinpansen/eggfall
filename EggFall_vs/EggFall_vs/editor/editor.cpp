#include "editor.hpp"

static const float SCALE = 30.f;

using namespace std;
using namespace sf;

editor::editor(maps* map, input* input, effects* effectBox)
{
	//Variables
	_displayGrid = true;
	_keyPressed = true;
	_menuOpened = false;
	_closeMenu = false;
	_mousePos = Vector2f(0.f, 0.f);
	_cursorSize = Vector2f(32.f, 32.f);
	_editorMode = EDITOR_ADD;
	//_editorMode = EDITOR_SELECT;
	_editorModule = EDITOR_BRIDGE_MODULE;
	_screenRect = FloatRect(0, 0, utils::VIEW_WIDTH, utils::VIEW_HEIGHT);

	//Objects
	_map = map;
	_input = input;
	_effectBox = effectBox;

	//Menu
	vector<string> menuMode;
	menuMode.push_back("GROUND");
	menuMode.push_back("PLATFORM");
	menuMode.push_back("BRIDGE");
	_menuSwitchMode = new menuList(menuMode, Vector2f(640, 360), _input, true);

	//Modules
	_editorGroundModule = new editorGroundModule(this, map, input, effectBox);
	_editorPlatformModule = new editorPlatformModule(this, map, input, effectBox);
	_editorBridgeModule = new editorBridgeModule(this, map, input, effectBox);

	//Modules settings
	_moduleSettings = new moduleSettings[3];
	_moduleSettings[0] = moduleSettings(32, true, true, true);
	_moduleSettings[1] = moduleSettings(0, false, true, true);
	_moduleSettings[2] = moduleSettings(0, false, true, false);

	//Grid initialization
	_verticalGrid = new RectangleShape[80];
	for (int i = 0; i < 80; i++)
	{
		_verticalGrid[i].setSize(Vector2f(1, utils::VIEW_HEIGHT));
		_verticalGrid[i].setFillColor(Color(100, 30, 120, 100));
		_verticalGrid[i].setPosition(Vector2f(i*16.f, 0.f));
	}

	_horizontalGrid = new RectangleShape[45];
	for (int i = 0; i < 45; i++)
	{
		_horizontalGrid[i].setSize(Vector2f(utils::VIEW_WIDTH, 1));
		_horizontalGrid[i].setFillColor(Color(100, 30, 120, 100));
		_horizontalGrid[i].setPosition(Vector2f(0.f, i*16.f));
	}

	//Cursor
	_cursor.setSize(Vector2f(32, 32));
	_cursor.setFillColor(Color(255, 0, 0, 100));
	_cursor.setPosition(Vector2f(0.f, 0.f));
	_selectCursor.setOutlineColor(Color::White);
}

void editor::ReloadModules()
{
	_editorGroundModule->ReloadMapReferences();
}

bool editor::StepPhysics()
{
	return _editorModule == EDITOR_BRIDGE_MODULE;// && !Mouse::isButtonPressed(Mouse::Left);
}

vector<pair<Sprite*, int>> editor::GetGroundTiles()
{
	return _editorGroundModule->GetTiles();
}

void editor::UpdateSelectArea(int step)
{
	IntRect initRect = IntRect(_mouseInitPosMove.x, _mouseInitPosMove.y, step, step);
	IntRect cursorRect;
	if (step == 16 || step == 32)
		cursorRect = (IntRect)_cursor.getGlobalBounds();
	else
		cursorRect = IntRect(_mousePos.x, _mousePos.y, step, step);
	IntRect selectArea = IntRect(min(initRect.left, cursorRect.left), min(initRect.top, cursorRect.top),

		max(initRect.left + initRect.width, cursorRect.left + cursorRect.width) - min(initRect.left, cursorRect.left),
		max(initRect.top + initRect.height, cursorRect.top + cursorRect.height) - min(initRect.top, cursorRect.top));
	if (selectArea.width % step != 0)
	{
		selectArea.width -= step / 2;
		selectArea.left += initRect.left > cursorRect.left ? step / 2 : 0;
	}
	if (selectArea.height % step != 0)
	{
		selectArea.height -= step / 2;
		selectArea.top += initRect.top > cursorRect.top ? step / 2 : 0;
	}
	selectArea.width = selectArea.width < 2 ? 2 : selectArea.width;
	selectArea.height = selectArea.height < 2 ? 2 : selectArea.height;
	_selectCursor.setPosition(selectArea.left, selectArea.top);
	_selectCursor.setSize(Vector2f(selectArea.width, selectArea.height));
}

void editor::MenuSwitchModeEvents(RenderWindow * window)
{
	if (!_keyPressed && (_input->isKeyPressed(CMD_VALID) ||
		(Mouse::isButtonPressed(Mouse::Left) && _menuSwitchMode->MouseOnSelectedText(window))))
	{
		_keyPressed = true;
		_closeMenu = true;
		_editorMode = EDITOR_ADD;
		switch (_menuSwitchMode->GetSelectedItem())
		{
			case 0:
				_editorModule = EDITOR_MODULE::EDITOR_GROUND_MODULE;
				break;
			case 1:
				_editorModule = EDITOR_MODULE::EDITOR_PLATFORM_MODULE;
				break;
			case 2:
				_editorModule = EDITOR_MODULE::EDITOR_BRIDGE_MODULE;
				break;
		}
	}
	else if (_input->isKeyPressed(CMD_BACK) || (Mouse::isButtonPressed(Mouse::Left) && _menuSwitchMode->IsMouseOutsideMenu(window)))
	{
		_keyPressed = true;
		_closeMenu = true;
	}
}

void editor::Step(float deltaTime)
{
	try
	{
		if (!_menuOpened)
		{
			//Moving cursor
			float step = _editorModule == EDITOR_GROUND_MODULE ? 16.f : 8.f;
			Vector2f cursorPos = Vector2f(((int)((_mousePos.x + step / 2.f) / step)) * step - step, ((int)((_mousePos.y + step / 2.f) / step)) * step - step);
			if (_editorModule == EDITOR_GROUND_MODULE)
			{
				cursorPos.x = cursorPos.x < 0 ? 0 : cursorPos.x;
				cursorPos.x = cursorPos.x + 32 > utils::VIEW_WIDTH ? utils::VIEW_WIDTH - 32 : cursorPos.x;
				cursorPos.y = cursorPos.y < 0 ? 0 : cursorPos.y;
				cursorPos.y = cursorPos.y + 32 > utils::VIEW_HEIGHT ? utils::VIEW_HEIGHT - 32 : cursorPos.y;
			}
			_cursor.setPosition(cursorPos);

			//Num module
			int numModule = static_cast<int>(_editorModule);

			//Can add/select/delete depending module and mode
			bool leftClick = Mouse::isButtonPressed(Mouse::Left) && !Mouse::isButtonPressed(Mouse::Right);
			bool rightClick = Mouse::isButtonPressed(Mouse::Right) && !Mouse::isButtonPressed(Mouse::Left);
			bool canAdd = _editorMode == EDITOR_ADD && leftClick && _moduleSettings[numModule].showAddingZone;
			bool canSelect = _editorMode == EDITOR_SELECT && leftClick && _moduleSettings[numModule].showSelectingZone;
			bool canDelete = _editorMode == EDITOR_ADD && rightClick && _moduleSettings[numModule].showDeletingZone;

			//Select area
			if ((canAdd || canSelect || canDelete) && _cursor.getGlobalBounds().intersects(_screenRect))
			{
				int selectStep = 2;
				if (canAdd)
					selectStep = _moduleSettings[numModule].addingStep;
				if (_selectCursor.getSize() == Vector2f(0, 0)) //Selection init
				{
					_mouseInitPosMove = _cursor.getPosition();
					_selectCursor.setPosition(_mouseInitPosMove);
					_selectCursor.setSize(Vector2f(2, 2));
					if (canAdd)
						_selectCursor.setSize(Vector2f(selectStep, selectStep));

					_selectCursor.setOutlineThickness(_editorMode == EDITOR_ADD ? 0 : 2);
					if (Mouse::isButtonPressed(Mouse::Left))
						_selectCursor.setFillColor(canAdd ? Color(0, 255, 0, 100) : Color::Transparent);
					else if (_editorMode == EDITOR_ADD)
						_selectCursor.setFillColor(Color(255, 0, 0, 100));
				}
				else
					UpdateSelectArea(selectStep);
			}
			else if (!Mouse::isButtonPressed(Mouse::Left) && !Mouse::isButtonPressed(Mouse::Right))
				_selectCursor.setSize(Vector2f(0, 0));

			//Saving map
			if (!_keyPressed && Keyboard::isKeyPressed(Keyboard::E))
			{
				_keyPressed = true;
				SaveMap();
			}

			//Switching module
			if (!_keyPressed && Keyboard::isKeyPressed(Keyboard::Space))
			{
				_keyPressed = true;
				//_editorModule = _editorModule == EDITOR_GROUND_MODULE ? EDITOR_PLATFORM_MODULE : EDITOR_GROUND_MODULE;
				_menuOpened = true;
				return;
			}

			//Stepping modules
			switch (_editorModule)
			{
				case EDITOR_GROUND_MODULE:
					_editorGroundModule->Step(deltaTime);
					break;
				case EDITOR_PLATFORM_MODULE:
					_editorPlatformModule->Step(deltaTime);
					break;
				case EDITOR_BRIDGE_MODULE:
					_editorBridgeModule->Step(deltaTime);
					break;
			}
		}

		//Key pressed released
		if (!Mouse::isButtonPressed(Mouse::Left) && !Mouse::isButtonPressed(Mouse::Right) &&
			!Keyboard::isKeyPressed(Keyboard::Left) && !Keyboard::isKeyPressed(Keyboard::Right) &&
			!Keyboard::isKeyPressed(Keyboard::PageUp) && !Keyboard::isKeyPressed(Keyboard::PageDown) &&
			!Keyboard::isKeyPressed(Keyboard::Up) && !Keyboard::isKeyPressed(Keyboard::Down) &&
			!Keyboard::isKeyPressed(Keyboard::E) && !Keyboard::isKeyPressed(Keyboard::S) &&
			!Keyboard::isKeyPressed(Keyboard::R) && !Keyboard::isKeyPressed(Keyboard::Space))
		{
			_keyPressed = false;
		}

		//Close menu
		if (!_keyPressed && _closeMenu)
		{
			_menuSwitchMode->ResetAnimation();
			_closeMenu = false;
			_menuOpened = false;
		}
	}
	catch (exception e)
	{
		cout << e.what() << endl;
	}
}

void editor::Display(RenderWindow * window, float deltaTime)
{

	//Updating mouse location
	_mousePos = window->mapPixelToCoords(sf::Mouse::getPosition(*window));
	//window->setMouseCursorVisible(_editorMode != EDITOR_ADD);

	//Grid display
	_displayGrid = Keyboard::isKeyPressed(Keyboard::M) ? true : _displayGrid;
	_displayGrid = Keyboard::isKeyPressed(Keyboard::L) ? false : _displayGrid;
	if (_displayGrid)
	{
		for (int i = 0; i < 80; i++)
			window->draw(_verticalGrid[i]);
		for (int i = 0; i < 45; i++)
			window->draw(_horizontalGrid[i]);
	}

	//Switching modules
	/*switch (_editorModule)
	{
		case EDITOR_GROUND_MODULE:
			_editorGroundModule->Display(window);
			break;
		case EDITOR_PLATFORM_MODULE:
			_editorPlatformModule->Display(window);
			break;
	}*/
	_editorGroundModule->Display(window);
	_editorPlatformModule->Display(window, _editorModule == EDITOR_PLATFORM_MODULE && !_menuOpened);
	_editorBridgeModule->Display(window, _editorModule == EDITOR_BRIDGE_MODULE && !_menuOpened);

	if (!_menuOpened)
	{
		//Drawing cursor
		if (_editorModule == EDITOR_GROUND_MODULE && _editorMode == EDITOR_ADD &&
			!Mouse::isButtonPressed(Mouse::Left) && !Mouse::isButtonPressed(Mouse::Right))
			window->draw(_cursor);

		//Select cursor
		int numModule = static_cast<int>(_editorModule);
		if (((_moduleSettings[numModule].showAddingZone || _moduleSettings[numModule].showDeletingZone) && _editorMode == EDITOR_ADD) ||
			(_moduleSettings[numModule].showSelectingZone && _editorMode == EDITOR_SELECT))
			window->draw(_selectCursor);
	}

	//Menu - Switch mode
	if (_menuOpened)
	{
		MenuSwitchModeEvents(window);
		_menuSwitchMode->Display(window, deltaTime, true);
	}
}

void editor::SaveMap()
{
	//Writing file
	cout << "Saving...\n";
	string filePath = "environment//mapFiles//map.xml";
	ofstream mapFile(filePath, ios::out | ios::trunc);
	if (mapFile)
	{
		mapFile << "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>" << endl;
		mapFile << _editorGroundModule->SaveGrounds();
		mapFile << _editorPlatformModule->SavePlatforms();
		mapFile << _editorBridgeModule->SaveBridges();
		mapFile.close();
		cout << "done !\n";
	}
	else
		cerr << "Unable to open file '" << filePath << "' !" << endl;
}
