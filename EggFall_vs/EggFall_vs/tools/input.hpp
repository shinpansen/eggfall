#ifndef input_hpp
#define input_hpp

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <fstream>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
//#include <Xinput.h> // ONLY FOR WINDOWS !
#include "files.hpp"
#include "xmlReader.hpp"
#include "utils.hpp"

using namespace std;
using namespace sf;

enum COMMAND {
    CMD_JUMP,
    CMD_ATTACK,
    CMD_RUN,
    CMD_COUNTERACT,
	CMD_DASH,
	CMD_AIM,
    CMD_UP,
    CMD_DOWN,
    CMD_LEFT,
    CMD_RIGHT,
	CMD_ANY_DIRECTION,
    CMD_VALID,
    CMD_BACK,
    CMD_PAUSE,
    CMD_NONE
};

enum X360 {
	A,
	B,
	X,
	Y,
	LB,
	RB,
	START,
	SELECT
};

class input
{
public:
    input();
    input(int player, int deathZone, RenderWindow* window, bool analogControls, bool AI = false);
	static float RoundAngle(float angleFinal);
    void SetKey(int action, Keyboard::Key key);
    void SetJoy(COMMAND cmd, int joy);
	void SetAI(bool AI);
	void SetAnalogControls(bool analog);
    bool isKeyPressed(COMMAND cmd, bool ignoreAI = false);
    void PerformLastController(RenderWindow* window);
    int GetPlayerNumber();
    bool GetMouseController();
    void WriteXML();
	float GetStickDeltaX();
	Vector2f GetStickDirection();
	int GetStickAngle(int defaultAngle = 0);
	void SetVibration(int player, float leftMotor, float rightMotor);
	int GetSFMLJoyNum(X360 button);
	void PressKeyAI(COMMAND cmd, float duration);
	void PressAnalogDirAI(Vector2f dir, float duration);
	void Step(float deltaTime, bool AI = false);
	string GetKeyName(const sf::Keyboard::Key key);
private:
    int _deadZone;
	float _runTimeoutA;
	float _runTimeoutB;
	bool _runKeyPressed;
    bool _mouseController;
	bool _analogControls;
	bool _AI;
    Vector2i _lastMousePos;
    RenderWindow* _window;

    //Keyboard
    Keyboard::Key _KEY_JUMP;
    Keyboard::Key _KEY_ATTACK;
	Keyboard::Key _KEY_COUNTERACT;
	Keyboard::Key _KEY_DASH;
	Keyboard::Key _KEY_AIM;
    Keyboard::Key _KEY_UP;
    Keyboard::Key _KEY_DOWN;
    Keyboard::Key _KEY_LEFT;
    Keyboard::Key _KEY_RIGHT;
    Keyboard::Key _KEY_VALID;
    Keyboard::Key _KEY_BACK;
    Keyboard::Key _KEY_PAUSE;

	//AI keys
	float _AI_JumpKey;
	float _AI_AttackKey;
	float _AI_CounteractKey;
	float _AI_DashKey;
	float _AI_RunKey;
	float _AI_UpKey;
	float _AI_DownKey;
	float _AI_LeftKey;
	float _AI_RightKey;
	float _AI_Analog;
	Vector2f _AI_AnalogDir;

    //Joystick
    int _player;
    int _JOY_JUMP;
    int _JOY_ATTACK;
	int _JOY_COUNTERACT;
	int _JOY_DASH;
	int _JOY_AIM;
    int _JOY_VALID;
    int _JOY_BACK;
    int _JOY_PAUSE;
};
#endif
