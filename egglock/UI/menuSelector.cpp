#include "menuSelector.hpp"

using namespace std;
using namespace sf;

menuSelector::menuSelector()
{
}

menuSelector::menuSelector(Vector2f shapeSize, Vector2f pos, vector<Sprite*> items, int firstItem, input* input, bool showAnimation)
{
    //Variables
    _selectedItem = firstItem;
    _firstItem = firstItem;
    _maxItemIndex = items.size()-1;
    _keyPressed = false;
    _lockSelection = false;
    _pos = pos;
    _shapeSize = shapeSize;
    _input = input;

    //RectangleShape
    _backShape.setSize(shapeSize);
    _backShape.setOrigin(shapeSize.x/2, shapeSize.y/2);
    _backShape.setFillColor(Color(33,33,33));
    _backShape.setOutlineColor(Color(50,50,50));
    _backShape.setOutlineThickness(5);
    _backShape.setPosition(Vector2f(_pos.x, _pos.y));
    _backShape.setScale(showAnimation ? Vector2f(0,1) : Vector2f(1,1));

    //Arrows
    _leftArrow = CircleShape(10,3);
    _leftArrow.setFillColor(Color(255,255,255));
    _leftArrow.setScale(1.8,0.8);
    _leftArrow.setOrigin(10,10);
    _leftArrow.setRotation(270);
    _leftArrow.setPosition(_pos.x - shapeSize.x/2 + 15, _pos.y);
    _rightArrow = _leftArrow;
    _rightArrow.setRotation(90);
    _rightArrow.setPosition(_pos.x + shapeSize.x/2 - 15, _pos.y);

    //Items
    _items = items;
    for(int i=0 ; i<_items.size() ; i++)
    {
        int alpha = i == _selectedItem ? 255 : 0;
        _items[i]->setColor(Color(255,255,255,alpha));
        _items[i]->setPosition(_pos);
    }
}

void menuSelector::ResetAnimation()
{
    _backShape.setScale(0,1);
    _selectedItem = _firstItem;
}

void menuSelector::PerformInputs(RenderWindow* window)
{
    bool itemHasChanged = false;

    //Keyboard
    if((_input->isKeyPressed("LEFT") || _input->isKeyPressed("RIGHT")) && !_keyPressed)
    {
        _selectedItem += _input->isKeyPressed("LEFT") ? -1 : 1;
        itemHasChanged = true;
        _keyPressed = true;
        if(_input->isKeyPressed("LEFT"))
            _leftArrow.setPosition(Vector2f(_pos.x - _shapeSize.x/2, _pos.y));
        else
            _rightArrow.setPosition(Vector2f(_pos.x + _shapeSize.x/2, _pos.y));
    }
    else if(!_input->isKeyPressed("LEFT") && !_input->isKeyPressed("RIGHT") && !Mouse::isButtonPressed(Mouse::Left))
        _keyPressed = false;

    //Mouse
    if(Mouse::isButtonPressed(Mouse::Left) && !_keyPressed)
    {
        _keyPressed = true;
        Vector2f mousePos = window->mapPixelToCoords(sf::Mouse::getPosition(*window));
        if(_leftArrow.getGlobalBounds().contains(mousePos))
        {
            _selectedItem --;
            itemHasChanged = true;
            _leftArrow.setPosition(Vector2f(_pos.x - _shapeSize.x/2, _pos.y));
        }
        else if(_rightArrow.getGlobalBounds().contains(mousePos))
        {
            _selectedItem ++;
            itemHasChanged = true;
            _rightArrow.setPosition(Vector2f(_pos.x + _shapeSize.x/2, _pos.y));
        }
    }

    //Loop
    if(_selectedItem < 0)
        _selectedItem = _maxItemIndex;
    else if(_selectedItem > _maxItemIndex)
        _selectedItem = 0;

    //Animation
    if(itemHasChanged)
        _items[_selectedItem]->setScale(1.35,1.35);
}

void menuSelector::SetSelectedItem(int i)
{
    if(i >= 0 && i <= _maxItemIndex)
        _selectedItem = i;
}

int menuSelector::GetSelectedItem()
{
    return _selectedItem;
}
void menuSelector::SetShapeColor(Color fillColor, Color outlineColor)
{
    _backShape.setFillColor(fillColor);
    _backShape.setOutlineColor(outlineColor);
}

void menuSelector::PerformItemScale()
{
    _items[_selectedItem]->setScale(1.5,1.5);
}

void menuSelector::LockSelection(bool lock)
{
    _lockSelection = lock;
}

void menuSelector::PerformSelectedItem()
{
    //Select item
    for(int i=0 ; i<_items.size() ; i++)
    {
        int alpha = i == _selectedItem ? 255 : 0;
        _items[i]->setColor(Color(255,255,255,alpha));
        if(_items[i]->getScale().x > 1)
            _items[i]->setScale(_items[i]->getScale().y-0.1, _items[i]->getScale().y-0.05);
    }

    //Arrows animation
    if(_leftArrow.getPosition().x < _pos.x - _shapeSize.x/2 + 15)
        _leftArrow.move(Vector2f(1*_deltaTime,0));
    else
        _leftArrow.setPosition(Vector2f(_pos.x - _shapeSize.x/2 + 15, _pos.y));

    if(_rightArrow.getPosition().x > _pos.x + _shapeSize.x/2 - 15)
        _rightArrow.move(Vector2f(-1*_deltaTime,0));
    else
        _rightArrow.setPosition(_pos.x + _shapeSize.x/2 - 15, _pos.y);

    //Arrows scale and opacity
    float scaleLeft = _pos.x - _shapeSize.x/2 + 15 - _leftArrow.getPosition().x;
    float scaleRight = _rightArrow.getPosition().x - (_pos.x + _shapeSize.x/2 - 15);
    _leftArrow.setScale(1.8+scaleLeft/50, 0.8+scaleLeft/50);
    _leftArrow.setFillColor(Color(255,255,255,135+scaleLeft*8));
    _rightArrow.setScale(1.8+scaleRight/50, 0.8+scaleRight/50);
    _rightArrow.setFillColor(Color(255,255,255,135+scaleRight*8));
}

void menuSelector::Display(RenderWindow* window, float deltaTime, Sprite* backItemSprite, Sprite* backItemSprite2)
{
    _deltaTime = deltaTime;
    window->draw(_backShape);
    if(_backShape.getScale().x < 1)
    {
        _backShape.setScale(_backShape.getScale().x+0.2, 1);
    }
    else
    {
        PerformSelectedItem();
        if(!_lockSelection)
        {
            PerformInputs(window);
            window->draw(_leftArrow);
            window->draw(_rightArrow);
        }
        if(backItemSprite != NULL)
            window->draw(*backItemSprite);
        if(backItemSprite2 != NULL)
            window->draw(*backItemSprite2);
        for(int i=0 ; i<_items.size() ; i++)
            window->draw(*_items[i]);
    }
}
