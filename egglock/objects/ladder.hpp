#ifndef ladder_HPP
#define ladder_HPP

#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

using namespace std;
using namespace sf;

class ladder
{
public:
    ladder();
    void AddLadder(Vector2f pos, int width, int height);
    bool TestCollision(RectangleShape* shape, Vector2f* ladderPosPointer, float* ladderTop);
    void DrawMasks(RenderWindow* window);
private:
    vector<RectangleShape*> _shapes;
};
#endif
