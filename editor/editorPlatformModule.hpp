#ifndef editorPlatformModule_HPP
#define editorPlatformModule_HPP

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

using namespace std;
using namespace sf;

enum PLATFORM_TYPE
{
	PLATFORM_LADDER = 0,
	PLATFORM_A = 1,
	PLATFORM_B = 2
};

class maps;
class editor;
class editorGroundModule;
class editorPlatformModule
{
public:
	editorPlatformModule(editor* editor, maps* map, input* input, effects* effectBox);
	bool MockupPosValid();
	void RecreateMockup();
	void PushCurrentMockup();
	void Step(float deltaTime);
	void Display(RenderWindow* window, bool showMockup);
	string SavePlatforms();
private:
	//Variables
	int _colorID;
	int _nbBlocksPlatform;
	float _addingPlatformEffectFrames;
	PLATFORM_TYPE _platformType;

	//Objects
	editor* _editor;
	maps* _map;
	input* _input;
	effects* _effectBox;
	scalablePlatform* _platformMockup;
	scalablePlatform* _platformMockupToMove;

	//Graphics
	Texture _miscelleneousTextureColor;
};
#endif
