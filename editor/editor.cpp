#include "editor.hpp"

static const float SCALE = 30.f;

using namespace std;
using namespace sf;

editor::editor(maps* map, input* input, effects* effectBox)
{
	//Variables
	_displayGrid = true;
	_keyPressed = true;
	_mousePos = Vector2f(0.f, 0.f);
	_cursorSize = Vector2f(32.f, 32.f);
	_editorMode = EDITOR_ADD;
	_editorModule = EDITOR_GROUND_MODULE;
	//_editorModule = EDITOR_PLATFORM_MODULE;

	//Objects
	_map = map;
	_input = input;
	_effectBox = effectBox;

	//Modules
	_editorGroundModule = new editorGroundModule(this, map, input, effectBox);
	_editorPlatformModule = new editorPlatformModule(this, map, input, effectBox);

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
}

void editor::ReloadModules()
{
	_editorGroundModule->ReloadMapReferences();
}

vector<pair<Sprite*, GRASS_COLOR_SCHEME>> editor::GetGroundTiles()
{
	return _editorGroundModule->GetTiles();
}

void editor::Step(float deltaTime)
{
	try
	{
		//Moving cursor
		Vector2f cursorPos = Vector2f(((int)((_mousePos.x + 8) / 16.f)) * 16 - 16, ((int)((_mousePos.y + 8) / 16.f)) * 16 - 16);
		if (_editorModule == EDITOR_GROUND_MODULE)
		{
			cursorPos.x = cursorPos.x < 0 ? 0 : cursorPos.x;
			cursorPos.x = cursorPos.x + 32 > utils::VIEW_WIDTH ? utils::VIEW_WIDTH - 32 : cursorPos.x;
			cursorPos.y = cursorPos.y < 0 ? 0 : cursorPos.y;
			cursorPos.y = cursorPos.y + 32 > utils::VIEW_HEIGHT ? utils::VIEW_HEIGHT - 32 : cursorPos.y;
		}
		_cursor.setPosition(cursorPos);

		//Switching module
		if (!_keyPressed && Keyboard::isKeyPressed(Keyboard::Space))
		{
			_keyPressed = true;
			_editorModule = _editorModule == EDITOR_GROUND_MODULE ? EDITOR_PLATFORM_MODULE : EDITOR_GROUND_MODULE;
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
		}

		//Key pressed released
		if (!Keyboard::isKeyPressed(Keyboard::Left) && !Keyboard::isKeyPressed(Keyboard::Right) &&
			!Keyboard::isKeyPressed(Keyboard::PageUp) && !Keyboard::isKeyPressed(Keyboard::PageDown) &&
			!Mouse::isButtonPressed(Mouse::Left) && !Mouse::isButtonPressed(Mouse::Right) &&
			!Keyboard::isKeyPressed(Keyboard::Up) && !Keyboard::isKeyPressed(Keyboard::Down) &&
			!Keyboard::isKeyPressed(Keyboard::E) && !Keyboard::isKeyPressed(Keyboard::G) &&
			!Keyboard::isKeyPressed(Keyboard::H) && !Keyboard::isKeyPressed(Keyboard::S) &&
			!Keyboard::isKeyPressed(Keyboard::Space))
		{
			_keyPressed = false;
		}

		//Saving map
		if (!_keyPressed && Keyboard::isKeyPressed(Keyboard::E))
		{
			_keyPressed = true;
			SaveMap();
		}
	}
	catch (exception e)
	{
		cout << e.what() << endl;
	}
}

void editor::Display(RenderWindow * window)
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
	_editorPlatformModule->Display(window, _editorModule == EDITOR_PLATFORM_MODULE);

	//Drawing cursor
	if (_editorModule == EDITOR_GROUND_MODULE && _editorMode == EDITOR_ADD && 
	    !Mouse::isButtonPressed(Mouse::Left) && !Mouse::isButtonPressed(Mouse::Right))
		window->draw(_cursor);
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
		mapFile.close();
		cout << "done !\n";
	}
	else
		cerr << "Unable to open file '" << filePath << "' !" << endl;
}
