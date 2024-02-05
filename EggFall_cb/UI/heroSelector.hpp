#ifndef heroSelector_HPP
#define heroSelector_HPP

#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>
#include "menuSelector.hpp"
#include "../tools/input.hpp"
#include "../tools/files.hpp"
#include "../tools/xmlReader.hpp"
#include "../effects/sound.hpp"
#include "keyboardKey.hpp"

using namespace std;
using namespace sf;

enum MENU_STATE
{
	STATE_WAIT,
	STATE_JOIN,
	STATE_SELECT
};

class heroSelector
{
public:
    heroSelector();
    heroSelector(Vector2f pos, input* input, string language, Texture* wizardSelect,
                 Texture* maoSun, vector<bool>* forbiddenId, int firstItem, sound* soundBox);
    void ResetSelector();
    void PerformInputs(RenderWindow* window);
    void PerformAnimations();
    void SetSelectedItem(int i);
    int GetSelectedItem();
    string GetKeyText();
    bool WizardClicked(RenderWindow* window, bool left);
    bool IsWaiting();
    bool HasSelected();
    void Display(RenderWindow* window, float deltaTime);
private:
    //Variables
    float _deltaTime;
    float _pressTimeOut;
    float _sunFrame;
    bool _keyPressed;
    string _language;
    Vector2f _pos;
	MENU_STATE _selectState;
    vector<bool>* _forbiddenId;

    //Graphics
    Texture _hatSelectorTexture;
    Texture _padKeysTexture;
    Texture* _wizardSelectTexture;
    Texture* _maoSunTexture;
    Sprite _hat;
    Sprite _padKey;
    Sprite _maoSun;
    Sprite _maoSun2;
    vector<Sprite*> _wizardMenuItems;
    RectangleShape _joinShape;
    Color* _colors;
    keyboardKey* _key;

    //Texts
    Font _font;
    Font _font2;
    Text _textPress;
    Text _textKey;
    Text _textReady;

    //Objects
    input* _input;
    menuSelector* _menuSelector;
    sound* _soundBox;
};
#endif
