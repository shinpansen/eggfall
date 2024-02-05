#include "menuList.hpp"

using namespace std;
using namespace sf;

menuList::menuList()
{
}

menuList::menuList(vector<string> items, Vector2f pos, input* input, bool showBackground, ALIGN alignment, menuList* menuParent)
{
    //Objects - variables
    _deltaTime = 1;
    _pos = pos;
    _items = items;
    _input = input;
    _selectedItem = 0;
    _maxItemIndex = items.size()-1;
    _keyPressed = false;
    _showBackground = showBackground;
	_alignment = alignment;
	_menuParent = menuParent;

    //Font
    _font.loadFromFile(files::font2);

    //Texts objects
    FloatRect textRect;
    _maxTextWidth = 0;
    for(int i=0 ; i<_items.size() ; i++)
    {
        _texts.push_back(new Text());
        _texts[_texts.size()-1]->setFont(_font);
		_texts[_texts.size() - 1]->setString(_items[i]);
        _texts[_texts.size()-1]->setCharacterSize(36);
        textRect = _texts[_texts.size()-1]->getLocalBounds();
        if(textRect.width > _maxTextWidth)
            _maxTextWidth = textRect.width;

        //Alignment
        if(alignment == ALIGN_LEFT)
            _texts[_texts.size()-1]->setOrigin(0, textRect.height/2+5);
        else if(alignment == ALIGN_RIGHT)
            _texts[_texts.size()-1]->setOrigin(textRect.width, textRect.height/2+5);
        else
            _texts[_texts.size()-1]->setOrigin(textRect.width/2, textRect.height/2+5);
    }

    //Shape
    _shape.setSize(Vector2f(_maxTextWidth+60, (textRect.height+20)*_items.size()+20));
    _shape.setFillColor(Color(0,0,0));
    _shape.setOrigin(_shape.getSize().x/2.f, _shape.getSize().y/2.f);
	_shape.setPosition(_pos);

    //Menu Outline textures
    _topLeftTexture.loadFromFile(files::menuTopLeft);
    _topRightTexture.loadFromFile(files::menuTopRight);
    _bottomLeftTexture.loadFromFile(files::menuBottomLeft);
    _bottomRightTexture.loadFromFile(files::menuBottomRight);
    _middleBottomTexture.loadFromFile(files::menuMiddleBottom);
    _duplicateTexture.loadFromFile(files::menuDuplicate);
    _duplicateTexture.setRepeated(true);

    //Menu Outline sprites
    _topLeft.setTexture(_topLeftTexture);
    _topLeft.setOrigin(20,20);
    _topRight.setTexture(_topRightTexture);
    _topRight.setOrigin(71,20);
    _bottomLeft.setTexture(_bottomLeftTexture);
    _bottomLeft.setOrigin(8,0);
    _bottomRight.setTexture(_bottomRightTexture);
    _bottomRight.setOrigin(71,0);
    _middleBottom.setTexture(_middleBottomTexture);
    _middleBottom.setOrigin(10,0);
    _duplicateLeft.setTexture(_duplicateTexture);
    _duplicateLeft.setRotation(270);
    _duplicateRight.setTexture(_duplicateTexture);
    _duplicateRight.setRotation(90);
    _duplicateTop.setTexture(_duplicateTexture);
    _duplicateBottom.setTexture(_duplicateTexture);
    _duplicateBottom.setRotation(180);


	//Texts/shapes positions
	SetPosition(pos);

    //Menu Outline
    //ApplyMenuOutlines();

    //Menu animation
    ResetAnimation();
}

void menuList::ApplyMenuOutlines()
{
    //Menu Outline positions
    _topLeft.setPosition(_pos.x-_shape.getSize().x/2, _shape.getPosition().y-_shape.getSize().y/2);
    _topRight.setPosition(_pos.x+_shape.getSize().x/2, _shape.getPosition().y-_shape.getSize().y/2);
    _bottomLeft.setPosition(_pos.x-_shape.getSize().x/2, _shape.getPosition().y+_shape.getSize().y/2);
    _bottomRight.setPosition(_pos.x+_shape.getSize().x/2, _shape.getPosition().y+_shape.getSize().y/2);
    _middleBottom.setPosition(_pos.x, _shape.getPosition().y+_shape.getSize().y/2+12);

    //Menu duplicate
    _duplicateLeft.setTextureRect(IntRect(0,0,_shape.getSize().y, 12));
    _duplicateLeft.setPosition(_pos.x-_shape.getSize().x/2, _shape.getPosition().y+_shape.getSize().y/2);
    _duplicateRight.setTextureRect(IntRect(0,0,_shape.getSize().y, 12));
    _duplicateRight.setPosition(_pos.x+_shape.getSize().x/2+1, _shape.getPosition().y-_shape.getSize().y/2);
    _duplicateTop.setTextureRect(IntRect(0,0,_shape.getSize().x,12));
    _duplicateTop.setPosition(_pos.x-_shape.getSize().x/2, _shape.getPosition().y-_shape.getSize().y/2-12);
    _duplicateBottom.setTextureRect(IntRect(0,0,_shape.getSize().x,12));
    _duplicateBottom.setPosition(_pos.x+_shape.getSize().x/2, _shape.getPosition().y+_shape.getSize().y/2+12);
}

void menuList::ResetAnimation(bool resetItemPos)
{
    if(resetItemPos)
        _selectedItem = 0;
    PerformSelectedItem(true);
    _shape.setScale(0,0);
    _background.setSize(Vector2f(utils::VIEW_WIDTH,utils::VIEW_HEIGHT));
    _background.setOrigin(utils::VIEW_WIDTH / 2.f, utils::VIEW_HEIGHT / 2.f);
    _background.setPosition(utils::VIEW_WIDTH / 2.f, utils::VIEW_HEIGHT / 2.f);
    _background.setFillColor(Color(0,0,0,150));
    _background.setScale(0,1);
}

void menuList::PerformInputs(RenderWindow* window)
{
    //Keyboard
    if(!_input->GetMouseController())
    {
        if((_input->isKeyPressed(CMD_UP) || _input->isKeyPressed(CMD_DOWN)) && !_keyPressed)
        {
            _selectedItem += _input->isKeyPressed(CMD_DOWN) ? 1 : -1;
            _keyPressed = true;
            if(_selectedItem >= 0 && _selectedItem <= _maxItemIndex)
                _texts[_selectedItem]->setScale(1.5, 1.5);
            else
                _texts[_selectedItem < 0 ? _maxItemIndex : 0]->setScale(1.5, 1.5);
        }
        else if(!_input->isKeyPressed(CMD_UP) && !_input->isKeyPressed(CMD_DOWN))
            _keyPressed = false;

        //Loop
        if(_selectedItem < 0)
            _selectedItem = _maxItemIndex;
        else if(_selectedItem > _maxItemIndex)
            _selectedItem = 0;
    }
    else
    {
        //Mouse
        _selectedItem = -1;
        Vector2f mousePos = window->mapPixelToCoords(sf::Mouse::getPosition(*window));
        for(int i=0 ; i<=_maxItemIndex ; i++)
        {
            FloatRect textRect = _texts[i]->getGlobalBounds();
            if(textRect.contains(mousePos))
            {
               _selectedItem = i;
               break;
            }
        }
    }
}

int menuList::GetSelectedItem()
{
    return _selectedItem;
}

string menuList::GetSelectedText()
{
	return _selectedItem >= 0 && _selectedItem < _texts.size() ? _texts[_selectedItem]->getString() : "";
}

void menuList::FocusText(string text)
{
    for(int i=0 ; i<_texts.size() ; i++)
    {
        if(_texts[i]->getString() == text)
        {
            _selectedItem = i;
            PerformSelectedItem();
            break;
        }
    }
}

bool menuList::MouseOnSelectedText(RenderWindow* window)
{
    if(_selectedItem < 0)
        return false;

    Vector2f mousePos = window->mapPixelToCoords(sf::Mouse::getPosition(*window));
    FloatRect textRect = _texts[_selectedItem]->getGlobalBounds();
	return textRect.contains(mousePos);
}

Vector2f menuList::GetPosition()
{
	return _shape.getPosition();
}

void menuList::SetPosition(Vector2f pos, ALIGN alignment, BG showBackground)
{
	//Alignment
	if (alignment != ALIGN_NOCHANGE)
		_alignment = alignment;

	//Position
	_pos = pos;
	FloatRect textRect = _texts[_texts.size() - 1]->getLocalBounds();
	_shape.setPosition(pos);
	int topPos = pos.y - (_shape.getSize().y / 2);
	_texts[_selectedItem]->setScale(1.24, 1.24);
	for (int i = 0; i<_texts.size(); i++)
	{
		if (_alignment == ALIGN_LEFT)
			_texts[i]->setPosition(pos.x - _maxTextWidth / 2, topPos + 23 + (i*(textRect.height + 20)));
		else if (_alignment == ALIGN_RIGHT)
			_texts[i]->setPosition(pos.x + _maxTextWidth / 2, topPos + 23 + (i*(textRect.height + 20)));
		else
			_texts[i]->setPosition(pos.x, topPos + 23 + (i*(textRect.height + 20)));
	}
	ApplyMenuOutlines();

	//Background
	if(showBackground != BG_NOCHANGE)
		_showBackground = showBackground == BG_SHOW ? true : false;
}

bool menuList::GetBackgroundVisibility()
{
	return _showBackground;
}

bool menuList::IsMouseOutsideMenu(RenderWindow* window)
{
	Vector2f mousePos = window->mapPixelToCoords(sf::Mouse::getPosition(*window));
	return !_shape.getGlobalBounds().contains(mousePos);
}

void menuList::ShowBackground(bool show)
{
	_showBackground = show;
}

void menuList::ForceKeyPressed(bool force)
{
	_keyPressed = force;
}

void menuList::PerformSelectedItem(bool forceFocus)
{
    for(int i=0 ; i<_texts.size() ; i++)
    {
        if(i > _maxItemIndex)
            return;

        _texts[i]->setFillColor(i == _selectedItem ? Color(255,205,0) : Color(255,255,255));
        if(i == _selectedItem && _texts[i]->getScale().x > 1.24)
            _texts[i]->setScale(_texts[i]->getScale().x-0.02*_deltaTime, _texts[i]->getScale().y-0.02*_deltaTime);
        else if (i != _selectedItem)
            _texts[i]->setScale(1,1);
    }

    if(forceFocus && _selectedItem >= 0)
        _texts[_selectedItem]->setScale(1.24,1.24);
}

void menuList::Display(RenderWindow* window, float deltaTime, bool step, bool borders)
{
    _deltaTime = deltaTime;

	//Background
    if(_showBackground)
        window->draw(_background);

	//Moving to parent menu
	if (_menuParent != NULL && _menuParent->GetPosition() != _shape.getPosition())
	{
		Vector2f pos = _menuParent->GetPosition();
		this->SetPosition(pos);
	}

	//Transition
    window->draw(_shape);
    if(_shape.getScale().x < 1)
    {
        _shape.setScale(_shape.getScale().x+(0.1*_deltaTime), _shape.getScale().y+(0.1*_deltaTime));
        _background.setScale(_background.getScale().x+(0.1*_deltaTime), 1);
    }
    else //Transition finished
    {
        _shape.setScale(1,1);
        _background.setScale(1,1);

        if(step)
        {
            PerformInputs(window);
            PerformSelectedItem();
        }
        if(borders)
        {
            window->draw(_duplicateLeft);
            window->draw(_duplicateRight);
            window->draw(_duplicateTop);
            window->draw(_duplicateBottom);
            window->draw(_topLeft);
            window->draw(_topRight);
            window->draw(_bottomLeft);
            window->draw(_bottomRight);
            window->draw(_middleBottom);
        }

        for(int i=0 ; i<_texts.size() ; i++)
            window->draw(*_texts[i]);
    }
}
