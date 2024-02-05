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
	EDITOR_GROUND_MODULE,
	EDITOR_PLATFORM_MODULE
};

class maps;
class editorGroundModule;
class editor
{
public:
	editor(maps* map, input* input, effects* effectBox);
	void ReloadModules();
	vector<pair<Sprite*, GRASS_COLOR_SCHEME>> GetGroundTiles();
	void Step(float deltaTime);
	void Display(RenderWindow* window);
	void SaveMap();

	//Shared variables with modules
	bool _keyPressed;
	Vector2f _mousePos;
	Vector2f _cursorSize;
	EDITOR_MODE _editorMode;
	EDITOR_MODULE _editorModule;
	RectangleShape _cursor;
	vector<FloatRect> _cursorBounds;

private:
	//Variables
	bool _displayGrid;

	//Objects
	maps* _map;
	input* _input;
	effects* _effectBox;

	//Modules
	editorGroundModule* _editorGroundModule;
	editorPlatformModule* _editorPlatformModule;

	//Graphics
	RectangleShape* _verticalGrid;
	RectangleShape* _horizontalGrid;
};
#endif
