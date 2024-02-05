#include "heroSelector.hpp"

using namespace std;
using namespace sf;

heroSelector::heroSelector()
{
}

heroSelector::heroSelector(Vector2f pos, input* input, string language, Texture* wizardSelect,
                           Texture* maoSun, vector<bool>* forbiddenId, int firstItem, sound* soundBox)
{
    //Variables - objects
    _keyPressed = true;
    _pressTimeOut = 60;
    _sunFrame = 0;
    _language = language;
    _pos = pos;
    _selectState = wait;
    _input = input;
    _soundBox = soundBox;
    _wizardSelectTexture = wizardSelect;
    _maoSunTexture = maoSun;
    _forbiddenId = forbiddenId;

    //Graphics
    _hatSelectorTexture.loadFromFile(_files.hatSelector);
    _padKeysTexture.loadFromFile(_files.pad360Keys);
    _hat.setTexture(_hatSelectorTexture);
    _hat.setOrigin(_hat.getLocalBounds().width/2, _hat.getLocalBounds().height/2);
    _hat.setPosition(Vector2f(_pos.x, _pos.y-50));
    _padKey.setTexture(_padKeysTexture);
    _padKey.setTextureRect(IntRect(0,0,29,29));
    _padKey.setOrigin(_padKey.getLocalBounds().width/2, _padKey.getLocalBounds().height/2);
    _maoSun.setTexture(*_maoSunTexture);
    _maoSun.setTextureRect(IntRect(0,0,180,230));
    _maoSun.setOrigin(90,115);
    _maoSun.setPosition(_pos);
    _maoSun2 = _maoSun;
    _maoSun2.setTextureRect(IntRect(60*180,0,180,230));
    for(int i=0 ; i<5 ; i++)
    {
        _wizardMenuItems.push_back(new Sprite);
        _wizardMenuItems[i]->setTexture(*_wizardSelectTexture);
        _wizardMenuItems[i]->setTextureRect(IntRect(i*108,0,108,138));
        _wizardMenuItems[i]->setOrigin(54,69);
    }

    //Shapes
    _joinShape.setSize(Vector2f(180,230));
    _joinShape.setOrigin(90, 115);
    _joinShape.setFillColor(Color(10,10,10));
    _joinShape.setOutlineColor(Color(0,0,0));
    _joinShape.setOutlineThickness(5);
    _joinShape.setPosition(_pos);

    //Colors
    _colors = new Color[5];
    _colors[0] = Color(0,40,40);
    _colors[1] = Color(50,0,0);
    _colors[2] = Color(0,50,0);
    _colors[3] = Color(40,0,40);
    _colors[4] = Color(40,40,40);

    //Texts
    _font.loadFromFile(_files.font2);
    _font2.loadFromFile(_files.font1);
    _textPress.setFont(_font);
    _textPress.setCharacterSize(35);
    _textPress.setString(xmlReader::getTranslationText("key2", _language, "PRESS"));
    _textPress.setOrigin(_textPress.getLocalBounds().width/2, _textPress.getLocalBounds().height/2);
    _textPress.setPosition(Vector2f(_pos.x, _pos.y+40));
    _textReady.setFont(_font);
    _textReady.setCharacterSize(45);
    _textReady.setString(xmlReader::getTranslationText("miscellaneous", _language, "Ready"));
    _textReady.setOrigin(_textReady.getLocalBounds().width/2, _textReady.getLocalBounds().height/2);
    _textReady.setPosition(Vector2f(_pos.x, _pos.y+140));
    _textReady.setScale(0,0);
    _textKey.setFont(_font2);
    _textKey.setCharacterSize(40);
    ResetSelector();

    //Menu selector
    _menuSelector = new menuSelector(Vector2f(180,230), _pos, _wizardMenuItems, firstItem, _input, true);
}

void heroSelector::ResetSelector()
{
    //Update join key text
    stringstream stream;
    stream << _input->GetPlayerNumber()+1;
    string keyString = xmlReader::getKeyValue(stream.str(), "JUMP");
    Keyboard::Key key = (Keyboard::Key)atoi(keyString.c_str());
    _key = new keyboardKey(_input->GetKeyName(key), Vector2f(_pos.x+28, _pos.y+90));
    _padKey.setPosition(Vector2f(_key->GetPosition().x-_key->GetWidth()/2-45, _pos.y+90));
    _textKey.setString("/");
    _textKey.setOrigin(_textKey.getLocalBounds().width/2, _textKey.getLocalBounds().height/2);
    _textKey.setPosition(Vector2f(_padKey.getPosition().x+30, _pos.y+73));

    //Reset
    _keyPressed = true;
    _selectState = wait;
    _joinShape.setScale(0,1);
    _hat.setScale(0,1);
    for(int i=0 ; i<_forbiddenId->size() ; i++)
        (*_forbiddenId)[i] = false;
}

void heroSelector::PerformInputs(RenderWindow* window)
{
    switch(_selectState)
    {
        case wait:
            if((_input->isKeyPressed("JUMP") || WizardClicked(window, true)) && !_keyPressed)
            {
                _keyPressed = true;
                _soundBox->PlaySound(0, false);
                _selectState = join;
            }
            break;
        case join:
            if((_input->isKeyPressed("JUMP") || WizardClicked(window, true)) && !_keyPressed)
            {
                _keyPressed = true;
                _soundBox->PlaySound(_menuSelector->GetSelectedItem()+1, false);
                if(!(*_forbiddenId)[_menuSelector->GetSelectedItem()])
                {
                    _selectState = select;
                    _textReady.setScale(1.8,1.8);
                    _menuSelector->PerformItemScale();
                    (*_forbiddenId)[_menuSelector->GetSelectedItem()] = true;
                }
            }
            else if((_input->isKeyPressed("ROLL") || WizardClicked(window, false)) && !_keyPressed)
            {
                _keyPressed = true;
                _soundBox->PlaySound(8, false);
                _selectState = wait;
                _menuSelector->ResetAnimation();
            }
            break;
        case select:
            if((_input->isKeyPressed("ROLL") || WizardClicked(window, false)) && !_keyPressed)
            {
                _keyPressed = true;
                _soundBox->PlaySound(8, false);
                _selectState = join;
                (*_forbiddenId)[_menuSelector->GetSelectedItem()] = false;
            }
            break;
    }

    if(!_input->isKeyPressed("JUMP") && !_input->isKeyPressed("ROLL") &&
       !Mouse::isButtonPressed(Mouse::Left) && !Mouse::isButtonPressed(Mouse::Right))
        _keyPressed = false;
}

void heroSelector::PerformAnimations()
{
    //Update timeout
    if(_pressTimeOut > 0)
        _pressTimeOut -= _deltaTime;
    else
        _pressTimeOut = 60;

    //Showing / hiding join texts
    Color color = _pressTimeOut > 15 ? Color(255,255,255) : Color(255,255,255,0);
    _textPress.setColor(color);
    _textKey.setColor(color);
    _padKey.setColor(color);
}

void heroSelector::SetSelectedItem(int i)
{
    _menuSelector->SetSelectedItem(i);
}

int heroSelector::GetSelectedItem()
{
    return _menuSelector->GetSelectedItem();
}

string heroSelector::GetKeyText()
{
    return _key->GetText();
}

bool heroSelector::WizardClicked(RenderWindow* window, bool left)
{
    if(!Mouse::isButtonPressed(left ? Mouse::Left : Mouse::Right))
        return false;

    Vector2f mousePos = window->mapPixelToCoords(sf::Mouse::getPosition(*window));
    for(int i=0 ; i<_wizardMenuItems.size() ; i++)
        if(_wizardMenuItems[i]->getGlobalBounds().contains(mousePos))
            return true;

    return false;
}

bool heroSelector::IsWaiting()
{
    return _selectState == wait;
}

bool heroSelector::HasSelected()
{
    return _selectState == select;
}

void heroSelector::Display(RenderWindow* window, float deltaTime)
{
    _deltaTime = deltaTime;
    PerformInputs(window);
    PerformAnimations();
    switch(_selectState)
    {
        case wait:
            window->draw(_joinShape);
            window->draw(_hat);
            if(_joinShape.getScale().x < 1)
            {
                _joinShape.setScale(_joinShape.getScale().x+0.2,1);
                _hat.setScale(_joinShape.getScale());
            }
            else
            {
                window->draw(_textPress);
                if(_pressTimeOut > 15)
                    _key->Display(window);
                window->draw(_textKey);
                window->draw(_padKey);
            }
            break;
        case join:
            if(_joinShape.getScale().x > 0)
            {
                window->draw(_joinShape);
                window->draw(_hat);
                _joinShape.setScale(_joinShape.getScale().x-0.2,1);
                _hat.setScale(_joinShape.getScale());
            }
            else
            {
                for(int i=0 ; i<5 ; i++)
                    _wizardMenuItems[i]->setTextureRect(IntRect(i*108,0,108,138));
                _menuSelector->SetShapeColor(Color(33,33,33), Color(50,50,50));
                _menuSelector->LockSelection(false);
                _menuSelector->Display(window, deltaTime);
            }
            break;
        case select:
            //Lightening sun
            Color c = _colors[_menuSelector->GetSelectedItem()];
            _sunFrame = _sunFrame < 89 ? _sunFrame+0.5*_deltaTime : 0;
            _maoSun.setColor(Color(c.r*5,c.g*5,c.b*5,80));
            _maoSun.setTextureRect(IntRect((int)_sunFrame*180,0,180,230));
            _maoSun2.setColor(Color(c.r*5,c.g*5,c.b*5,80));
            _maoSun2.setTextureRect(IntRect((89-(int)_sunFrame)*180,0,180,230));

            //Drawing
            for(int i=0 ; i<5 ; i++)
                _wizardMenuItems[i]->setTextureRect(IntRect(i*108,138,108,138));
            int id = _menuSelector->GetSelectedItem();
            _menuSelector->SetShapeColor(_colors[id], Color(_colors[id].r*1.5, _colors[id].g*1.5, _colors[id].b*1.5));
            _menuSelector->LockSelection(true);
            _menuSelector->Display(window, deltaTime, &_maoSun, &_maoSun2);
            window->draw(_textReady);
            if(_textReady.getScale().x > 1)
                _textReady.setScale(_textReady.getScale().x-0.1, _textReady.getScale().y-0.1);
            break;
    }
}
