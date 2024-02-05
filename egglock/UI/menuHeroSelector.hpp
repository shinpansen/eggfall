#ifndef menuHeroSelector_HPP
#define menuHeroSelector_HPP

#include <iostream>
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>
#include "heroSelector.hpp"
#include "menuList.hpp"
#include "keyboardKey.hpp"
#include "../tools/files.hpp"
#include "../tools/input.hpp"
#include "../tools/xmlReader.hpp"
#include "../effects/sound.hpp"

using namespace std;
using namespace sf;

class menuHeroSelector
{
public:
    menuHeroSelector(string language, vector<input*> inputs, sound* soundBox);
    void ResetMenu();
    void ManageKeyPosition();
    bool PlayerWaiting(int numPlayer);
    bool PlayerReady(int numPlayer);
    bool MinTwoPlayerOK();
    int GetSelectedItem(int numPlayer);
    void TextAnimation();
    void Display(RenderWindow* window, float deltaTime);
private:
    //Variables - Objects
    string _language;
    int _frameText;
    bool _upScaleFightText;
    files _files;
    sound* _soundBox;
    vector<heroSelector*> _heroSelectors;
    vector<input*> _inputs;
    vector<bool> _forbiddenId;

    //Graphics
    Texture _wizardSelectTexture;
    Texture _maoSunTexture;
    Texture _crossTexture;
    Texture _padKeysTexture;
    Sprite* _crosses;
    Sprite _padKey;
    RectangleShape _fightShape;

    //Texts
    Font _font;
    Text _waitingJoin;
    Text _fightText;

    //UI
    menuList* _menuFight;
    keyboardKey* _keyPlayer1;
    keyboardKey* _keyPlayer2;
    keyboardKey* _keyPlayer3;
    keyboardKey* _keyPlayer4;
    vector<keyboardKey*> _keys;
};
#endif
