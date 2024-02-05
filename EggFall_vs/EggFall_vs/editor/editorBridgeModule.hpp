#ifndef editorBridgeModule_HPP
#define editorBridgeModule_HPP

#include "editor.hpp"

using namespace std;
using namespace sf;

class maps;
class editor;
class editorGroundModule;
class editorPlatformModule;
class editorBridgeModule
{
public:
	editorBridgeModule(editor * editor, maps * map, input * input, effects * effectBox);
	bool BridgePosValid();
	pair<Vector2f, Vector2f> GetLeftRightPos();
	void ShowDeleteEffect(int i);
	void Step(float deltaTime);
	void Display(RenderWindow* window, bool step);
	string SaveBridges();
private:
	//Variables
	int _colorID;
	int _nbBlocks;
	float _creationTimeout;
	bool _lastMouseClick;
	Vector2f _initPosBridge;

	//Objects
	editor * _editor;
	maps * _map;
	input * _input;
	effects * _effectBox;

	//Graphics
	Sprite _bridgeA;
	Sprite _bridgeB;
	vector<CircleShape*> _bridgeBlocksShapes;
	ConvexShape _bridgeJointShape;
	RectangleShape _lastSelectCursor;
};
#endif
