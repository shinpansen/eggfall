#ifndef mainGame_HPP
#define mainGame_HPP

#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <Box2D/Box2D.h>

#include "../tools/Box2DTools.hpp"
#include "../tools/input.hpp"
#include "../tools/camera.hpp"
#include "../tools/xmlReader.hpp"
#include "../tools/settings.hpp"
#include "../effects/effects.hpp"
#include "../effects/spells.hpp"
#include "../effects/sound.hpp"
#include "../characters/hero.hpp"
#include "../environment/maps.hpp"
#include "../environment/tree.hpp"
#include "../objects/ladder.hpp"
#include "menuList.hpp"
#include "menuOptions.hpp"
#include "menuPad.hpp"
#include "menuSelector.hpp"
#include "menuHeroSelector.hpp"
#include "keyboardKey.hpp"
#include "scoreManager.hpp"

using namespace std;
using namespace sf;

enum gamestates
{
    intro,
    mainmenu,
    mainoptions,
    optionlanguage,
    optionscreen,
    optiontime,
    optionlife,
    optionMusic,
    optionEffects,
    maincontrols,
    keyconfig,
    showpad,
    heroSelect,
    game,
    gameEnded,
    pause,
    yesno
};

class mainGame
{
public:
    mainGame(RenderWindow* window, settings* settings);
    bool ValidMenu(menuList* menu);
    void OpeningClosingMenu();
    void ManageWizardCreation();
    void MainMenuEvents();
    void MainOptionsEvents();
    void OptionsEvents();
    void RedrawUI();
    void ChangeScreenMode();
    void MainControlsEvents();
    void KeyConfigEvents();
    void PauseMenuEvents();
    void YesNoMenuEvents();
    void DisplayGameResources();
    bool TitleAnimation();
    void MenuAnimation();
    bool GameEndedAnimation();
    bool IsMusicPaused();
    void PauseMusic(bool pause);
    void GoOn(Keyboard::Key key, bool keyPressed, float deltaTime);
private:
    //Variables
    gamestates _gameState;
    gamestates _yesnoLastGameState;
    int _titleAnimationFrame;
    float _deltaTime;
    Vector2f * _titleAnimationScales;
    bool _menuKeyPressed;
    bool _globalKeyPressed;
    bool _waitingKey;
    bool _showMasks;
    Keyboard::Key _currentKey;

    //Objects
    settings* _settings;
    RenderWindow* _window;
    ladder _ladderBox;
    files _files;
    scoreManager _scoreManager;

    //Box2D
    b2World* _world;
    maps* _maps;

    //Effects - Spells
    effects _effectBox;
    sound _soundBox;
    spells* _spellBox;

    //Graphics
    Texture _logoTexture;
    Texture _titleScreenBackgroundTexture;
    Texture _titleScreenTitleTexture;
    Texture _titleScreenHatsTexture;
    Texture _treeTexture;
    Sprite _logo;
    Sprite _titleScreenBackground;
    Sprite _titleScreenTitle;
    Sprite * _titleScreenHats;
    RectangleShape _transitionBlack;
    RectangleShape _transitionShapes[15][20];
    RectangleShape _yesnoShape;

    //Inputs
    input _input0;
    input _input1;
    input _input2;
    input _input3;
    input _input4;
    input* _editingInput;
    vector<input*> _inputs;
    int _editingInputNum;

    //Wizards
    hero* _wizard1;
    hero* _wizard2;
    hero* _wizard3;
    hero* _wizard4;
    vector<hero*> _wizards;

    //UI
    menuList* _mainMenu;
    menuList* _mainControls;
    menuOptions* _mainOptions;
    menuList* _optionLanguage;
    menuList* _optionScreen;
    menuList* _optionTime;
    menuList* _optionLife;
    menuList* _optionMusic;
    menuList* _optionEffects;
    menuOptions* _keyConfig;
    menuList* _pauseMenu;
    menuList* _yesNoMenu;
    menuPad* _padMenu;
    menuHeroSelector* _menuHeroSelector;

    //Texts
    Font _font;
    Text _heroSelectText;
    Text _resultText;
    Text _yesnoText;
};
#endif
