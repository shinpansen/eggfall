#ifndef camera_HPP
#define camera_HPP

#include <iostream>
#include <math.h>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <Box2D/Box2D.h>
#include "../characters/hero.hpp"

using namespace std;
using namespace sf;

class camera
{
public:
    camera();
    camera(View* view, RenderWindow* window, int speed, int worldLimit, Vector2i screenSize);
    void SetSpeed(int speed);
    void FollowObject(Vector2f objectPos);
    void FollowHeros(vector<hero*> heros);
    void PerformLoop(hero* hero);
    Vector2i minPos(vector<hero*> heros);
    Vector2i maxPos(vector<hero*> heros);
private:
    View* _view;
    RenderWindow* _window;
    int _speed;
    int _worldLimit;
    Vector2i _screenSize;
};
#endif
