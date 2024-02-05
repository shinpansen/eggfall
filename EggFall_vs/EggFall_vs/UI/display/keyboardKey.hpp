#ifndef keyboardKey_HPP
#define keyboardKey_HPP

#include <iostream>
#include <vector>
#include <string>
#include <math.h>
#include <SFML/Graphics.hpp>
#include "../../tools/files.hpp"

using namespace std;
using namespace sf;

class keyboardKey
{
public:
    keyboardKey(string text, Vector2f pos, bool transparent = false);
    void SetString(string text);
    void SetPosition(Vector2f pos);
    void SetTransparent(bool transparent);
    Vector2f GetPosition();
    string GetText();
    int GetWidth();
    void Display(RenderWindow* window);
private:
    //Variables - Objects
    Vector2f _pos;

    //Text
    Font _font;
    Text _text;

    //Graphics
    Texture _keyboardKeyTexture;
    Texture _keyboardKeyMiddleTexture;
    Sprite _keyLeft;
    Sprite _keyMiddle;
    Sprite _keyRight;
};
#endif
