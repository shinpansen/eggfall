#include "menuHeroSelector.hpp"

using namespace std;
using namespace sf;

menuHeroSelector::menuHeroSelector(string language, vector<input*> inputs, sound* soundBox)
{
    //Variables - Objects
    _language = language;
    _inputs = inputs;
    _frameText = 0;
    _upScaleFightText = true;
    _soundBox = soundBox;

    //Graphics
    _wizardSelectTexture.loadFromFile(_files.wizardSelect);
    _maoSunTexture.loadFromFile(_files.maoSun);
    _crossTexture.loadFromFile(_files.cross);
    _padKeysTexture.loadFromFile(_files.pad360Keys);
    _crosses = new Sprite[4];
    for(int i=0 ; i<4 ; i++)
    {
        _crosses[i].setTexture(_crossTexture);
        _crosses[i].setOrigin(_crosses[i].getLocalBounds().width/2, _crosses[i].getLocalBounds().height/2);
        _crosses[i].setPosition(Vector2f(106+i*196, 310));
    }
    _padKey.setTexture(_padKeysTexture);
    _padKey.setTextureRect(IntRect(0,0,29,29));
    _padKey.setOrigin(_padKey.getLocalBounds().width/2, _padKey.getLocalBounds().height/2);

    //Hero selectors
    for(int i=0 ; i<4 ; i++)
        _heroSelectors.push_back(new heroSelector(Vector2f(106+i*196,310), _inputs[i], _language, &_wizardSelectTexture, &_maoSunTexture, &_forbiddenId, i, _soundBox));

    //Forbidden id, when player select a wizard
    for(int i=0 ; i<5 ; i++)
        _forbiddenId.push_back(false);

    //Texts
    _font.loadFromFile(_files.font2);
    _waitingJoin.setFont(_font);
    _waitingJoin.setCharacterSize(40);
    _waitingJoin.setString(xmlReader::getTranslationText("miscellaneous", _language, "waitingJoin"));
    _waitingJoin.setOrigin(_waitingJoin.getLocalBounds().width/2,_waitingJoin.getLocalBounds().height/2+10);
    _waitingJoin.setPosition(400,540);
    _waitingJoin.setColor(Color(255,201,14,150));
    _fightText = _waitingJoin;
    _fightText.setString(xmlReader::getTranslationText("miscellaneous", _language, "fight"));
    _fightText.setOrigin(_fightText.getLocalBounds().width/2,_fightText.getLocalBounds().height/2+12);
    _fightText.setPosition(400,535);
    _fightText.setColor(Color(255,255,255));
    _fightText.setScale(1,1);

    //Shape
    _fightShape.setSize(Vector2f(900,50));
    _fightShape.setFillColor(Color(163,0,161));
    _fightShape.setOutlineColor(Color(207,4,205));
    _fightShape.setOutlineThickness(5);
    _fightShape.setOrigin(450,25);
    _fightShape.setScale(1,0);
    _fightShape.setPosition(400,540);

    //Keys
    _keys.push_back(new keyboardKey(_heroSelectors[0]->GetKeyText(), Vector2f(0,0)));
    _keys.push_back(new keyboardKey(_heroSelectors[1]->GetKeyText(), Vector2f(0,0)));
    _keys.push_back(new keyboardKey(_heroSelectors[2]->GetKeyText(), Vector2f(0,0)));
    _keys.push_back(new keyboardKey(_heroSelectors[3]->GetKeyText(), Vector2f(0,0)));

    //Keys position
    ManageKeyPosition();
}

void menuHeroSelector::ResetMenu()
{
    for(int i=0 ; i<4 ; i++)
    {
        _heroSelectors[i]->ResetSelector();
        _keys[i]->SetString(_heroSelectors[i]->GetKeyText());
    }
    ManageKeyPosition();
}

void menuHeroSelector::ManageKeyPosition()
{
    float width = _fightText.getLocalBounds().width;
    _keys[1]->SetPosition(Vector2f(_fightText.getPosition().x-width/2-_keys[1]->GetWidth()/2-35,540));
    _keys[0]->SetPosition(Vector2f(_keys[1]->GetPosition().x-_keys[1]->GetWidth()/2-_keys[0]->GetWidth()/2-20,540));
    _keys[2]->SetPosition(Vector2f(_fightText.getPosition().x+width/2+_keys[2]->GetWidth()/2+35,540));
    _keys[3]->SetPosition(Vector2f(_keys[2]->GetPosition().x+_keys[2]->GetWidth()/2+_keys[3]->GetWidth()/2+20,540));
    _padKey.setPosition(Vector2f(_keys[0]->GetPosition().x-_keys[0]->GetWidth()/2-15-20,540));
}

bool menuHeroSelector::PlayerWaiting(int numPlayer)
{
    if(numPlayer >= 1 && numPlayer <= 4)
        return _heroSelectors[numPlayer-1]->IsWaiting();
    else
        return false;
}

bool menuHeroSelector::PlayerReady(int numPlayer)
{
    if(numPlayer >= 1 && numPlayer <= 4)
        return _heroSelectors[numPlayer-1]->HasSelected();
    else
        return false;
}

bool menuHeroSelector::MinTwoPlayerOK()
{
    int playersReady = 0;
    for(int i=0 ; i<4 ; i++)
        if(_heroSelectors[i]->HasSelected())
            playersReady++;

    return playersReady >= 2;
}

int menuHeroSelector::GetSelectedItem(int numPlayer)
{
    if(numPlayer >= 1 && numPlayer <= 4)
        return _heroSelectors[numPlayer-1]->GetSelectedItem();
    else
        return 0;
}

void menuHeroSelector::TextAnimation()
{
    //Waiting text
    _frameText++;
    if(_frameText > 60)
        _frameText = 0;

    Color color = _frameText > 15 ? Color(255,201,14,150) : Color(255,201,14,0);
    _waitingJoin.setColor(color);

    //Fight text
    if(_fightText.getScale().x <= 1)
        _upScaleFightText = true;
    else if(_fightText.getScale().x >= 1.2)
        _upScaleFightText = false;

    float scaleValue = _upScaleFightText ? 0.02 : -0.02;
    _fightText.setScale(_fightText.getScale().x+scaleValue,_fightText.getScale().y+scaleValue);
}

void menuHeroSelector::Display(RenderWindow* window, float deltaTime)
{
    //Menus
    for(int i=0 ; i<4 ; i++)
        _heroSelectors[i]->Display(window, deltaTime);

    //Crosses
    for(int i=0 ; i<_heroSelectors.size() ; i++)
        if(!_heroSelectors[i]->HasSelected() && !_heroSelectors[i]->IsWaiting() &&
           _forbiddenId[_heroSelectors[i]->GetSelectedItem()])
            window->draw(_crosses[i]);

    //Texts
    TextAnimation();
    if(!MinTwoPlayerOK())
    {
        _fightShape.setScale(1,0);
        window->draw(_waitingJoin);
    }
    else
    {
        window->draw(_fightShape);
        if(_fightShape.getScale().y < 1)
            _fightShape.setScale(1, _fightShape.getScale().y+0.1);
        else
        {
            window->draw(_fightText);
            window->draw(_padKey);
            for(int i=0 ; i<_keys.size() ; i++)
            {
                _keys[i]->SetTransparent(!_heroSelectors[i]->HasSelected());
                _keys[i]->Display(window);
            }
        }
    }
}
