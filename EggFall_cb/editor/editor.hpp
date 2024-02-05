#ifndef editor_HPP
#define editor_HPP

#include <iostream>
#include <vector>
#include <functional>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <Box2D/Box2D.h>
#include "../environment/maps.hpp"
#include "../tools/input.hpp"
#include "../tools/utils.hpp"
#include "../tools/Box2DTools.hpp"
#include "../effects/effects.hpp"
#include "../environment/grass.hpp"
#include "editorGroundModule.hpp"
#include "editorPlatformModule.hpp"

using namespace std;
using namespace sf;

enum EDITOR_MODE
{
	EDITOR_ADD,
	EDITOR_SELECT,
	EDITOR_MOVE
};

enum EDITOR_MODULE
{
	EDITOR_GROUND_MODULE = 0,
	EDITOR_PLATFORM_MODULE = 1
};

class moduleSettings
{
public:
	moduleSettings() {};
	moduleSettings(int addingStep, bool showAddingZone, bool showDeletingZone, bool showSelectingZone)
	{
		this->addingStep = addingStep;
		this->showAddingZone = showAddingZone;
		this->showDeletingZone = showDeletingZone;
		this->showSelectingZone = showSelectingZone;
	};
	int addingStep;
	bool showAddingZone;
	bool showDeletingZone;
	bool showSelectingZone;
};

class maps;
class editorGroundModule;
class editor
{
public:
	editor() {};
	editor(maps * map, input * input, effects * effectBox);
	void ReloadModules();
	vector<pair<Sprite*, int>> GetGroundTiles();
	void UpdateSelectArea(int step);
	void Step(float deltaTime);
	void Display(RenderWindow* window);
	void SaveMap();

	//Shared variables with modules
	bool _keyPressed;
	Vector2f _mousePos;
	Vector2f _mouseInitPosMove;
	Vector2f _cursorSize;
	EDITOR_MODE _editorMode;
	EDITOR_MODULE _editorModule;
	RectangleShape _cursor;
	RectangleShape _selectCursor;
	vector<FloatRect> _cursorBounds;
	FloatRect _screenRect;

private:
	//Variables
	bool _displayGrid;

	//Objects
	maps * _map;
	input * _input;
	effects * _effectBox;

	//Modules
	moduleSettings * _moduleSettings;
	editorGroundModule * _editorGroundModule;
	editorPlatformModule * _editorPlatformModule;

	//Graphics
	RectangleShape * _verticalGrid;
	RectangleShape * _horizontalGrid;
};
#endif
