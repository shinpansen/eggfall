#include "camera.hpp"

static const float SCALE = 30.f;

using namespace std;
using namespace sf;

camera::camera()
{
}

camera::camera(View* view, RenderWindow* window, int speed, int worldLimit, Vector2i screenSize)
{
    _view = view;
    _window = window;
    _speed = (speed >= 0 && speed <= 10) ? speed : 5;
    _worldLimit = worldLimit;
    _screenSize = screenSize;
}

void camera::SetSpeed(int speed)
{
    _speed = (speed >= 0 && speed <= 10) ? speed : 5;
}

void camera::FollowObject(Vector2f objectPos)
{
    Vector2f viewCenter = _view->getCenter();
    double gapX = fabs(viewCenter.x - objectPos.x)/(10-_speed);
    double gapY = fabs(viewCenter.y - objectPos.y)/(10-_speed);
    viewCenter.x += (objectPos.x >= viewCenter.x) ? gapX : -gapX;
    viewCenter.y += (objectPos.y >= viewCenter.y) ? gapY : -gapY;

    _view->setCenter(viewCenter);
    _window->setView(*_view);
}

void camera::FollowHeros(vector<hero*> heros)
{
    Vector2i topLeft = minPos(heros);
    Vector2i bottomRight = maxPos(heros);
    float sizeX = bottomRight.x - topLeft.x + 200;
    float sizeY = bottomRight.y - topLeft.y + 200;

    if(sizeX*_screenSize.y/_screenSize.x >= sizeY)
    {
        Vector2f viewSize = _view->getSize();
        double gapX = fabs(viewSize.x - sizeX)/50;
        double gapY = fabs(viewSize.y - (sizeX*_screenSize.y/_screenSize.x))/50;
        viewSize.x += sizeX >= viewSize.x ? gapX : -gapX*10;
        viewSize.y += (sizeX*_screenSize.y/_screenSize.x) >= viewSize.y ? gapY : -gapY*10;
        _view->setSize(viewSize.x, viewSize.y);
        //_view->setSize(sizeX, sizeX*_screenSize.y/_screenSize.x);
        //_view->setCenter(topLeft.x+(sizeX/2)-100, topLeft.y+(sizeY/2)-100);
        FollowObject(Vector2f(topLeft.x+(sizeX/2)-100, topLeft.y+(sizeY/2)-100));
    }
    else
    {
        Vector2f viewSize = _view->getSize();
        double gapX = fabs(viewSize.x - (sizeY*_screenSize.x/_screenSize.y))/50;
        double gapY = fabs(viewSize.y - sizeY)/50;
        viewSize.x += (sizeY*_screenSize.x/_screenSize.y) >= viewSize.x ? gapX : -gapX*10;
        viewSize.y += sizeY >= viewSize.y ? gapY : -gapY*10;
        _view->setSize(viewSize.x, viewSize.y);
        //_view->setSize(sizeY*_screenSize.x/_screenSize.y, sizeY);
        //_view->setCenter(topLeft.x+(sizeX/2)-100, topLeft.y+(sizeY/2)-100);
        FollowObject(Vector2f(topLeft.x+(sizeX/2)-100, topLeft.y+(sizeY/2)-100));
    }

    //Resizing if to small
    if(_view->getSize().x < _screenSize.x || _view->getSize().y < _screenSize.y)
        _view->setSize(_screenSize.x, _screenSize.y);
}

void camera::PerformLoop(hero* hero)
{
    b2Vec2 pos = hero->GetBodyA()->GetPosition();
    float gapBetweenHeroX = _view->getCenter().x - (pos.x*SCALE);
    float gapBetweenHeroY = _view->getCenter().y - (pos.y*SCALE);
    if(pos.y*SCALE > _worldLimit)
    {
        hero->GetBodyA()->SetTransform(b2Vec2(pos.x, -500/SCALE), 0);
        _view->setCenter((pos.x*SCALE)+gapBetweenHeroX, -500 + gapBetweenHeroY);
        _window->setView(*_view);
    }
}

Vector2i camera::minPos(vector<hero*> heros)
{
    int minX = heros[0]->GetPosition().x;
    int minY = heros[0]->GetPosition().y;
    for(int i=1 ; i<heros.size() ; i++)
    {
        if(heros[i]->GetPosition().x < minX)
            minX = heros[i]->GetPosition().x;
        if(heros[i]->GetPosition().y < minY)
            minY = heros[i]->GetPosition().y;
    }
    return Vector2i(minX, minY);
}

Vector2i camera::maxPos(vector<hero*> heros)
{
    int maxX = heros[0]->GetPosition().x;
    int maxY = heros[0]->GetPosition().y;
    for(int i=1 ; i<heros.size() ; i++)
    {
        if(heros[i]->GetPosition().x > maxX)
            maxX = heros[i]->GetPosition().x;
        if(heros[i]->GetPosition().y > maxY)
            maxY = heros[i]->GetPosition().y;
    }
    return Vector2i(maxX, maxY);
}
