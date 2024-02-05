#include "keyboardKey.hpp"

using namespace std;
using namespace sf;

keyboardKey::keyboardKey(string text, Vector2f pos, bool transparent)
{
    //Variables
    _pos = pos;

   //Texts
   _font.loadFromFile(_files.font2);
   _text.setFont(_font);
   _text.setCharacterSize(22);
   _text.setColor(Color(0,0,0));
   _text.setString(text.size() == 1 ? " " + text + " " : text);
   _text.setOrigin(_text.getLocalBounds().width/2,_text.getLocalBounds().height);
   _text.setPosition(_pos.x, _pos.y-1);

   //Textures
   _keyboardKeyTexture.loadFromFile(_files.keyboardKey);
   _keyboardKeyMiddleTexture.loadFromFile(_files.keyboardKeyMiddle);
   _keyboardKeyMiddleTexture.setRepeated(true);

   //Sprites
   _keyMiddle.setTexture(_keyboardKeyMiddleTexture);
   _keyMiddle.setTextureRect(IntRect(0,0,_text.getLocalBounds().width,29));
   _keyMiddle.setOrigin(_text.getLocalBounds().width/2, 14.5);
   _keyMiddle.setPosition(_pos);
   _keyLeft.setTexture(_keyboardKeyTexture);
   _keyLeft.setTextureRect(IntRect(0,0,7,29));
   _keyLeft.setOrigin(7,14.5);
   _keyLeft.setPosition(_pos.x-_text.getLocalBounds().width/2,_pos.y);
   _keyRight.setTexture(_keyboardKeyTexture);
   _keyRight.setTextureRect(IntRect(14,0,7,29));
   _keyRight.setOrigin(0,14.5);
   _keyRight.setPosition(_pos.x+_text.getLocalBounds().width/2,_pos.y);

   //Transparent
   if(transparent)
   {
       _text.setColor(Color(0,0,0,100));
       _keyMiddle.setColor(Color(255,255,255,100));
       _keyRight.setColor(Color(255,255,255,100));
       _keyRight.setColor(Color(255,255,255,100));
   }
}

void keyboardKey::SetString(string text)
{
   _text.setString(text);
   _text.setOrigin(_text.getLocalBounds().width/2,_text.getLocalBounds().height);
   _text.setPosition(_pos.x, _pos.y-1);
   _keyMiddle.setTextureRect(IntRect(0,0,_text.getLocalBounds().width,29));
   _keyMiddle.setOrigin(_text.getLocalBounds().width/2, 14.5);
   _keyMiddle.setPosition(_pos);
   _keyLeft.setPosition(_pos.x-_text.getLocalBounds().width/2,_pos.y);
   _keyRight.setPosition(_pos.x+_text.getLocalBounds().width/2,_pos.y);
}

void keyboardKey::SetPosition(Vector2f pos)
{
    _pos = pos;
    _text.setPosition(_pos.x, _pos.y-1);
    _keyMiddle.setPosition(_pos);
    _keyLeft.setPosition(_pos.x-_text.getLocalBounds().width/2,_pos.y);
    _keyRight.setPosition(_pos.x+_text.getLocalBounds().width/2,_pos.y);
}

void keyboardKey::SetTransparent(bool transparent)
{
    if(transparent)
    {
        _text.setColor(Color(0,0,0,100));
        _keyMiddle.setColor(Color(255,255,255,100));
        _keyLeft.setColor(Color(255,255,255,100));
        _keyRight.setColor(Color(255,255,255,100));
    }
    else
    {
        _text.setColor(Color(0,0,0));
        _keyMiddle.setColor(Color(255,255,255));
        _keyLeft.setColor(Color(255,255,255));
        _keyRight.setColor(Color(255,255,255));
    }
}

Vector2f keyboardKey::GetPosition()
{
    return _pos;
}

string keyboardKey::GetText()
{
    return _text.getString();
}

int keyboardKey::GetWidth()
{
    return _keyMiddle.getLocalBounds().width + _keyLeft.getLocalBounds().width*2;
}

void keyboardKey::Display(RenderWindow* window)
{
    window->draw(_keyMiddle);
    window->draw(_keyLeft);
    window->draw(_keyRight);
    window->draw(_text);
}
