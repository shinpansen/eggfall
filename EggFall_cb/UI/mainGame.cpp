#include "mainGame.hpp"

using namespace std;
using namespace sf;

mainGame::mainGame(RenderWindow* window, settings* settings)
{
    //Variables
    _gameState = MAIN_MENU;
    _lastGameState = MAIN_MENU;
    _deltaTime = 1;
    _menuKeyPressed = false;
    _globalKeyPressed = false;
    _waitingKey = false;
	_isPlayersLoading = false;
    _showMasks = false;
    _showFps = false;
    _editingInputNum = 0;
	_frameScreenshot = 0.f;

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

	//Inputs
	_input0 = input(-1, 30, _window, _settings->_analog == "ON"); //Menu input
	_input1 = input(0, 30, _window, _settings->_analog == "ON");
	_input2 = input(1, 30, _window, _settings->_analog == "ON");
	_input3 = input(2, 30, _window, _settings->_analog == "ON");
	_input4 = input(3, 30, _window, _settings->_analog == "ON");
	_inputList.push_back(&_input1);
	_inputList.push_back(&_input2);
	_inputList.push_back(&_input3);
	_inputList.push_back(&_input4);

    //Box2D - World
    _world = new b2World(b2Vec2(0.f, 42.f));
    _maps = new maps(_world, &_effectBox, &_input0);
	_maps->LoadMap("environment//mapFiles//map.xml");
	_contactListener = new contactListener(_world); //Override PreSolve event using custom contact listener

    //Spells - Music
    _projectileBox = new projectiles(_world, &_effectBox, &_soundBox, _maps);
	_scoreManager = new scoreManager(&_effectBox);
    _soundBox.PlayMusic(MUSIC::Chibi_Ninja);

    //Graphics
    _logoTexture.loadFromFile(files::logo);
    _titleScreenBackgroundTexture.loadFromFile(files::titleScreenBackground);
    _titleScreenTitleTexture.loadFromFile(files::titleScreenTitle);
    _titleScreenHatsTexture.loadFromFile(files::titleScreenHats);
    _treeTexture.loadFromFile(files::trees);
    _logo.setTexture(_logoTexture);
    _logo.setColor(Color(255,255,255,0));
    _titleScreenBackground.setTexture(_titleScreenBackgroundTexture);
    _titleScreenTitle.setTexture(_titleScreenTitleTexture);
    _titleScreenTitle.setOrigin(_titleScreenTitle.getLocalBounds().width/2, _titleScreenTitle.getLocalBounds().height/2);
    _titleScreenTitle.setPosition(640, 100);
    _titleScreenTitle.setScale(30,30);
    _titleScreenHats = new Sprite[5];
    for(int i=0 ; i<5 ; i++)
    {
        int width = i <= 0 ? 48 : 42;
        _titleScreenHats[i].setTexture(_titleScreenHatsTexture);
        _titleScreenHats[i].setTextureRect(IntRect(i*48, 0, width, 30));
        _titleScreenHats[i].setPosition(-264+(4-i)*52, 190);
    }

    //Wizards
    _player1 = new wizard(0, 2, Vector2f(185,145), _world, &_input1, &_effectBox, _projectileBox, &_soundBox, _scoreManager);
	_player2 = new wizard(1, 2, Vector2f(615,145), _world, &_input2, &_effectBox, _projectileBox, &_soundBox, _scoreManager);
	_player3 = new wizard(2, 2, Vector2f(115,416), _world, &_input3, &_effectBox, _projectileBox, &_soundBox, _scoreManager);
	_player4 = new wizard(3, 2, Vector2f(685,416), _world, &_input4, &_effectBox, _projectileBox, &_soundBox, _scoreManager);
	_players.push_back(_player1);
	_players.push_back(_player2);
	_players.push_back(_player3);
	_players.push_back(_player4);
	_projectileBox->UpdatePlayersPointers(&_players);
	_player1->_players = _players;
	_player2->_players = _players;
	_player3->_players = _players;
	_player4->_players = _players;

    //UI
    _mainMenu = new menuList(xmlReader::getTranslationTexts("menuMain", _settings->_language), Vector2f(640,440), &_input0, false);
	_mainOptions = new menuOptions(xmlReader::getTranslationTexts("settings", _settings->_language), Vector2f(640, 440), &_input0, false);
    _mainControls = new menuList(xmlReader::getTranslationTexts("menuControls", _settings->_language), Vector2f(640, 440), &_input0, false);
    _optionLanguage = new menuList(_settings->getSettingsChoices(CHOICE_LANG), Vector2f(640, 440), &_input0, true, ALIGN_MIDDLE, _mainOptions);
    _optionScreen = new menuList(_settings->getSettingsChoices(CHOICE_ON_OFF), Vector2f(640, 440), &_input0, true, ALIGN_MIDDLE, _mainOptions);
    _optionTime = new menuList(_settings->getSettingsChoices(CHOICE_TIME), Vector2f(640, 440), &_input0, true, ALIGN_MIDDLE, _mainOptions);
    _optionLife = new menuList(_settings->getSettingsChoices(CHOICE_LIFE), Vector2f(640, 440), &_input0, true, ALIGN_MIDDLE, _mainOptions);
    _optionMusic = new menuList(_settings->getSettingsChoices(CHOICE_SOUND), Vector2f(640, 440), &_input0, true, ALIGN_MIDDLE, _mainOptions);
    _optionEffects = new menuList(_settings->getSettingsChoices(CHOICE_SOUND), Vector2f(640, 440), &_input0, true, ALIGN_MIDDLE, _mainOptions);
    _keyConfig = new menuOptions(xmlReader::getTranslationTexts("key", _settings->_language), Vector2f(640, 440), &_input0, false);
    _pauseMenu = new menuList(xmlReader::getTranslationTexts("pause", _settings->_language), Vector2f(640, 360), &_input0, true);
    _yesNoMenu = new menuList(xmlReader::getTranslationTexts("yesno", _settings->_language), Vector2f(640, 360), &_input0, true);
    _padMenu = new menuPad(420, _settings->_language, &_input0);
    _scoreManager->Reset(_settings->_timeInSec,_settings->_language);
    _scoreManager->_players = _players;
	_currentOptionMenu = NULL;

    //UI SELECTOR
    _inputs.push_back(&_input1);
    _inputs.push_back(&_input2);
    _inputs.push_back(&_input3);
    _inputs.push_back(&_input4);
    _menuHeroSelector = new menuHeroSelector(_settings->_language, _inputs, &_soundBox);

	//Thread
	_threadLoadPlayers = new Thread(bind(&ManagePlayerCreation, &_isPlayersLoading, &_players, _player1, _player2, _player3, _player4,
		_menuHeroSelector, _projectileBox, _settings, _scoreManager));

    //Texts
	_font.loadFromFile(files::font2);
	_fontB.loadFromFile(files::font3);
	_heroSelectText = Text(xmlReader::getTranslationText("miscellaneous", _settings->_language, "heroSelect"), _fontB, 40);
    _heroSelectText.setOrigin(_heroSelectText.getLocalBounds().width/2, _heroSelectText.getLocalBounds().height/2);
    _heroSelectText.setFillColor(Color(207,4,205));
    _heroSelectText.setPosition(Vector2f(640,-100));
	_resultText = Text(xmlReader::getTranslationText("miscellaneous", _settings->_language, "results"), _fontB, 40);
    _resultText.setOrigin(_resultText.getLocalBounds().width/2, _resultText.getLocalBounds().height/2);
    _resultText.setFillColor(Color(248,29,67));
    _resultText.setPosition(Vector2f(640,-100));
	_yesnoText = Text(xmlReader::getTranslationText("miscellaneous", _settings->_language, "confirm"), _fontB, 40);
    _yesnoText.setOrigin(_yesnoText.getLocalBounds().width/2, _yesnoText.getLocalBounds().height/2);
    _yesnoText.setFillColor(Color(255,40,165));
    _yesnoText.setPosition(Vector2f(640,135));
    _frameRate.setFont(_font);
    _frameRate.setFillColor(Color(165,70,165));
    _frameRate.setOutlineColor(Color(30,30,30));
    _frameRate.setOutlineThickness(2);
    _frameRate.setCharacterSize(30);
    _frameRate.setPosition(Vector2f(5, 0));

    //Game ended transition
    _transitionBlack.setSize(Vector2f(utils::VIEW_WIDTH, utils::VIEW_HEIGHT));
    _yesnoShape.setSize(Vector2f(utils::VIEW_WIDTH,60));
    _yesnoShape.setPosition(Vector2f(0,110));
	int rowSize = sizeof(_transitionShapes) / sizeof(_transitionShapes[0]);
	int colSize = sizeof(_transitionShapes[0]) / sizeof(_transitionShapes[0][0]);
    for(int i=0 ; i<rowSize; i++)
    {
        for(int j=0 ; j<colSize; j++)
        {
            _transitionShapes[i][j].setSize(Vector2f(40,40));
            _transitionShapes[i][j].setFillColor(Color(248,29,67));
            _transitionShapes[i][j].setScale(0,0);
            _transitionShapes[i][j].setOrigin(20,20);
            _transitionShapes[i][j].setPosition(j*40+20,i*40+20);
        }
    }
}

void ManagePlayerCreation(bool * isPlayersLoading, vector<hero*> * players, hero * player1, hero * player2, hero * player3, hero * player4,
	menuHeroSelector * menuHeroSelector, projectiles * projectileBox, settings * settings, scoreManager * scoreManager)
{
	*isPlayersLoading = true;

	//Clear - disable
	players->clear();
	player1->DisableBody();
	player2->DisableBody();
	player3->DisableBody();
	player4->DisableBody();
	projectileBox->DestroyEverything();

	//Recreate regarding players in the game
	if (menuHeroSelector->PlayerReady(1))
	{
		player1->ResetHero(menuHeroSelector->GetSelectedItem(1), settings->_lifePoints);
		players->push_back(player1);
	}
	if (menuHeroSelector->PlayerReady(2))
	{
		player2->ResetHero(menuHeroSelector->GetSelectedItem(2), settings->_lifePoints);
		players->push_back(player2);
	}
	if (menuHeroSelector->PlayerReady(3))
	{
		player3->ResetHero(menuHeroSelector->GetSelectedItem(3), settings->_lifePoints);
		players->push_back(player3);
	}
	if (menuHeroSelector->PlayerReady(4))
	{
		player4->ResetHero(menuHeroSelector->GetSelectedItem(4), settings->_lifePoints);
		players->push_back(player4);
	}

	//Updating object references
	projectileBox->UpdatePlayersPointers(players);
	player1->_players = *players;
	player2->_players = *players;
	player3->_players = *players;
	player4->_players = *players;
	scoreManager->_players = *players;
	scoreManager->Reset(settings->_timeInSec, settings->_language);
	menuHeroSelector->ResetMenu();

	*isPlayersLoading = false;
}

/////////////// EVENTS METHODS ////////////////////
bool mainGame::ValidMenu(menuList* menu)
{
    if((_input0.isKeyPressed(CMD_VALID) ||
       (Mouse::isButtonPressed(Mouse::Left) && menu->MouseOnSelectedText(_window))) &&
       menu->GetSelectedItem() >= 0)
       return true;
    else
        return false;
}

void mainGame::OpeningClosingMenu()
{
    switch(_gameState)
    {
        case INTRO:
            if(_input0.isKeyPressed(CMD_VALID) || Mouse::isButtonPressed(Mouse::Left))
               _gameState = MAIN_MENU;
            break;
        case MAIN_OPTIONS:
            if((_input0.isKeyPressed(CMD_BACK) || _input0.isKeyPressed(CMD_PAUSE)) && !_menuKeyPressed)
            {
                _menuKeyPressed = true;
                _mainOptions->ResetAnimation();
                _gameState = _lastGameState;
            }
            break;
		case OPTION_LANG:case OPTION_SCREEN:case OPTION_TIME:case OPTION_ANALOG:case OPTION_LIFE:case OPTION_MUSIC:case OPTION_SFX:
			if ((_input0.isKeyPressed(CMD_BACK) || _input0.isKeyPressed(CMD_PAUSE) || (Mouse::isButtonPressed(Mouse::Left) &&
				_currentOptionMenu != NULL && _currentOptionMenu->IsMouseOutsideMenu(_window))) &&
				!_menuKeyPressed)
            {
                _menuKeyPressed = true;
                _optionLanguage->ResetAnimation();
                _optionScreen->ResetAnimation();
                _optionTime->ResetAnimation();
                _optionLife->ResetAnimation();
                _gameState = MAIN_OPTIONS;
            }
            break;
        case MAIN_COMMANDS:
            if((_input0.isKeyPressed(CMD_BACK) || _input0.isKeyPressed(CMD_PAUSE)) && !_menuKeyPressed)
            {
                _menuKeyPressed = true;
                _mainControls->ResetAnimation();
                _gameState = _lastGameState;
            }
            break;
        case CONFIG_KEYS:
            if((_input0.isKeyPressed(CMD_BACK) || _input0.isKeyPressed(CMD_PAUSE)) && !_menuKeyPressed && !_waitingKey)
            {
                _menuKeyPressed = true;
                _keyConfig->ResetAnimation();
                _gameState = MAIN_COMMANDS;
            }
            break;
        case SHOW_GAMEPAD:
            if((_input0.isKeyPressed(CMD_BACK) || _input0.isKeyPressed(CMD_PAUSE) ||
                (_padMenu->_backTextFocused && Mouse::isButtonPressed(Mouse::Left))) && !_menuKeyPressed)
            {
                _menuKeyPressed = true;
                _padMenu->ResetAnimation();
                _gameState = MAIN_COMMANDS;
            }
            break;
        case HERO_SELECT:
            if(!_menuKeyPressed && (_input1.isKeyPressed(CMD_COUNTERACT) ||
               _input2.isKeyPressed(CMD_COUNTERACT) || _input3.isKeyPressed(CMD_COUNTERACT) ||
               _input4.isKeyPressed(CMD_COUNTERACT) || _input0.isKeyPressed(CMD_PAUSE)))
            {
                _menuKeyPressed = true;
                if((_menuHeroSelector->PlayerWaiting(1) && _input1.isKeyPressed(CMD_COUNTERACT)) ||
                   (_menuHeroSelector->PlayerWaiting(2) && _input2.isKeyPressed(CMD_COUNTERACT)) ||
                   (_menuHeroSelector->PlayerWaiting(3) && _input3.isKeyPressed(CMD_COUNTERACT)) ||
                   (_menuHeroSelector->PlayerWaiting(4) && _input4.isKeyPressed(CMD_COUNTERACT)) ||
                    _input0.isKeyPressed(CMD_PAUSE))
                {
                    _mainMenu->ResetAnimation();
                    _heroSelectText.setPosition(640,-100);
                    _gameState = MAIN_MENU;
                }
            }
            else if(!_menuKeyPressed && (_input1.isKeyPressed(CMD_JUMP) ||
               _input2.isKeyPressed(CMD_JUMP) || _input3.isKeyPressed(CMD_JUMP) ||
               _input4.isKeyPressed(CMD_JUMP) || _input0.isKeyPressed(CMD_VALID)))
            {
               _menuKeyPressed = true;
                if(((_menuHeroSelector->PlayerReady(1) && _input1.isKeyPressed(CMD_JUMP)) ||
                    (_menuHeroSelector->PlayerReady(2) && _input2.isKeyPressed(CMD_JUMP)) ||
                    (_menuHeroSelector->PlayerReady(3) && _input3.isKeyPressed(CMD_JUMP)) ||
                    ( _menuHeroSelector->PlayerReady(4) && _input4.isKeyPressed(CMD_JUMP))) &&
                     _menuHeroSelector->MinTwoPlayerOK())
                {
					//ManagePlayerCreation();
					_transitionBlack.setFillColor(Color(0, 0, 0, 255));
					_threadLoadPlayers->launch();
					_maps->LoadMap("environment//mapFiles//map.xml");
                    _gameState = GAME;
                    _soundBox.PlayMusic(MUSIC::HHavok);
                }
            }
            break;
        case GAME:
            if(_input0.isKeyPressed(CMD_PAUSE) && !_menuKeyPressed && !_maps->IsMapLoading())
            {
                _menuKeyPressed = true;
                _yesNoMenu->ResetAnimation(true);
                _gameState = PAUSE;
            }
            break;
        case GAME_END:
            if((_input0.isKeyPressed(CMD_VALID) || _input0.isKeyPressed(CMD_PAUSE)) &&
               !_menuKeyPressed && _scoreManager->isAnimationFinished())
            {
                for(int i=0 ; i<15 ; i++)
                {
                    for(int j=0 ; j<20 ; j++)
                    {
                        _transitionShapes[i][j].setScale(0,0);
                        _transitionShapes[i][j].setRotation(0);
                    }
                }
                _resultText.setPosition(Vector2f(640,-100));
                _menuKeyPressed = true;
                _gameState = HERO_SELECT;
                _soundBox.PlayMusic(MUSIC::Chibi_Ninja);
            }
            break;
        case PAUSE:
            if((_input0.isKeyPressed(CMD_BACK) || _input0.isKeyPressed(CMD_PAUSE)) && !_menuKeyPressed)
            {
                _pauseMenu->ResetAnimation();
                _menuKeyPressed = true;
                _gameState = GAME;
            }
            break;
        case YESNO:
            if(_input0.isKeyPressed(CMD_BACK) && !_menuKeyPressed)
            {
                _menuKeyPressed = true;
                _yesNoMenu->ResetAnimation(true);
                _gameState = _lastGameState;
            }
            break;
    }
    if(!Keyboard::isKeyPressed(Keyboard::Num0) &&
       !_input0.isKeyPressed(CMD_BACK) && !_input0.isKeyPressed(CMD_PAUSE) &&
       !_input0.isKeyPressed(CMD_VALID) && !Mouse::isButtonPressed(Mouse::Left) &&
       !_input1.isKeyPressed(CMD_COUNTERACT) && !_input2.isKeyPressed(CMD_COUNTERACT) &&
       !_input3.isKeyPressed(CMD_COUNTERACT) && !_input4.isKeyPressed(CMD_COUNTERACT) &&
       !_input1.isKeyPressed(CMD_JUMP) && !_input2.isKeyPressed(CMD_JUMP) &&
       !_input3.isKeyPressed(CMD_JUMP) && !_input4.isKeyPressed(CMD_JUMP))
       _menuKeyPressed = false;
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
                _gameState = HERO_SELECT;
                break;
            case 1:
                _menuKeyPressed = true;
                _mainMenu->ResetAnimation(false);
				_mainOptions->SetPosition(Vector2f(640, 440), ALIGN_NOCHANGE, BG_HIDE);
                _mainOptions->UpdateOptions(xmlReader::getSettingsValue(), false);
				_lastGameState = MAIN_MENU;
                _gameState = MAIN_OPTIONS;
                break;
            case 2:
                _menuKeyPressed = true;
                _mainMenu->ResetAnimation(false);
				_mainControls->SetPosition(Vector2f(640, 440), ALIGN_NOCHANGE, BG_HIDE);
				_lastGameState = MAIN_MENU;
                _gameState = MAIN_COMMANDS;
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
                _gameState = OPTION_LANG;
				_currentOptionMenu = _optionLanguage;
                _optionLanguage->FocusText(_settings->_language);
                break;
            case 1:
                _menuKeyPressed = true;
                _gameState = OPTION_SCREEN;
				_currentOptionMenu = _optionScreen;
                _optionScreen->FocusText(_settings->_fullscreen);
                break;
            case 2:
                _menuKeyPressed = true;
                _gameState = OPTION_TIME;
				_currentOptionMenu = _optionTime;
                _optionTime->FocusText(_settings->_timeString);
                break;
			case 3:
				_menuKeyPressed = true;
				_gameState = OPTION_ANALOG;
				_currentOptionMenu = _optionScreen;
				_optionScreen->FocusText(_settings->_analog);
				break;
            case 4:
                _menuKeyPressed = true;
                _gameState = OPTION_LIFE;
				_currentOptionMenu = _optionLife;
                _optionLife->FocusText(_settings->_lifeString);
                break;
            case 5:
                _menuKeyPressed = true;
                _gameState = OPTION_MUSIC;
				_currentOptionMenu = _optionMusic;
                _optionMusic->FocusText(_settings->_musicString);
                break;
            case 6:
                _menuKeyPressed = true;
                _gameState = OPTION_SFX;
				_currentOptionMenu = _optionEffects;
                _optionEffects->FocusText(_settings->_effectsString);
                break;
            case 7:
                _menuKeyPressed = true;
                _mainOptions->ResetAnimation();
                _gameState = _lastGameState;
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
            case OPTION_LANG:{
                if(_optionLanguage->GetSelectedText() == "")
                    return;
                _settings->_language = _optionLanguage->GetSelectedText();
                RedrawUI();}
                break;
            case OPTION_SCREEN:{
                if(_optionScreen->GetSelectedText() == "")
                    return;
                _settings->_fullscreen = _optionScreen->GetSelectedText();
                ChangeScreenMode();}
                break;
            case OPTION_TIME:{
                if(_optionTime->GetSelectedText() == "")
                    return;
                string time = _optionTime->GetSelectedText().substr(0,2);
                _settings->_timeInSec = atoi(time.c_str())*60;
                _settings->_timeString = _optionTime->GetSelectedText(); }
                break;
			case OPTION_ANALOG: {
				if (_optionScreen->GetSelectedText() == "")
					return;
				_settings->_analog = _optionScreen->GetSelectedText();
				_input1.SetAnalogControls(_settings->_analog == "ON");
				_input2.SetAnalogControls(_settings->_analog == "ON");
				_input3.SetAnalogControls(_settings->_analog == "ON");
				_input4.SetAnalogControls(_settings->_analog == "ON"); }
				break;
            case OPTION_LIFE:{
                if(_optionLife->GetSelectedText() == "")
                    return;
                _settings->_lifePoints = atoi(_optionLife->GetSelectedText().c_str());
                _settings->_lifeString = _optionLife->GetSelectedText();}
                break;
            case OPTION_MUSIC:{
                if(_optionMusic->GetSelectedText() == "")
                    return;
                _settings->_music = atoi(_optionMusic->GetSelectedText().c_str());
                _settings->_musicString = _optionMusic->GetSelectedText();
                _soundBox.UpdateVolumes(_settings);}
                break;
            case OPTION_SFX:{
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
        _gameState = MAIN_OPTIONS;
    }
}

void mainGame::MainControlsEvents()
{
    if(ValidMenu(_mainControls) && !_menuKeyPressed)
    {
        switch(_mainControls->GetSelectedItem())
        {
            case 0:
                _menuKeyPressed = true;
				_padMenu->ShowBackground(_lastGameState == PAUSE);
				_padMenu->SetPosition(_lastGameState == PAUSE ? 360 : 440);
                _gameState = SHOW_GAMEPAD;
                _mainControls->ResetAnimation();
                break;
            case 5:
                _menuKeyPressed = true;
                _gameState = _lastGameState;
                _mainControls->ResetAnimation();
                break;
            case 1:case 2:case 3:case 4:
                _menuKeyPressed = true;
                stringstream stream;
                stream << _mainControls->GetSelectedItem();
				_keyConfig->ShowBackground(_lastGameState == PAUSE);
				_keyConfig->SetPosition(Vector2f(640, _lastGameState == PAUSE ? 360 : 440));
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
                _gameState = CONFIG_KEYS;
                break;
        }
    }
}

void mainGame::KeyConfigEvents()
{
    if(_keyConfig->GetSelectedItem() < 9)
    {
        if(ValidMenu(_keyConfig) && !_menuKeyPressed && !_waitingKey)
        {
            _menuKeyPressed = true;
            _keyConfig->SetTextOption(_keyConfig->GetSelectedItem(), "...");
            _waitingKey = true;
        }
        else if(_waitingKey && !_menuKeyPressed)
        {
            if(_globalKeyPressed)
            {
                _menuKeyPressed = true;
                _editingInput->SetKey(_keyConfig->GetSelectedItem(), _currentKey);
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
        _gameState = MAIN_COMMANDS;
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
                for(int i=0 ; i<_players.size() ; i++)
					_players[i]->SetHasJumped(true);
                _gameState = GAME;
                break;
            case 1:case 4: //Validation YES-NO
                _menuKeyPressed = true;
                _yesNoMenu->ResetAnimation(true);
                _lastGameState = PAUSE;
                _yesnoShape.setPosition(utils::VIEW_WIDTH, _yesnoShape.getPosition().y);
                _yesnoText.setPosition(0, _yesnoText.getPosition().y);
                _gameState = YESNO;
                break;
			case 2:
				_menuKeyPressed = true;
				_pauseMenu->ResetAnimation(false);
				_mainOptions->SetPosition(Vector2f(640, 360), ALIGN_NOCHANGE, BG_SHOW);
				_mainOptions->UpdateOptions(xmlReader::getSettingsValue(), false);
				_lastGameState = PAUSE;
				_gameState = MAIN_OPTIONS;
				break;
			case 3:
				_menuKeyPressed = true;
				_pauseMenu->ResetAnimation(false);
				_mainControls->SetPosition(Vector2f(640, 360), ALIGN_NOCHANGE, BG_SHOW);
				_lastGameState = PAUSE;
				_gameState = MAIN_COMMANDS;
				break;
            case 5:
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
                    for(int i=0 ; i<_players.size() ; i++)
						_players[i]->ResetHero(_players[i]->GetColorID(), _settings->_lifePoints);
					_projectileBox->DestroyEverything();
                    _scoreManager->_players = _players;
                    _scoreManager->Reset(_settings->_timeInSec, _settings->_language);
                    _menuHeroSelector->ResetMenu();
                    _transitionBlack.setFillColor(Color(0, 0, 0));
                    _gameState = GAME;
                    _pauseMenu->ResetAnimation(true);
                    _soundBox.PlayMusic(MUSIC::HHavok);
                }
                else if(_pauseMenu->GetSelectedItem() == 4)
                {
                    _menuKeyPressed = true;
                    _mainMenu->ResetAnimation();
                    _pauseMenu->ResetAnimation(true);
                    _gameState = MAIN_MENU;
                    _soundBox.PlayMusic(MUSIC::Chibi_Ninja);
                }
                break;
            case 1:
                _menuKeyPressed = true;
                _yesNoMenu->ResetAnimation(true);
                _gameState = _lastGameState;
                break;
        }
    }
}
/////////////////////////////////////

void mainGame::RedrawUI()
{
	//Reset UI texts
	_settings->_language = _optionLanguage->GetSelectedText();
	_mainMenu = new menuList(xmlReader::getTranslationTexts("menuMain", _settings->_language), Vector2f(640, 440), &_input0, false);
	_mainControls = new menuList(xmlReader::getTranslationTexts("menuControls", _settings->_language), _mainControls->GetPosition(), &_input0, _mainControls->GetBackgroundVisibility());
	_mainOptions = new menuOptions(xmlReader::getTranslationTexts("settings", _settings->_language), _mainOptions->GetPosition(), &_input0, _mainOptions->GetBackgroundVisibility());
	_mainOptions->UpdateOptions(xmlReader::getSettingsValue(), false);
	_optionLanguage = new menuList(_settings->getSettingsChoices(CHOICE_LANG), Vector2f(640, 440), &_input0, true, ALIGN_MIDDLE, _mainOptions);
    _optionScreen = new menuList(_settings->getSettingsChoices(CHOICE_ON_OFF), Vector2f(640, 440), &_input0, true, ALIGN_MIDDLE, _mainOptions);
    _optionTime = new menuList(_settings->getSettingsChoices(CHOICE_TIME), Vector2f(640, 440), &_input0, true, ALIGN_MIDDLE, _mainOptions);
    _optionLife = new menuList(_settings->getSettingsChoices(CHOICE_LIFE), Vector2f(640, 440), &_input0, true, ALIGN_MIDDLE, _mainOptions);
    _optionMusic = new menuList(_settings->getSettingsChoices(CHOICE_SOUND), Vector2f(640, 440), &_input0, true, ALIGN_MIDDLE, _mainOptions);
    _optionEffects = new menuList(_settings->getSettingsChoices(CHOICE_SOUND), Vector2f(640, 440), &_input0, true, ALIGN_MIDDLE, _mainOptions);
	_keyConfig = new menuOptions(xmlReader::getTranslationTexts("key", _settings->_language), Vector2f(640, 440), &_input0, _keyConfig->GetBackgroundVisibility());
	_pauseMenu = new menuList(xmlReader::getTranslationTexts("pause", _settings->_language), Vector2f(640, 360), &_input0, true);
	_padMenu = new menuPad(420, _settings->_language, &_input0);
	_menuHeroSelector = new menuHeroSelector(_settings->_language, _inputs, &_soundBox);
	_yesNoMenu = new menuList(xmlReader::getTranslationTexts("yesno", _settings->_language), Vector2f(640, 360), &_input0, true);
	_heroSelectText.setString(xmlReader::getTranslationText("miscellaneous", _settings->_language, "heroSelect"));
	_heroSelectText.setOrigin(_heroSelectText.getLocalBounds().width / 2, _heroSelectText.getLocalBounds().height / 2);
	_heroSelectText.setPosition(Vector2f(640, -100));
	_resultText.setString(xmlReader::getTranslationText("miscellaneous", _settings->_language, "results"));
	_resultText.setOrigin(_resultText.getLocalBounds().width / 2, _resultText.getLocalBounds().height / 2);
	_resultText.setPosition(Vector2f(640, -100));
	_yesnoText.setString(xmlReader::getTranslationText("miscellaneous", _settings->_language, "confirm"));
	_yesnoText.setOrigin(_yesnoText.getLocalBounds().width / 2, _yesnoText.getLocalBounds().height / 2);
}

void mainGame::ChangeScreenMode()
{
	//Getting screen resolution
	Vector2i screenSize;
	if (_settings->_fullscreen == "ON")
		screenSize = Vector2i(VideoMode::getDesktopMode().width, VideoMode::getDesktopMode().height);
	else
		screenSize = Vector2i(utils::VIEW_WIDTH, utils::VIEW_HEIGHT);

	//Recreating window
	ContextSettings ctxSettings;
	ctxSettings.antialiasingLevel = 0;
	_window->create(VideoMode(screenSize.x, screenSize.y, 32), "EggFall",
		_settings->_fullscreen == "ON" ? Style::Fullscreen : Style::Default, ctxSettings);

	//View
	FloatRect rect;
	if (utils::VIEW_HEIGHT*screenSize.x / screenSize.y >= utils::VIEW_WIDTH)
		rect = FloatRect(0, 0, utils::VIEW_HEIGHT*screenSize.x / screenSize.y, utils::VIEW_HEIGHT);
	else
		rect = FloatRect(0, 0, utils::VIEW_WIDTH, utils::VIEW_WIDTH * screenSize.y / screenSize.x);
	View view(rect); //1280x720 native view
	view.setCenter(640, 360);
	_window->setView(view);
}

void mainGame::DisplayGameResources()
{
	//Drawing background elements of map
    _maps->Display(_window, _deltaTime, _isPlayersLoading);
	if (_maps->IsMapLoading() || _isPlayersLoading)
		return;

	//Black transition
	_window->draw(_transitionBlack);
	if (_transitionBlack.getFillColor().a > 0)
		_transitionBlack.setFillColor(Color(0, 0, 0, _transitionBlack.getFillColor().a - 5));

	//Drawing non dashing players
    for(int i=0 ; i<_players.size() ; i++)
		if(!_players[i]->IsDashing())
			_players[i]->Display(_window, _deltaTime);
	//Drawing dashing players above others
	for (int i = 0; i<_players.size(); i++)
		if (_players[i]->IsDashing())
			_players[i]->Display(_window, _deltaTime);

    //Scores / effects / projectiles
    _scoreManager->Display(_window);
    _effectBox.Display(_window, (_settings->_fullscreen == "ON"), _deltaTime);
    _projectileBox->Display(_window);
}

void mainGame::DisplaySettingsResources()
{
	if (_lastGameState == PAUSE)
	{
		DisplayGameResources();
		_projectileBox->Display(_window);
		_effectBox.Display(_window, (_settings->_fullscreen == "ON"), _deltaTime);
		_scoreManager->Display(_window);
	}
	else
	{
		_window->draw(_titleScreenBackground);
		_window->draw(_titleScreenTitle);
		for (int i = 0; i<5; i++)
			_window->draw(_titleScreenHats[i]);
	}
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
        if(_titleScreenHats[i].getPosition().x < 514 + i * 50 &&
           !_input0.isKeyPressed(CMD_VALID) && !Mouse::isButtonPressed(Mouse::Left))
        {
            Vector2f pos = _titleScreenHats[i].getPosition();
            _titleScreenHats[i].setPosition(pos.x + 60.f*_deltaTime, pos.y);
        }
        else
        {
            _menuKeyPressed = true;
            _titleScreenHats[i].setPosition(514 + i * 50, _titleScreenHats[i].getPosition().y);
            _titleAnimationFrame ++;
        }
        return false;
    }
    else if(_gameState != HERO_SELECT && _titleScreenTitle.getPosition().y < 100)
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
    if(_gameState != HERO_SELECT && _titleScreenHats[0].getPosition().y < 210)
    {
        for(int i=0 ; i<5 ; i++)
        {
            if(_titleScreenHats[i].getPosition().y < 210)
                _titleScreenHats[i].move(0,10*_deltaTime);
            else
                _titleScreenHats[i].setPosition(_titleScreenHats[i].getPosition().x, 210);
        }
    }
    else if(_gameState == HERO_SELECT && _titleScreenHats[0].getPosition().y > 120)
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
	_gameState = _gameState == GAME ? PAUSE : _gameState;
}

void mainGame::Render(float deltaTime)
{
    //Update delta time
    _deltaTime = deltaTime;

    //Update last key (keyboard or pad)
    _input0.PerformLastController(_window);

    //Main switch
    switch(_gameState)
    {
        case GAME:
			if (_maps->IsMapLoading() || _isPlayersLoading)
				break;

			//Step
            _world->Step(_deltaTime/60.f, 60, 20);
            Box2DTools::ManageBodyLoop(_world);
            _maps->RenderMap(_deltaTime);
            _soundBox.PerformSounds();
            for(int i=0 ; i<_players.size() ; i++)
				_players[i]->Step(_deltaTime);
			_projectileBox->RenderProjectiles(_deltaTime);
			_effectBox.RenderEffects(_world, (_settings->_fullscreen == "ON"), _deltaTime);
            _scoreManager->Step(_deltaTime);

			//Inputs step
			for (int i = 0; i < _inputList.size(); i++)
				_inputList[i]->Step(_deltaTime);
            break;
    }
}

void mainGame::Display(Keyboard::Key key, bool keyPressed, float deltaTime)
{
    //Updating variables
    _currentKey = key;
    _globalKeyPressed = keyPressed;
    _deltaTime = deltaTime;

	//Switch fullscreen mode with ALT+RETURN
	if ((Keyboard::isKeyPressed(Keyboard::LAlt) || Keyboard::isKeyPressed(Keyboard::RAlt)) &&
		Keyboard::isKeyPressed(Keyboard::Return) && !_menuKeyPressed)
	{
		_menuKeyPressed = true;
		_settings->_fullscreen = _settings->_fullscreen == "ON" ? "OFF" : "ON";
		_settings->save();
		_mainOptions->UpdateOptions(xmlReader::getSettingsValue(), false);
		ChangeScreenMode();
	}

    //Playing music
    if(IsMusicPaused())
        PauseMusic(false);

    //Events
    OpeningClosingMenu();
    MenuAnimation();

    //Main switch
    switch(_gameState)
    {
        case MAIN_MENU:
            _window->draw(_titleScreenBackground);
            _window->draw(_titleScreenTitle);
            for(int i=0 ; i<5 ; i++)
                _window->draw(_titleScreenHats[i]);
            if(TitleAnimation())
            {
                MainMenuEvents();
                _mainMenu->Display(_window, _deltaTime);
            }

			//EDITOR TEST
			if (Keyboard::isKeyPressed(Keyboard::E))
				_gameState = EDITOR;
            break;
        case MAIN_OPTIONS:
            MainOptionsEvents();
			DisplaySettingsResources();
            _mainOptions->Display(_window, deltaTime);
            break;
		case MAIN_COMMANDS:
			MainControlsEvents();
			DisplaySettingsResources();
			_mainControls->Display(_window, deltaTime);
			break;
		case OPTION_LANG:case OPTION_SCREEN:case OPTION_TIME:case OPTION_ANALOG:case OPTION_LIFE:case OPTION_MUSIC:case OPTION_SFX:
            OptionsEvents();
			DisplaySettingsResources();
            _mainOptions->Display(_window, deltaTime, false);
            if(_gameState == OPTION_LANG) { _optionLanguage->Display(_window, deltaTime, true, false); }
            if(_gameState == OPTION_SCREEN || _gameState == OPTION_ANALOG)   { _optionScreen->Display(_window, deltaTime, true, false); }
            if(_gameState == OPTION_TIME)     { _optionTime->Display(_window, deltaTime, true, false); }
            if(_gameState == OPTION_LIFE)     { _optionLife->Display(_window, deltaTime, true, false); }
            if(_gameState == OPTION_MUSIC)    { _optionMusic->Display(_window, deltaTime, true, false); }
            if(_gameState == OPTION_SFX)      { _optionEffects->Display(_window, deltaTime, true, false); }
            break;
		case CONFIG_KEYS:
			if(_waitingKey)
				_keyConfig->ForceKeyPressed(true); //Avoid moving up or down in menu after changing key
			KeyConfigEvents();
			DisplaySettingsResources();
			_keyConfig->Display(_window, deltaTime, !_waitingKey);
            break;
        case SHOW_GAMEPAD:
			DisplaySettingsResources();
            _padMenu->Display(_window, deltaTime);
            break;
        case HERO_SELECT:
            //Step
            //_world->Step(_deltaTime/60.f, 8, 3);
            _maps->RenderMap(deltaTime);
            _effectBox.RenderEffects(_world, (_settings->_fullscreen == "ON"), deltaTime);

            //Animations
            if(_heroSelectText.getPosition().y < 20)
                _heroSelectText.move(0, 10*deltaTime);
            else
                _heroSelectText.setPosition(640,20);
            if(_titleScreenTitle.getPosition().y > -100)
                _titleScreenTitle.move(0, -20*deltaTime);
            else
                _titleScreenTitle.setPosition(640,-100);

            //Draw
            _window->draw(_titleScreenBackground);
            _window->draw(_titleScreenTitle);
            _window->draw(_heroSelectText);
            for(int i=0 ; i<5 ; i++)
                _window->draw(_titleScreenHats[i]);
            _menuHeroSelector->Display(_window, deltaTime);
            break;
        case GAME:
			//Slow-Motion end of game
			/*if (_scoreManager->timePassed() >= (double)_settings->_timeInSec - 5)
				_deltaTime = ((double)_settings->_timeInSec - _scoreManager->timePassed()) * 0.2;
			if(Keyboard::isKeyPressed(Keyboard::Space))
				_deltaTime *= 0.2;*/

            DisplayGameResources();
			if (_maps->IsMapLoading() || _isPlayersLoading)
				break;

            //Score
            if(_scoreManager->timePassed() >= (double)_settings->_timeInSec-6.01 &&
               _scoreManager->timePassed() <= (double)_settings->_timeInSec-5.99)
                _soundBox.PlaySound(SOUND::SND_TICK, false, 36);
            else if(_scoreManager->timePassed() >= (float)_settings->_timeInSec)
            {
                _titleScreenBackground.setColor(Color(255,255,255,0));
                _scoreManager->Reset(0, _settings->_language, true);
                _gameState = GAME_END;
                _soundBox.PlayMusic(MUSIC::All_of_Us);
            }
            break;
        case GAME_END:
            DisplayGameResources();
			_projectileBox->Display(_window);
            _effectBox.Display(_window, (_settings->_fullscreen == "ON"), deltaTime);
            for(int i=0 ; i<15 ; i++)
                for(int j=0 ; j<20 ; j++)
                    _window->draw(_transitionShapes[i][j]);
            _window->draw(_titleScreenBackground);
            if(GameEndedAnimation())
                _scoreManager->Display(_window, true);
            _window->draw(_resultText);
            break;
        case PAUSE:
            DisplayGameResources();
            PauseMenuEvents();
			_projectileBox->Display(_window);
            _effectBox.Display(_window, (_settings->_fullscreen == "ON"), deltaTime);
            _scoreManager->Display(_window);
            _pauseMenu->Display(_window, deltaTime);
            break;
        case YESNO:
            if(_lastGameState == PAUSE)
            {
                DisplayGameResources();
				_projectileBox->Display(_window);
                _effectBox.Display(_window, (_settings->_fullscreen == "ON"), deltaTime);
                _scoreManager->Display(_window);
                _pauseMenu->Display(_window, deltaTime, false);
            }
            YesNoMenuEvents();
            _yesNoMenu->Display(_window, deltaTime, true, false);

			//YesNo confirmation animation
			if (_yesnoShape.getPosition().x > 0)
				_yesnoShape.setPosition(_yesnoShape.getPosition().x - (100.f*deltaTime), _yesnoShape.getPosition().y);
			else
				_yesnoShape.setPosition(0, _yesnoShape.getPosition().y);
            if(_yesnoText.getPosition().x < 640)
                _yesnoText.setPosition(_yesnoText.getPosition().x+(50.f*deltaTime), _yesnoText.getPosition().y);
			else
				_yesnoText.setPosition(640, _yesnoText.getPosition().y);
            _window->draw(_yesnoShape);
            _window->draw(_yesnoText);
            break;
		case EDITOR:
			_effectBox.RenderEffects(_world, (_settings->_fullscreen == "ON"), deltaTime);
			_effectBox.Display(_window, (_settings->_fullscreen == "ON"), deltaTime);
			_maps->EditMap(_window, deltaTime, true);
			if (Keyboard::isKeyPressed(Keyboard::Escape))
			{
				_gameState = MAIN_MENU;
				_window->setMouseCursorVisible(true);
			}
			break;
    }

	/*if (_frameScreenshot == 0.f && fabs(_players[0]->_attacking) > 0.f)
		_frameScreenshot = fabs(_players[0]->_attacking) + 10.f;
	if(_frameScreenshot > 0.f)
	{
		_frameScreenshot -= 1.f;
		int nb = 37 - (int)fabs(_players[0]->_attacking);
		stringstream stream;
		stream << nb;
		Vector2u windowSize = _window->getSize();
		Texture texture;
		texture.create(windowSize.x, windowSize.y);
		texture.update(*_window);
		//100, 120, 340, 100
		Image screenshot = texture.copyToImage();
		Image screenCrop;
		screenCrop.create(340, 100);
		for (int y = 120; y < 220; y++)
			for (int x = 100; x < 440; x++)
				screenCrop.setPixel(x - 100, y - 120, screenshot.getPixel(x, y));

		screenCrop.saveToFile("C:\\Users\\JDA\\Desktop\\sc\\sc_" + stream.str() + ".png");
	}*/

	//DEBUG FUNCTION (SHOW HITBOXES)
    _showMasks = Keyboard::isKeyPressed(Keyboard::M) ? true : _showMasks;
    _showMasks = Keyboard::isKeyPressed(Keyboard::L) ? false : _showMasks;
    if(_showMasks && _gameState == GAME)
        Box2DTools::DrawCollisionMask(_world, _window);

    //DEBUG FUNCTION (SHOW FPS)
    _showFps = Keyboard::isKeyPressed(Keyboard::P) ? true : _showFps;
    _showFps = Keyboard::isKeyPressed(Keyboard::O) ? false : _showFps;
    if(_showFps)
    {
        stringstream stream;
        stream << (int)(60.f/deltaTime);
        _frameRate.setString(stream.str());
        _window->draw(_frameRate);
    }
}
