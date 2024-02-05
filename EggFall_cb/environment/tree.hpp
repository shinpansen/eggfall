#ifndef tree_HPP
#define tree_HPP

#include <iostream>
#include <math.h>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <Box2D/Box2D.h>
#include "../tools/files.hpp"
#include "../tools/Box2DTools.hpp"
#include "../effects/effects.hpp"
#include "../effects/effectObj.hpp"

using namespace std;
using namespace sf;

class tree
{
public:
    tree();
    tree(Vector2f pos, Vector2i heightMinMax, Texture* texture, IntRect rect, Color color, b2World* world, effects* effectBox);
    tree(Vector2f pos, Texture* texture, int colorID, int kindOfTree, b2World* world, effects* effectBox);
    void performTeeter(float deltaTime);
    void CalcAngle();
    Vector2i getHeightMinMax(int kindOfTree);
    Color getColor(int colorID);
    Vector2f GetPosition();
    void SetPosition(Vector2f pos);
    void SetKind(int colorID, int kindOfTree);
    int GetColorID();
    int GetKindOfTree();
    void Step(float deltaTime);
    void DrawTree(RenderWindow* window);
protected:
    //Variables
    Vector2f _position;
    Vector2i _heightMinMax;
    IntRect _rect;
    double _angle;
    double _angleToReach;
    int _colorID;
    int _kindOfTree;

    //Objects
    effects* _effectBox;
    b2World* _world;
    Sprite _sprite;
    Texture* _texture;
    Color _color;
	drawOption* _leafDrawOption;
	drawOption* _woodDrawOption;
};
#endif
