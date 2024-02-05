#ifndef input_hpp
#define input_hpp

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <fstream>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include "files.hpp"
#include "xmlReader.hpp"

using namespace std;
using namespace sf;

enum Options {
    JUMP,
    ATTACK,
    RUN,
    ROLL,
    UP,
    DOWN,
    LEFT,
    RIGHT,
    VALID,
    BACK,
    PAUSE,
    NONE
};

class input
{
public:
    input();
    input(int player, int deathZone, RenderWindow* window);
    void setKey(int action, Keyboard::Key key);
    void setJoy(string action, int joy);
    bool isKeyPressed(string action);
    Options ResolveOption(string option);
    void LockKey(string action, int frames);
    void PerformLastController(RenderWindow* window);
    void PerformUnlockKeys();
    int GetPlayerNumber();
    bool GetMouseController();
    void WriteXML();
    string GetKeyName(const sf::Keyboard::Key key);
private:
    int _deadZone;
    bool _mouseController;
    Vector2i _lastMousePos;
    RenderWindow* _window;
    files _files;

    //Keyboard
    Keyboard::Key _KEY_JUMP;
    Keyboard::Key _KEY_ATTACK;
    Keyboard::Key _KEY_RUN;
    Keyboard::Key _KEY_ROLL;
    Keyboard::Key _KEY_UP;
    Keyboard::Key _KEY_DOWN;
    Keyboard::Key _KEY_LEFT;
    Keyboard::Key _KEY_RIGHT;
    Keyboard::Key _KEY_VALID;
    Keyboard::Key _KEY_BACK;
    Keyboard::Key _KEY_PAUSE;

    //Joystick
    int _player;
    int _JOY_JUMP;
    int _JOY_ATTACK;
    int _JOY_RUN;
    int _JOY_ROLL;
    int _JOY_VALID;
    int _JOY_BACK;
    int _JOY_PAUSE;

    //Lock keys
    int _LOCK_KEY_JUMP;
    int _LOCK_KEY_ATTACK;
    int _LOCK_KEY_RUN;
    int _LOCK_KEY_ROLL;
    int _LOCK_KEY_UP;
    int _LOCK_KEY_DOWN;
    int _LOCK_KEY_LEFT;
    int _LOCK_KEY_RIGHT;
    int _LOCK_KEY_VALID;
    int _LOCK_KEY_BACK;
};
#endif
