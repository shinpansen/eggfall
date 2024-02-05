#ifndef editorPlatformModule_HPP
#define editorPlatformModule_HPP

#include "editor.hpp"
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
class editorBridgeModule;
class editorPlatformModule
{
public:
	editorPlatformModule(editor* editor, maps* map, input* input, effects* effectBox);
	bool PlatformPosValid(scalablePlatform* platformTest);
	void RecreateMockup(bool changeType = false);
	void PushCurrentMockup();
	void DeleteMovingPlatforms(bool resetPlatformsColor = false);
	vector<int> HighlightSelectedPlatforms(Color color);
	void Step(float deltaTime);
	void Display(RenderWindow* window, bool showMockup);
	string SavePlatforms();
private:
	//Variables
	int _colorID;
	int _nbBlocksPlatform;
	float _addingPlatformEffectFrames;
	float _deletingDelay;
	PLATFORM_TYPE _platformType;

	//Objects
	editor* _editor;
	maps* _map;
	input* _input;
	effects* _effectBox;
	scalablePlatform* _platformMockup;
	scalablePlatform* _platformMockupToMove;
	vector<pair<scalablePlatform*, int>> _platformsToMove;

	//Graphics
	Texture _ladderPlatformTextureColor;
	RectangleShape _lastSelectCursor;
};
#endif
