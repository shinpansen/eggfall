#include "mainGame.hpp"

using namespace std;
using namespace sf;

mainGame::mainGame(RenderWindow* window, settings* settings)
{
    //Variables
    _gameState = mainmenu;
    _yesnoLastGameState = mainmenu;
    _deltaTime = 1;
    _menuKeyPressed = false;
    _globalKeyPressed = false;
    _waitingKey = false;
    _showMasks = false;
    _editingInputNum = 0;

    //Objects
    _settings = settings;
    _window = window;
    _soundBox.UpdateVolumes(_settings);

    //Title Animation
    _titleAnimationFrame = 0;
    _titleAnimationScales = new Vector2f[4];
    _titleAnimationScales[0] = Vector2f(1,1);
    _titleAnimationScales[1] = Vector2f(1.4,0.6);
    _titleAnimationScales[2] = Vector2f(0.6,1.4);
    _titleAnimationScales[3] = Vector2f(1,1);

    //Box2D - World
    _world = new b2World(b2Vec2(0.f, 42.f));
    _maps = new maps(_world, &_ladderBox, &_effectBox);

    //Spells - Music
    _spellBox = new spells(_world, &_effectBox, &_soundBox);
    _soundBox.PlayRandomMusic(0);

    //Graphics
    _logoTexture.loadFromFile(_files.logo);
    _titleScreenBackgroundTexture.loadFromFile(_files.titleScreenBackground);
    _titleScreenTitleTexture.loadFromFile(_files.titleScreenTitle);
    _titleScreenHatsTexture.loadFromFile(_files.titleScreenHats);
    _treeTexture.loadFromFile(_files.trees);
    _logo.setTexture(_logoTexture);
    _logo.setColor(Color(255,255,255,0));
    _titleScreenBackground.setTexture(_titleScreenBackgroundTexture);
    _titleScreenTitle.setTexture(_titleScreenTitleTexture);
    _titleScreenTitle.setOrigin(_titleScreenTitle.getLocalBounds().width/2, _titleScreenTitle.getLocalBounds().height/2);
    _titleScreenTitle.setPosition(400, 100);
    _titleScreenTitle.setScale(30,30);
    _titleScreenHats = new Sprite[5];
    for(int i=0 ; i<5 ; i++)
    {
        int width = i <= 0 ? 48 : 42;
        _titleScreenHats[i].setTexture(_titleScreenHatsTexture);
        _titleScreenHats[i].setTextureRect(IntRect(i*48, 0, width, 30));
        _titleScreenHats[i].setPosition(-264+(4-i)*52, 210);
    }

    //Inputs
    _input0 = input(-1, 30, _window); //Menu input
    _input1 = input(0, 30, _window);
    _input2 = input(1, 30, _window);
    _input3 = input(2, 30, _window);
    _input4 = input(3, 30, _window);

    //Wizards
    _wizard1 = new hero(0, Vector2i(185,145), _world, &_input1, &_effectBox, _spellBox, &_ladderBox, &_soundBox, &_scoreManager, 2);
    _wizard2 = new hero(1, Vector2i(615,145), _world, &_input2, &_effectBox, _spellBox, &_ladderBox, &_soundBox, &_scoreManager, 2);
    _wizard3 = new hero(2, Vector2i(115,416), _world, &_input3, &_effectBox, _spellBox, &_ladderBox, &_soundBox, &_scoreManager, 2);
    _wizard4 = new hero(3, Vector2i(685,416), _world, &_input4, &_effectBox, _spellBox, &_ladderBox, &_soundBox, &_scoreManager, 2);
    _wizards.push_back(_wizard1);
    _wizards.push_back(_wizard2);
    _wizards.push_back(_wizard3);
    _wizards.push_back(_wizard4);
    _spellBox->UpdateWizardsPointers(&_wizards);
    _wizard1->_wizards = _wizards;
    _wizard2->_wizards = _wizards;
    _wizard3->_wizards = _wizards;
    _wizard4->_wizards = _wizards;

    //UI
    _mainMenu = new menuList(xmlReader::getTranslationTexts("menuMain", _settings->_language), Vector2f(400,400), &_input0, false);
    _mainControls = new menuList(xmlReader::getTranslationTexts("menuControls", _settings->_language), Vector2f(400,400), &_input0, false);
    _mainOptions = new menuOptions(xmlReader::getTranslationTexts("settings", _settings->_language), Vector2f(400,360), &_input0, false);
    _optionLanguage = new menuList(xmlReader::getSettingsChoices(0), Vector2f(400,360), &_input0, true);
    _optionScreen = new menuList(xmlReader::getSettingsChoices(1), Vector2f(400,360), &_input0, true);
    _optionTime = new menuList(xmlReader::getSettingsChoices(2), Vector2f(400,360), &_input0, true);
    _optionLife = new menuList(xmlReader::getSettingsChoices(3), Vector2f(400,360), &_input0, true);
    _optionMusic = new menuList(xmlReader::getSettingsChoices(4), Vector2f(400,360), &_input0, true);
    _optionEffects = new menuList(xmlReader::getSettingsChoices(4), Vector2f(400,360), &_input0, true);
    _keyConfig = new menuOptions(xmlReader::getTranslationTexts("key", _settings->_language), Vector2f(400,355), &_input0, false);
    _pauseMenu = new menuList(xmlReader::getTranslationTexts("pause", _settings->_language), Vector2f(400,300), &_input0, true);
    _yesNoMenu = new menuList(xmlReader::getTranslationTexts("yesno", _settings->_language), Vector2f(400,300), &_input0, true);
    _padMenu = new menuPad(420, _settings->_language, &_input0);
    _scoreManager.Reset(_settings->_timeInSec,_settings->_language);
    _scoreManager._wizards = _wizards;

    //UI SELECTOR
    _inputs.push_back(&_input1);
    _inputs.push_back(&_input2);
    _inputs.push_back(&_input3);
    _inputs.push_back(&_input4);
    _menuHeroSelector = new menuHeroSelector(_settings->_language, _inputs, &_soundBox);

    //Texts
    _font.loadFromFile(_files.font2);
    _heroSelectText.setFont(_font);
    _heroSelectText.setCharacterSize(60);
    _heroSelectText.setString(xmlReader::getTranslationText("miscellaneous", _settings->_language, "heroSelect"));
    _heroSelectText.setOrigin(_heroSelectText.getLocalBounds().width/2, _heroSelectText.getLocalBounds().height/2);
    _heroSelectText.setColor(Color(207,4,205));
    _heroSelectText.setPosition(Vector2f(400,-100));
    _resultText.setFont(_font);
    _resultText.setCharacterSize(60);
    _resultText.setString(xmlReader::getTranslationText("miscellaneous", _settings->_language, "results"));
    _resultText.setOrigin(_resultText.getLocalBounds().width/2, _resultText.getLocalBounds().height/2);
    _resultText.setColor(Color(248,29,67));
    _resultText.setPosition(Vector2f(400,-100));
    _yesnoText.setFont(_font);
    _yesnoText.setCharacterSize(40);
    _yesnoText.setString(xmlReader::getTranslationText("miscellaneous", _settings->_language, "confirm"));
    _yesnoText.setOrigin(_yesnoText.getLocalBounds().width/2, _yesnoText.getLocalBounds().height/2);
    _yesnoText.setColor(Color(255,40,165));
    _yesnoText.setPosition(Vector2f(400,200));

    //Game ended transition
    _transitionBlack.setSize(Vector2f(800,600));
    _yesnoShape.setSize(Vector2f(800,60));
    _yesnoShape.setPosition(Vector2f(0,185));
    for(int i=0 ; i<15 ; i++)
    {
        for(int j=0 ; j<20 ; j++)
        {
            _transitionShapes[i][j].setSize(Vector2f(40,40));
            _transitionShapes[i][j].setFillColor(Color(248,29,67));
            _transitionShapes[i][j].setScale(0,0);
            _transitionShapes[i][j].setOrigin(20,20);
            _transitionShapes[i][j].setPosition(j*40+20,i*40+20);
        }
    }
}

bool mainGame::ValidMenu(menuList* menu)
{
    if((_input0.isKeyPressed("VALID") ||
       (Mouse::isButtonPressed(Mouse::Left) && menu->mouseOnSelectedText(_window))) &&
       menu->GetSelectedItem() >= 0)
       return true;
    else
        return false;
}

void mainGame::OpeningClosingMenu()
{
    switch(_gameState)
    {
        case intro:
            if(_input0.isKeyPressed("VALID") || Mouse::isButtonPressed(Mouse::Left))
               _gameState = mainmenu;
            break;
        case mainoptions:
            if((_input0.isKeyPressed("BACK") || _input0.isKeyPressed("PAUSE")) && !_menuKeyPressed)
            {
                _menuKeyPressed = true;
                _mainOptions->ResetAnimation();
                _gameState = mainmenu;
            }
            break;
        case optionlanguage:case optionscreen:case optiontime:case optionlife:case optionMusic:case optionEffects:
            if((_input0.isKeyPressed("BACK") || _input0.isKeyPressed("PAUSE")) && !_menuKeyPressed)
            {
                _menuKeyPressed = true;
                _optionLanguage->ResetAnimation();
                _optionScreen->ResetAnimation();
                _optionTime->ResetAnimation();
                _optionLife->ResetAnimation();
                _gameState = mainoptions;
            }
            break;
        case maincontrols:
            if((_input0.isKeyPressed("BACK") || _input0.isKeyPressed("PAUSE")) && !_menuKeyPressed)
            {
                _menuKeyPressed = true;
                _mainControls->ResetAnimation();
                _gameState = mainmenu;
            }
            break;
        case keyconfig:
            if((_input0.isKeyPressed("BACK") || _input0.isKeyPressed("PAUSE")) && !_menuKeyPressed && !_waitingKey)
            {
                _menuKeyPressed = true;
                _keyConfig->ResetAnimation();
                _gameState = maincontrols;
            }
            break;
        case showpad:
            if((_input0.isKeyPressed("BACK") || _input0.isKeyPressed("PAUSE") ||
                (_padMenu->_backTextFocused && Mouse::isButtonPressed(Mouse::Left))) && !_menuKeyPressed)
            {
                _menuKeyPressed = true;
                _padMenu->ResetAnimation();
                _gameState = maincontrols;
            }
            break;
        case heroSelect:
            if(!_menuKeyPressed && (_input1.isKeyPressed("ROLL") ||
               _input2.isKeyPressed("ROLL") || _input3.isKeyPressed("ROLL") ||
               _input4.isKeyPressed("ROLL") || _input0.isKeyPressed("PAUSE")))
            {
                _menuKeyPressed = true;
                if((_menuHeroSelector->PlayerWaiting(1) && _input1.isKeyPressed("ROLL")) ||
                   (_menuHeroSelector->PlayerWaiting(2) && _input2.isKeyPressed("ROLL")) ||
                   (_menuHeroSelector->PlayerWaiting(3) && _input3.isKeyPressed("ROLL")) ||
                   (_menuHeroSelector->PlayerWaiting(4) && _input4.isKeyPressed("ROLL")) ||
                    _input0.isKeyPressed("PAUSE"))
                {
                    _mainMenu->ResetAnimation();
                    _heroSelectText.setPosition(400,-100);
                    _gameState = mainmenu;
                }
            }
            else if(!_menuKeyPressed && (_input1.isKeyPressed("JUMP") ||
               _input2.isKeyPressed("JUMP") || _input3.isKeyPressed("JUMP") ||
               _input4.isKeyPressed("JUMP") || _input0.isKeyPressed("VALID")))
            {
               _menuKeyPressed = true;
                if(((_menuHeroSelector->PlayerReady(1) && _input1.isKeyPressed("JUMP")) ||
                    (_menuHeroSelector->PlayerReady(2) && _input2.isKeyPressed("JUMP")) ||
                    (_menuHeroSelector->PlayerReady(3) && _input3.isKeyPressed("JUMP")) ||
                    ( _menuHeroSelector->PlayerReady(4) && _input4.isKeyPressed("JUMP"))) &&
                     _menuHeroSelector->MinTwoPlayerOK())
                {
                    ManageWizardCreation();
                    _spellBox->DestroyAllSpells();
                    _scoreManager._wizards = _wizards;
                    _scoreManager.Reset(_settings->_timeInSec, _settings->_language);
                    _menuHeroSelector->ResetMenu();
                    _transitionBlack.setFillColor(Color(0,0,0));
                    _gameState = game;
                    _soundBox.PlayRandomMusic(2);
                }
            }
            break;
        case game:
            if(_input0.isKeyPressed("PAUSE") && !_menuKeyPressed)
            {
                _menuKeyPressed = true;
                _yesNoMenu->ResetAnimation(true);
                _gameState = pause;
            }
            break;
        case gameEnded:
            if((_input0.isKeyPressed("VALID") || _input0.isKeyPressed("PAUSE")) &&
               !_menuKeyPressed && _scoreManager.isAnimationFinished())
            {
                for(int i=0 ; i<15 ; i++)
                {
                    for(int j=0 ; j<20 ; j++)
                    {
                        _transitionShapes[i][j].setScale(0,0);
                        _transitionShapes[i][j].setRotation(0);
                    }
                }
                _resultText.setPosition(Vector2f(400,-100));
                _menuKeyPressed = true;
                _gameState = heroSelect;
                _soundBox.PlayRandomMusic(0);
            }
            break;
        case pause:
            if((_input0.isKeyPressed("BACK") || _input0.isKeyPressed("PAUSE")) && !_menuKeyPressed)
            {
                _pauseMenu->ResetAnimation();
                _menuKeyPressed = true;
                _gameState = game;
            }
            break;
        case yesno:
            if(_input0.isKeyPressed("BACK") && !_menuKeyPressed)
            {
                _menuKeyPressed = true;
                _yesNoMenu->ResetAnimation(true);
                _gameState = _yesnoLastGameState;
            }
            break;
    }
    if(!Keyboard::isKeyPressed(Keyboard::Num0) &&
       !_input0.isKeyPressed("BACK") && !_input0.isKeyPressed("PAUSE") &&
       !_input0.isKeyPressed("VALID") && !Mouse::isButtonPressed(Mouse::Left) &&
       !_input1.isKeyPressed("ROLL") && !_input2.isKeyPressed("ROLL") &&
       !_input3.isKeyPressed("ROLL") && !_input4.isKeyPressed("ROLL") &&
       !_input1.isKeyPressed("JUMP") && !_input2.isKeyPressed("JUMP") &&
       !_input3.isKeyPressed("JUMP") && !_input4.isKeyPressed("JUMP"))
       _menuKeyPressed = false;
}

void mainGame::ManageWizardCreation()
{
    _wizards.clear();
    _wizard1->DisableBody();
    _wizard2->DisableBody();
    _wizard3->DisableBody();
    _wizard4->DisableBody();
    if(_menuHeroSelector->PlayerReady(1))
    {
        _wizard1->ResetHero(_menuHeroSelector->GetSelectedItem(1), _settings->_lifePoints);
        _wizards.push_back(_wizard1);
    }
    if(_menuHeroSelector->PlayerReady(2))
    {
        _wizard2->ResetHero(_menuHeroSelector->GetSelectedItem(2), _settings->_lifePoints);
        _wizards.push_back(_wizard2);
    }
    if(_menuHeroSelector->PlayerReady(3))
    {
        _wizard3->ResetHero(_menuHeroSelector->GetSelectedItem(3), _settings->_lifePoints);
        _wizards.push_back(_wizard3);
    }
    if(_menuHeroSelector->PlayerReady(4))
    {
        _wizard4->ResetHero(_menuHeroSelector->GetSelectedItem(4), _settings->_lifePoints);
        _wizards.push_back(_wizard4);
    }
    _spellBox->UpdateWizardsPointers(&_wizards);
    _wizard1->_wizards = _wizards;
    _wizard2->_wizards = _wizards;
    _wizard3->_wizards = _wizards;
    _wizard4->_wizards = _wizards;
}

void mainGame::MainMenuEvents()
{
    if(ValidMenu(_mainMenu) && !_menuKeyPressed)
    {
        switch(_mainMenu->GetSelectedItem())
        {
            case 0:
                _menuKeyPressed = true;
                _menuHeroSelector->ResetMenu();
                _gameState = heroSelect;
                break;
            case 1:
                _menuKeyPressed = true;
                _mainMenu->ResetAnimation(false);
                _mainOptions->UpdateOptions(xmlReader::getSettingsValue(), false);
                _gameState = mainoptions;
                break;
            case 2:
                _menuKeyPressed = true;
                _mainMenu->ResetAnimation(false);
                _gameState = maincontrols;
                break;
            case 3:
                exit(0);
                break;
        }
    }
}

void mainGame::MainOptionsEvents()
{
    if(ValidMenu(_mainOptions) && !_menuKeyPressed)
    {
        switch(_mainOptions->GetSelectedItem())
        {
            case 0:
                _menuKeyPressed = true;
                _gameState = optionlanguage;
                _optionLanguage->FocusText(_settings->_language);
                break;
            case 1:
                _menuKeyPressed = true;
                _gameState = optionscreen;
                _optionScreen->FocusText(_settings->_fullscreen);
                break;
            case 2:
                _menuKeyPressed = true;
                _gameState = optiontime;
                _optionTime->FocusText(_settings->_timeString);
                break;
            case 3:
                _menuKeyPressed = true;
                _gameState = optionlife;
                _optionLife->FocusText(_settings->_lifeString);
                break;
            case 4:
                _menuKeyPressed = true;
                _gameState = optionMusic;
                _optionMusic->FocusText(_settings->_musicString);
                break;
            case 5:
                _menuKeyPressed = true;
                _gameState = optionEffects;
                _optionEffects->FocusText(_settings->_effectsString);
                break;
            case 6:
                _menuKeyPressed = true;
                _mainOptions->ResetAnimation();
                _gameState = mainmenu;
                break;
        }
    }
}

void mainGame::OptionsEvents()
{
    if((ValidMenu(_optionLanguage) || ValidMenu(_optionScreen) ||
        ValidMenu(_optionTime) || ValidMenu(_optionLife) ||
        ValidMenu(_optionMusic) || ValidMenu(_optionEffects)) &&
        !_menuKeyPressed)
    {
        _menuKeyPressed = true;
        switch(_gameState)
        {
            case optionlanguage:{
                if(_optionLanguage->GetSelectedText() == "")
                    return;
                _settings->_language = _optionLanguage->GetSelectedText();
                RedrawUI();}
                break;
            case optionscreen:{
                if(_optionScreen->GetSelectedText() == "")
                    return;
                _settings->_fullscreen = _optionScreen->GetSelectedText();
                ChangeScreenMode();}
                break;
            case optiontime:{
                if(_optionTime->GetSelectedText() == "")
                    return;
                string time = _optionTime->GetSelectedText().substr(0,2);
                _settings->_timeInSec = atoi(time.c_str())*60;
                _settings->_timeString = _optionTime->GetSelectedText(); }
                break;
            case optionlife:{
                if(_optionLife->GetSelectedText() == "")
                    return;
                _settings->_lifePoints = atoi(_optionLife->GetSelectedText().c_str());
                _settings->_lifeString = _optionLife->GetSelectedText();}
                break;
            case optionMusic:{
                if(_optionMusic->GetSelectedText() == "")
                    return;
                _settings->_music = atoi(_optionMusic->GetSelectedText().c_str());
                _settings->_musicString = _optionMusic->GetSelectedText();
                _soundBox.UpdateVolumes(_settings);}
                break;
            case optionEffects:{
                if(_optionEffects->GetSelectedText() == "")
                    return;
                _settings->_effects = atoi(_optionEffects->GetSelectedText().c_str());
                _settings->_effectsString = _optionEffects->GetSelectedText();
                _soundBox.UpdateVolumes(_settings);}
                break;
        }
        _settings->save();
        _optionLanguage->ResetAnimation();
        _optionScreen->ResetAnimation();
        _optionTime->ResetAnimation();
        _optionLife->ResetAnimation();
        _optionMusic->ResetAnimation();
        _optionEffects->ResetAnimation();
        _mainOptions->UpdateOptions(xmlReader::getSettingsValue(), false);
        _gameState = mainoptions;
    }
}

void mainGame::RedrawUI()
{
    //Reset UI texts
    _settings->_language = _optionLanguage->GetSelectedText();
    _mainMenu = new menuList(xmlReader::getTranslationTexts("menuMain", _settings->_language), Vector2f(400,400), &_input0, false);
    _mainControls = new menuList(xmlReader::getTranslationTexts("menuControls", _settings->_language), Vector2f(400,400), &_input0, false);
    _mainOptions = new menuOptions(xmlReader::getTranslationTexts("settings", _settings->_language), Vector2f(400,360), &_input0, false);
    _keyConfig = new menuOptions(xmlReader::getTranslationTexts("key", _settings->_language), Vector2f(400,355), &_input0, false);
    _pauseMenu = new menuList(xmlReader::getTranslationTexts("pause", _settings->_language), Vector2f(400,300), &_input0, true);
    _padMenu = new menuPad(420, _settings->_language, &_input0);
    _menuHeroSelector = new menuHeroSelector(_settings->_language, _inputs, &_soundBox);
    _yesNoMenu = new menuList(xmlReader::getTranslationTexts("yesno", _settings->_language), Vector2f(400,300), &_input0, true);
    _heroSelectText.setString(xmlReader::getTranslationText("miscellaneous", _settings->_language, "heroSelect"));
    _heroSelectText.setOrigin(_heroSelectText.getLocalBounds().width/2, _heroSelectText.getLocalBounds().height/2);
    _heroSelectText.setPosition(Vector2f(400,-100));
    _resultText.setString(xmlReader::getTranslationText("miscellaneous", _settings->_language, "results"));
    _resultText.setOrigin(_resultText.getLocalBounds().width/2, _resultText.getLocalBounds().height/2);
    _resultText.setPosition(Vector2f(400,-100));
    _yesnoText.setString(xmlReader::getTranslationText("miscellaneous", _settings->_language, "confirm"));
    _yesnoText.setOrigin(_yesnoText.getLocalBounds().width/2, _yesnoText.getLocalBounds().height/2);
}

void mainGame::ChangeScreenMode()
{
    //Getting screen resolution
    Vector2i screenSize;
    if(_settings->_fullscreen == "ON")
        screenSize = Vector2i(VideoMode::getDesktopMode().width, VideoMode::getDesktopMode().height);
    else
        screenSize = Vector2i(800,600);

    //Recreating window
    ContextSettings ctxSettings;
    ctxSettings.antialiasingLevel = 0;
    _window->create(VideoMode(screenSize.x, screenSize.y, 32), "EggFall",
    _settings->_fullscreen == "ON" ? Style::Fullscreen : Style::Default, ctxSettings);

    //View
    FloatRect rect;
    if(600.0*screenSize.x/screenSize.y >= 800)
        rect = FloatRect(0,0,600.0*screenSize.x/screenSize.y,600);
    else
        rect = FloatRect(0,0,800,800*screenSize.y/screenSize.x);
    View view(rect); //800x600 native view
    view.setCenter(400,300);
    _window->setView(view);
}

void mainGame::MainControlsEvents()
{
    if(ValidMenu(_mainControls) && !_menuKeyPressed)
    {
        switch(_mainControls->GetSelectedItem())
        {
            case 0:
                _menuKeyPressed = true;
                _gameState = showpad;
                _mainControls->ResetAnimation();
                break;
            case 5:
                _menuKeyPressed = true;
                _gameState = mainmenu;
                _mainControls->ResetAnimation();
                break;
            case 1:case 2:case 3:case 4:
                _menuKeyPressed = true;
                stringstream stream;
                stream << _mainControls->GetSelectedItem();
                _keyConfig->UpdateOptions(xmlReader::getKeysValue(stream.str()), true);
                _editingInputNum = _mainControls->GetSelectedItem();
                if(_mainControls->GetSelectedItem() == 1)
                    _editingInput = &_input1;
                else if(_mainControls->GetSelectedItem() == 2)
                    _editingInput = &_input2;
                else if(_mainControls->GetSelectedItem() == 3)
                    _editingInput = &_input3;
                else
                    _editingInput = &_input4;
                _mainControls->ResetAnimation(false);
                _gameState = keyconfig;
                break;
        }
    }
}

void mainGame::KeyConfigEvents()
{
    if(_keyConfig->GetSelectedItem() < 8)
    {
        if(ValidMenu(_keyConfig) && !_menuKeyPressed && !_waitingKey)
        {
            _menuKeyPressed = true;
            _keyConfig->SetTextOption(_keyConfig->GetSelectedItem(), "...");
            _waitingKey = true;
        }
        else if(_waitingKey)
        {
            if(_globalKeyPressed)
            {
                _menuKeyPressed = true;
                _editingInput->setKey(_keyConfig->GetSelectedItem(), _currentKey);
                _editingInput->WriteXML();
                stringstream stream;
                stream << _editingInputNum;
                _keyConfig->UpdateOptions(xmlReader::getKeysValue(stream.str()), true);
                _waitingKey = false;
            }
        }
    }
    else if(ValidMenu(_keyConfig) && !_menuKeyPressed)
    {
        _menuKeyPressed = true;
        _gameState = maincontrols;
        _keyConfig->ResetAnimation();
    }
}

void mainGame::PauseMenuEvents()
{
    if(ValidMenu(_pauseMenu) && !_menuKeyPressed)
    {
        switch(_pauseMenu->GetSelectedItem())
        {
            case 0:
                _menuKeyPressed = true;
                _pauseMenu->ResetAnimation();
                for(int i=0 ; i<_wizards.size() ; i++)
                    _wizards[i]->_hasJumped = true;
                _gameState = game;
                break;
            case 1:case 2: //Validation YES-NO
                _menuKeyPressed = true;
                _yesNoMenu->ResetAnimation(true);
                _yesnoLastGameState = pause;
                _yesnoShape.setPosition(800, _yesnoShape.getPosition().y);
                _yesnoText.setPosition(0, _yesnoText.getPosition().y);
                _gameState = yesno;
                break;
            case 3:
                exit(0);
                break;
        }
    }
}

void mainGame::YesNoMenuEvents()
{
    if(ValidMenu(_yesNoMenu) && !_menuKeyPressed)
    {
        switch(_yesNoMenu->GetSelectedItem())
        {
            case 0:
                if(_pauseMenu->GetSelectedItem() == 1)
                {
                    for(int i=0 ; i<_wizards.size() ; i++)
                        _wizards[i]->ResetHero(_wizards[i]->GetWizardType(), _settings->_lifePoints);
                    _spellBox->DestroyAllSpells();
                    _scoreManager._wizards = _wizards;
                    _scoreManager.Reset(_settings->_timeInSec, _settings->_language);
                    _menuHeroSelector->ResetMenu();
                    _transitionBlack.setFillColor(Color(0,0,0));
                    _gameState = game;
                    _pauseMenu->ResetAnimation(true);
                    _soundBox.PlayRandomMusic(2);
                }
                else if(_pauseMenu->GetSelectedItem() == 2)
                {
                    _menuKeyPressed = true;
                    _mainMenu->ResetAnimation();
                    _pauseMenu->ResetAnimation(true);
                    _gameState = mainmenu;
                    _soundBox.PlayRandomMusic(0);
                }
                break;
            case 1:
                _menuKeyPressed = true;
                _yesNoMenu->ResetAnimation(true);
                _gameState = _yesnoLastGameState;
                break;
        }
    }
}

void mainGame::DisplayGameResources()
{
    _maps->DisplayMap(_window);
    for(int i=0 ; i<_wizards.size() ; i++)
        _wizards[i]->Display(_window, _deltaTime);
}

bool mainGame::TitleAnimation()
{
     //Animating Title Text
    float limit = _titleAnimationFrame == 0 ? 0.28 : 0.05;
    if(fabs(_titleScreenTitle.getScale().x  - _titleAnimationScales[_titleAnimationFrame].x) > limit &&
       _titleAnimationFrame <= 3)
    {
        int speed = _titleAnimationFrame == 0 ? 4 : 3;
        Vector2f scale = _titleScreenTitle.getScale();
        Vector2f scale2 = _titleAnimationScales[_titleAnimationFrame];
        _titleScreenTitle.setScale(scale.x+(scale2.x-scale.x)/speed*_deltaTime,scale.y+(scale2.y-scale.y)/speed*_deltaTime);
        return false;
    }
    else if(_titleAnimationFrame < 3)
    {
        _titleScreenTitle.setScale(_titleAnimationScales[_titleAnimationFrame]);
        _titleAnimationFrame ++;
        return false;
    }
    else if(_titleAnimationFrame >= 3 && _titleAnimationFrame < 8)
    {
        int i = 4-(_titleAnimationFrame-3);
        if(fabs(_titleScreenHats[i].getPosition().x - (275+i*52)) > 3 &&
           !_input0.isKeyPressed("VALID") && !Mouse::isButtonPressed(Mouse::Left))
        {
            Vector2f pos = _titleScreenHats[i].getPosition();
            _titleScreenHats[i].setPosition(pos.x+((275+i*52)-pos.x)/3*_deltaTime, pos.y);
        }
        else
        {
            _menuKeyPressed = true;
            _titleScreenHats[i].setPosition(275+i*52, _titleScreenHats[i].getPosition().y);
            _titleAnimationFrame ++;
        }
        return false;
    }
    else if(_gameState != heroSelect && _titleScreenTitle.getPosition().y < 100)
    {
        if(_titleScreenTitle.getPosition().y < 100)
            _titleScreenTitle.move(0, 20*_deltaTime);
        else
            _titleScreenTitle.setPosition(400,100);
    }
    else
        return true;
}

void mainGame::MenuAnimation()
{
    //Moving hats
    if(_gameState != heroSelect && _titleScreenHats[0].getPosition().y < 210)
    {
        for(int i=0 ; i<5 ; i++)
        {
            if(_titleScreenHats[i].getPosition().y < 210)
                _titleScreenHats[i].move(0,10*_deltaTime);
            else
                _titleScreenHats[i].setPosition(_titleScreenHats[i].getPosition().x, 210);
        }
    }
    else if(_gameState == heroSelect && _titleScreenHats[0].getPosition().y > 120)
    {
        for(int i=0 ; i<5 ; i++)
        {
            if(_titleScreenHats[i].getPosition().y > 120)
                _titleScreenHats[i].move(0,-10*_deltaTime);
            else
                _titleScreenHats[i].setPosition(_titleScreenHats[i].getPosition().x, 120);
        }
    }
}

bool mainGame::GameEndedAnimation()
{
    if(_transitionShapes[0][0].getScale().x < 1)
    {
        float scale = _transitionShapes[0][0].getScale().x;
        for(int i=0 ; i<15 ; i++)
        {
            for(int j=0 ; j<20 ; j++)
            {
                _transitionShapes[i][j].setScale(scale+0.025, scale+0.025);
                _transitionShapes[i][j].setRotation(scale*40*4.5);
            }
        }
    }
    else if(_titleScreenBackground.getColor().a < 250)
        _titleScreenBackground.setColor(Color(255,255,255,_titleScreenBackground.getColor().a+10));
    else if(_resultText.getPosition().y < 20)
        _resultText.move(0,10);
    else
        return true;
    return false;
}

bool mainGame::IsMusicPaused()
{
    return _soundBox.IsMusicPaused();
}

void mainGame::PauseMusic(bool pause)
{
    _soundBox.PauseMusic(pause);
}

void mainGame::GoOn(Keyboard::Key key, bool keyPressed, float deltaTime)
{
    //Updating variables
    _currentKey = key;
    _globalKeyPressed = keyPressed;
    _deltaTime = deltaTime;

    //Updating last key (keyboard or pad)
    _input0.PerformLastController(_window);

    //Playing music
    if(IsMusicPaused())
        PauseMusic(false);

    //Events
    OpeningClosingMenu();
    MenuAnimation();

    //Main switch
    switch(_gameState)
    {
        case mainmenu:
            _window->draw(_titleScreenBackground);
            _window->draw(_titleScreenTitle);
            for(int i=0 ; i<5 ; i++)
                _window->draw(_titleScreenHats[i]);
            if(TitleAnimation())
            {
                MainMenuEvents();
                _mainMenu->Display(_window, _deltaTime);
            }
            break;
        case mainoptions:
            MainOptionsEvents();
            _window->draw(_titleScreenBackground);
            _window->draw(_titleScreenTitle);
            for(int i=0 ; i<5 ; i++)
                _window->draw(_titleScreenHats[i]);
            _mainOptions->Display(_window, _deltaTime);
            break;
        case optionlanguage:case optionscreen:case optiontime:
        case optionlife:case optionMusic:case optionEffects:
            OptionsEvents();
            _window->draw(_titleScreenBackground);
            _window->draw(_titleScreenTitle);
            for(int i=0 ; i<5 ; i++)
                _window->draw(_titleScreenHats[i]);
            _mainOptions->Display(_window, _deltaTime, false);
            if(_gameState == optionlanguage) { _optionLanguage->Display(_window, _deltaTime, true, false); }
            if(_gameState == optionscreen)   { _optionScreen->Display(_window, _deltaTime, true, false); }
            if(_gameState == optiontime)     { _optionTime->Display(_window, _deltaTime, true, false); }
            if(_gameState == optionlife)     { _optionLife->Display(_window, _deltaTime, true, false); }
            if(_gameState == optionMusic)    { _optionMusic->Display(_window, _deltaTime, true, false); }
            if(_gameState == optionEffects)  { _optionEffects->Display(_window, _deltaTime, true, false); }
            break;
        case maincontrols:
            MainControlsEvents();
            _window->draw(_titleScreenBackground);
            _window->draw(_titleScreenTitle);
            for(int i=0 ; i<5 ; i++)
                _window->draw(_titleScreenHats[i]);
            _mainControls->Display(_window, _deltaTime);
            break;
        case keyconfig:
            KeyConfigEvents();
            _window->draw(_titleScreenBackground);
            _window->draw(_titleScreenTitle);
            for(int i=0 ; i<5 ; i++)
                _window->draw(_titleScreenHats[i]);
            _keyConfig->Display(_window, _deltaTime);
            break;
        case showpad:
            _window->draw(_titleScreenBackground);
            _window->draw(_titleScreenTitle);
            for(int i=0 ; i<5 ; i++)
                _window->draw(_titleScreenHats[i]);
            _padMenu->Display(_window, _deltaTime);
            break;
        case heroSelect:
            //Step
            _world->Step(_deltaTime/60.f, 8, 3);
            _maps->RenderMap();
            _effectBox.RenderEffects(_world, _window, _deltaTime);

            //Animations
            if(_heroSelectText.getPosition().y < 20)
                _heroSelectText.move(0, 10*_deltaTime);
            else
                _heroSelectText.setPosition(400,20);
            if(_titleScreenTitle.getPosition().y > -100)
                _titleScreenTitle.move(0, -20*_deltaTime);
            else
                _titleScreenTitle.setPosition(400,-100);

            //Draw
            _window->draw(_titleScreenBackground);
            _window->draw(_titleScreenTitle);
            _window->draw(_heroSelectText);
            for(int i=0 ; i<5 ; i++)
                _window->draw(_titleScreenHats[i]);
            _menuHeroSelector->Display(_window, _deltaTime);
            break;
        case game:
            DisplayGameResources();
            _world->Step(_deltaTime/60.f, 8, 3);
            Box2DTools::ManageBodyLoop(_world);
            _maps->RenderMap();
            _soundBox.PerformSounds();
            for(int i=0 ; i<_wizards.size() ; i++)
                _wizards[i]->Step();
            _effectBox.RenderEffects(_world, _window, _deltaTime);
            _spellBox->RenderSpells(_deltaTime);
            _spellBox->Display(_window);
            _scoreManager.Step();
            _scoreManager.Display(_window);

            //Score
            if(_scoreManager.timePassed() >= (float)_settings->_timeInSec-6.01 &&
               _scoreManager.timePassed() <= (float)_settings->_timeInSec-5.99)
                _soundBox.PlaySound(12, false, 36);
            else if(_scoreManager.timePassed() >= (float)_settings->_timeInSec)
            {
                _titleScreenBackground.setColor(Color(255,255,255,0));
                _scoreManager.Reset(0, _settings->_language, true);
                _gameState = gameEnded;
                _soundBox.PlayRandomMusic(4);
            }

            //Black transition
            _window->draw(_transitionBlack);
            if(_transitionBlack.getFillColor().a > 0)
                _transitionBlack.setFillColor(Color(0,0,0,_transitionBlack.getFillColor().a-5));
            break;
        case gameEnded:
            DisplayGameResources();
            _spellBox->Display(_window);
            _effectBox.Display(_window);
            for(int i=0 ; i<15 ; i++)
                for(int j=0 ; j<20 ; j++)
                    _window->draw(_transitionShapes[i][j]);
            _window->draw(_titleScreenBackground);
            if(GameEndedAnimation())
                _scoreManager.Display(_window, true);
            _window->draw(_resultText);
            break;
        case pause:
            DisplayGameResources();
            PauseMenuEvents();
            _spellBox->Display(_window);
            _effectBox.Display(_window);
            _scoreManager.Display(_window);
            _pauseMenu->Display(_window, _deltaTime);
            break;
        case yesno:
            if(_yesnoLastGameState == pause)
            {
                DisplayGameResources();
                _spellBox->Display(_window);
                _effectBox.Display(_window);
                _scoreManager.Display(_window);
                _pauseMenu->Display(_window, _deltaTime, false);
            }
            YesNoMenuEvents();
            _yesNoMenu->Display(_window, _deltaTime, true, false);

            if(_yesnoShape.getPosition().x > 0)
                _yesnoShape.setPosition(_yesnoShape.getPosition().x-80, _yesnoShape.getPosition().y);
            if(_yesnoText.getPosition().x < 400)
                _yesnoText.setPosition(_yesnoText.getPosition().x+40, _yesnoText.getPosition().y);
            _window->draw(_yesnoShape);
            _window->draw(_yesnoText);
            break;
    }

    //DEBUG
    _showMasks = Keyboard::isKeyPressed(Keyboard::M) ? true : _showMasks;
    _showMasks = Keyboard::isKeyPressed(Keyboard::L) ? false : _showMasks;

    if(_showMasks && _gameState == game)
        Box2DTools::DrawCollisionMask(_world, _window);
}
