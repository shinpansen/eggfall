#include "ladder.hpp"

using namespace std;
using namespace sf;

ladder::ladder()
{
}

void ladder::AddLadder(Vector2f pos, int width, int height)
{
    //Add new shape
    _shapes.push_back(new RectangleShape());

    //Shape configuration
    int i = _shapes.size()-1;
    _shapes[i]->setSize(Vector2f(width, height));
    _shapes[i]->setFillColor(Color(255,255,255,50));
    _shapes[i]->setOutlineColor(Color(150,150,150));
    _shapes[i]->setOutlineThickness(2);
    _shapes[i]->setOrigin(width/2, height/2);
    _shapes[i]->setPosition(pos);
}

bool ladder::TestCollision(RectangleShape* shape, Vector2f* ladderPosPointer, float* ladderTop)
{
    for(int i=0 ; i<_shapes.size() ; i++)
    {
        Vector2f shapeSize = shape->getSize();
        Vector2f shapePos = shape->getPosition();
        Vector2f ladderSize = _shapes[i]->getSize();
        Vector2f ladderPos = _shapes[i]->getPosition();
        *ladderPosPointer = ladderPos;
        *ladderTop = ladderPos.y - ladderSize.y/2;

        if(((shapePos.y-shapeSize.y/2 >= ladderPos.y-ladderSize.y/2  &&
             shapePos.y-shapeSize.y/2 <= ladderPos.y+ladderSize.y/2) ||
            (shapePos.y+shapeSize.y/2 >= ladderPos.y-ladderSize.y/2  &&
             shapePos.y+shapeSize.y/2 <= ladderPos.y+ladderSize.y/2  &&
             shapePos.y-shapeSize.y/2 >= ladderPos.y-ladderSize.y/2  &&
             shapePos.y-shapeSize.y/2 <= ladderPos.y+ladderSize.y/2) ||
            (shapePos.y-shapeSize.y/2 <= ladderPos.y-ladderSize.y/2  &&
             shapePos.y+shapeSize.y/2 >= ladderPos.y+ladderSize.y/2))
             &&
           ((shapePos.x-shapeSize.x/2 >= ladderPos.x-ladderSize.x/2  &&
             shapePos.x-shapeSize.x/2 <= ladderPos.x+ladderSize.x/2) ||
            (shapePos.x+shapeSize.x/2 >= ladderPos.x-ladderSize.x/2  &&
             shapePos.x+shapeSize.x/2 <= ladderPos.x+ladderSize.x/2) ||
            (shapePos.x-shapeSize.x/2 <= ladderPos.x-ladderSize.x/2  &&
             shapePos.x+shapeSize.x/2 >= ladderPos.x+ladderSize.x/2)))
             return true;

    }
    return false;
}

void ladder::DrawMasks(RenderWindow* window)
{
    for(int i=0 ; i<_shapes.size() ; i++)
        window->draw(*_shapes[i]);
}
