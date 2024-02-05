#ifndef mainGame_HPP
#define mainGame_HPP

#include <iostream>
#include <vector>
#include <math.h>
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
#include "../effects/sound.hpp"
#include "../projectiles/projectiles.hpp"
#include "../characters/wizard.hpp"
#include "../environment/maps.hpp"
#include "../environment/tree.hpp"
#include "../editor/editor.hpp";
#include "menuList.hpp"
#include "menuOptions.hpp"
#include "menuPad.hpp"
#include "menuSelector.hpp"
#include "menuHeroSelector.hpp"
#include "keyboardKey.hpp"
#include "scoreManager.hpp"

using namespace std;
using namespace sf;

enum GAMESTATES
{
    INTRO,
    MAIN_MENU,
    MAIN_OPTIONS,
	MAIN_COMMANDS,
    OPTION_LANG,
    OPTION_SCREEN,
    OPTION_TIME,
	OPTION_ANALOG,
	OPTION_LIFE,
    OPTION_MUSIC,
    OPTION_SFX,
    CONFIG_KEYS,
	SHOW_GAMEPAD,
    HERO_SELECT,
    GAME,
    GAME_END,
    PAUSE,
    YESNO,
	EDITOR
};

void ManagePlayerCreation(bool * isPlayersLoading, vector<hero*> * players, hero * player1, hero * player2, hero * player3, hero * player4,
	menuHeroSelector * menuHeroSelector, projectiles * projectileBox, settings * settings, scoreManager * scoreManager);

class mainGame
{
public:
    mainGame(RenderWindow* window, settings* settings);
    bool ValidMenu(menuList* menu);
    void OpeningClosingMenu();
    void MainMenuEvents();
    void MainOptionsEvents();
    void OptionsEvents();
    void MainControlsEvents();
    void KeyConfigEvents();
    void PauseMenuEvents();
    void YesNoMenuEvents();
	void RedrawUI();
	void ChangeScreenMode();
	void DisplayGameResources();
	void DisplaySettingsResources();
    bool TitleAnimation();
    void MenuAnimation();
    bool GameEndedAnimation();
    bool IsMusicPaused();
    void PauseMusic(bool pause);
    void Render(float deltaTime);
    void Display(Keyboard::Key key, bool keyPressed, float deltaTime);
private:
    //Variables
	GAMESTATES _gameState;
	GAMESTATES _lastGameState;
    int _titleAnimationFrame;
    float _deltaTime;
    Vector2f * _titleAnimationScales;
    bool _menuKeyPressed;
    bool _globalKeyPressed;
    bool _waitingKey;
    bool _showMasks;
    bool _showFps;
	bool _isPlayersLoading;
    Keyboard::Key _currentKey;
	contactListener * _contactListener;
	float _frameScreenshot;

    //Objects
    settings * _settings;
    RenderWindow * _window;
    scoreManager * _scoreManager;
	Thread * _threadLoadPlayers;

    //Box2D
    b2World * _world;
    maps * _maps;

    //Effects - projectiles
    effects _effectBox;
    sound _soundBox;
	projectiles * _projectileBox;

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
	vector<input*> _inputList;
    vector<input*> _inputs;
    int _editingInputNum;

    //Wizards
    hero * _player1;
    hero * _player2;
    hero * _player3;
    hero * _player4;
    vector<hero*> _players;

    //UI
    menuList * _mainMenu;
    menuList * _mainControls;
    menuOptions * _mainOptions;
    menuList * _optionLanguage;
    menuList * _optionScreen;
    menuList * _optionTime;
    menuList * _optionLife;
    menuList * _optionMusic;
    menuList * _optionEffects;
    menuOptions * _keyConfig;
    menuList * _pauseMenu;
    menuList * _yesNoMenu;
    menuPad * _padMenu;
    menuHeroSelector * _menuHeroSelector;
	menuList * _currentOptionMenu;

    //Texts
	Font _font;
	Font _fontB;
    Text _heroSelectText;
    Text _resultText;
    Text _yesnoText;
    Text _frameRate;
};
#endif
