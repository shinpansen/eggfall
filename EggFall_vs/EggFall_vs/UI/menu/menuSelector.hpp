#ifndef menuSelector_HPP
#define menuSelector_HPP

#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>
#include "../../tools/input.hpp"

using namespace std;
using namespace sf;

class menuSelector
{
public:
    menuSelector();
    menuSelector(Vector2f shapeSize, Vector2f pos, vector<Sprite*> items, int firstItem, input* input, bool showAnimation);
    void ResetAnimation();
    void PerformInputs(RenderWindow* window);
    void SetSelectedItem(int i);
    int GetSelectedItem();
    void SetShapeColor(Color fillColor, Color outlineColor);
    void PerformItemScale();
    void LockSelection(bool lock);
    void PerformSelectedItem(float deltaTime);
    void Display(RenderWindow* window, float deltaTime, Sprite* backItemSprite = NULL, Sprite* backItemSprite2 = NULL);
private:
    //Variables - Objects
    float _deltaTime;
    int _selectedItem;
    int _firstItem;
    int _maxItemIndex;
    bool _keyPressed;
    bool _lockSelection;
    Vector2f _pos;
    Vector2f _shapeSize;
    input* _input;

    //Graphics
    RectangleShape _backShape;
    CircleShape _leftArrow;
    CircleShape _rightArrow;
    vector<Sprite*> _items;
};
#endif
