#include "mainGame.hpp"

static const float SCALE = 30.f;

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
	_loadNewGame = true;
    _showMasks = false;
    _showFps = false;
	_bridgeDeleteTimeout = 0.f;
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
	_inputUI = input(-1, 30, _window, _settings->_analog == "ON"); //Menu input
	_inputsPlayers.push_back(new input(0, 30, _window, _settings->_analog == "ON"));
	_inputsPlayers.push_back(new input(1, 30, _window, _settings->_analog == "ON"));
	_inputsPlayers.push_back(new input(2, 30, _window, _settings->_analog == "ON"));
	_inputsPlayers.push_back(new input(3, 30, _window, _settings->_analog == "ON"));

    //Box2D - World
    _world = new b2World(b2Vec2(0.f, 42.f));
    _maps = new maps(_world, &_effectBox, &_inputUI, &_AIs);
	_maps->LoadMap("environment//mapFiles//map.xml");
	_contactListener = new contactListener(_world); //Override PreSolve event using custom contact listener

    //Spells - Music
    _projectileBox = new projectiles(_world, &_effectBox, &_soundBox, _maps);
	_projectileBox->UpdatePlayersPointers(&_players);
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

	//Players useful textures
	utils::LoadTextureSafely(files::lifeStamina, &_lifeStaminaTexture);
	utils::LoadTextureSafely(files::aimArrow, &_aimArrowTexture);

    //UI
    _mainMenu = new menuList(xmlReader::getTranslationTexts("menuMain", _settings->_language), Vector2f(640,440), &_inputUI, false);
	_mainOptions = new menuOptions(xmlReader::getTranslationTexts("settings", _settings->_language), Vector2f(640, 440), &_inputUI, false);
    _mainControls = new menuList(xmlReader::getTranslationTexts("menuControls", _settings->_language), Vector2f(640, 440), &_inputUI, false);
    _optionLanguage = new menuList(_settings->getSettingsChoices(CHOICE_LANG), Vector2f(640, 440), &_inputUI, true, ALIGN_MIDDLE, _mainOptions);
    _optionScreen = new menuList(_settings->getSettingsChoices(CHOICE_ON_OFF), Vector2f(640, 440), &_inputUI, true, ALIGN_MIDDLE, _mainOptions);
    _optionTime = new menuList(_settings->getSettingsChoices(CHOICE_TIME), Vector2f(640, 440), &_inputUI, true, ALIGN_MIDDLE, _mainOptions);
    _optionLife = new menuList(_settings->getSettingsChoices(CHOICE_LIFE), Vector2f(640, 440), &_inputUI, true, ALIGN_MIDDLE, _mainOptions);
    _optionMusic = new menuList(_settings->getSettingsChoices(CHOICE_SOUND), Vector2f(640, 440), &_inputUI, true, ALIGN_MIDDLE, _mainOptions);
    _optionEffects = new menuList(_settings->getSettingsChoices(CHOICE_SOUND), Vector2f(640, 440), &_inputUI, true, ALIGN_MIDDLE, _mainOptions);
    _keyConfig = new menuOptions(xmlReader::getTranslationTexts("key", _settings->_language), Vector2f(640, 440), &_inputUI, false);
    _pauseMenu = new menuList(xmlReader::getTranslationTexts("pause", _settings->_language), Vector2f(640, 360), &_inputUI, true);
    _yesNoMenu = new menuList(xmlReader::getTranslationTexts("yesno", _settings->_language), Vector2f(640, 360), &_inputUI, true);
    _padMenu = new menuPad(420, _settings->_language, &_inputUI);
    _scoreManager->Reset(_settings->_timeInSec,_settings->_language);
    _scoreManager->UpdatePlayersPointers(&_players);
	_currentOptionMenu = NULL;

    //UI SELECTOR
    _menuHeroSelector = new menuHeroSelector(_settings->_language, _inputsPlayers, &_soundBox);

	//Thread
	_threadLoadPlayers = new Thread(bind(&ManagePlayerCreation, &_isPlayersLoading, &_loadNewGame, &_players,
		&_inputsPlayers, _world, &_effectBox, &_soundBox, _menuHeroSelector, _projectileBox, _settings, _scoreManager,
		&_playersTexture, &_playersFlashTexture, &_lifeStaminaTexture, &_aimArrowTexture));

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

void ManagePlayerCreation(bool * isPlayersLoading, bool * loadNewGame, vector<hero*> * players, vector<input*> * inputsPlayers,
	b2World * world, effects * effectBox, sound * soundBox, menuHeroSelector * menuHeroSelector, projectiles * projectileBox,
	settings * settings, scoreManager * scoreManager, vector<Texture> * playersTexture, vector<Texture> * playersFlashTexture,
	Texture * lifeStaminaTexture, Texture * aimArrowTexture)
{
	//START
	*isPlayersLoading = true;

	//Load textures (if not already loaded)
	if (playersTexture->size() == 0)
	{
		//Add void texture
		for (int i = 0; i < 5; i++)
			playersTexture->push_back(Texture());
		playersFlashTexture->push_back(Texture());

		//Load textures
		utils::LoadTextureSafely(files::wizardTextureA, &(*playersTexture)[0]);
		utils::LoadTextureSafely(files::wizardTextureB, &(*playersTexture)[1]);
		utils::LoadTextureSafely(files::wizardTextureC, &(*playersTexture)[2]);
		utils::LoadTextureSafely(files::wizardTextureD, &(*playersTexture)[3]);
		utils::LoadTextureSafely(files::wizardTextureE, &(*playersTexture)[4]);

		//Load flash textures
		(*playersFlashTexture)[0].loadFromImage(utils::ColorizeImage(files::wizardTextureA, Color::White));
	}

	//Destroy players
	for (int i = 0; i < players->size(); i++)
	{
		if ((*players)[i] != NULL)
		{
			if (*loadNewGame)
			{
				delete (*players)[i];
				(*players)[i] = NULL;
			}
			else //Restart game
			{
				hero * heroClone = (*players)[i];
				(*players)[i] = new wizard(heroClone->GetColorID(), settings->_lifePoints, heroClone->GetInitPos(), world,
					heroClone->GetInput(), effectBox, projectileBox, soundBox, scoreManager, heroClone->GetHeroTexture(),
					heroClone->GetHeroTextureFlash(), lifeStaminaTexture, aimArrowTexture);
				delete heroClone;
			}
		}
	}
	if (*loadNewGame)
		players->clear();

	//Destroy projectiles and effects
	projectileBox->DestroyEverything();
	effectBox->DestroyEverything();

	//Recreate players for the game
	if (*loadNewGame)
	{
		Texture * heroFlashTexture = &(*playersFlashTexture)[0];
		vector<pair<int, int>> playersInTheGame = menuHeroSelector->GetSelectedItems();
		for (int i = 0; i < playersInTheGame.size(); i++)
		{
			int numPlayer = playersInTheGame[i].first-1;
			int colorID = playersInTheGame[i].second;
			Texture * heroTexture = &(*playersTexture)[colorID];
			players->push_back(new wizard(colorID, settings->_lifePoints, Vector2f(185 + 100 * i, 145), world, (*inputsPlayers)[numPlayer], effectBox,
				projectileBox, soundBox, scoreManager, heroTexture, heroFlashTexture, lifeStaminaTexture, aimArrowTexture));
		}
	}

	//Update objects references
	for (int i = 0; i < players->size(); i++)
		if ((*players)[i] != NULL)
			(*players)[i]->_players = *players;

	//Reset
	scoreManager->Reset(settings->_timeInSec, settings->_language);
	menuHeroSelector->ResetMenu();

	//END
	*isPlayersLoading = false;
}

/////////////// EVENTS METHODS ////////////////////
bool mainGame::ValidMenu(menuList* menu)
{
    if((_inputUI.isKeyPressed(CMD_VALID) ||
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
            if(_inputUI.isKeyPressed(CMD_VALID) || Mouse::isButtonPressed(Mouse::Left))
               _gameState = MAIN_MENU;
            break;
        case MAIN_OPTIONS:
            if((_inputUI.isKeyPressed(CMD_BACK) || _inputUI.isKeyPressed(CMD_PAUSE)) && !_menuKeyPressed)
            {
                _menuKeyPressed = true;
                _mainOptions->ResetAnimation();
                _gameState = _lastGameState;
            }
            break;
		case OPTION_LANG:case OPTION_SCREEN:case OPTION_TIME:case OPTION_ANALOG:case OPTION_LIFE:case OPTION_MUSIC:case OPTION_SFX:
			if ((_inputUI.isKeyPressed(CMD_BACK) || _inputUI.isKeyPressed(CMD_PAUSE) || (Mouse::isButtonPressed(Mouse::Left) &&
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
            if((_inputUI.isKeyPressed(CMD_BACK) || _inputUI.isKeyPressed(CMD_PAUSE)) && !_menuKeyPressed)
            {
                _menuKeyPressed = true;
                _mainControls->ResetAnimation();
                _gameState = _lastGameState;
            }
            break;
        case CONFIG_KEYS:
            if((_inputUI.isKeyPressed(CMD_BACK) || _inputUI.isKeyPressed(CMD_PAUSE)) && !_menuKeyPressed && !_waitingKey)
            {
                _menuKeyPressed = true;
                _keyConfig->ResetAnimation();
                _gameState = MAIN_COMMANDS;
            }
            break;
        case SHOW_GAMEPAD:
            if((_inputUI.isKeyPressed(CMD_BACK) || _inputUI.isKeyPressed(CMD_PAUSE) ||
                (_padMenu->_backTextFocused && Mouse::isButtonPressed(Mouse::Left))) && !_menuKeyPressed)
            {
                _menuKeyPressed = true;
                _padMenu->ResetAnimation();
                _gameState = MAIN_COMMANDS;
            }
            break;
        case HERO_SELECT:
            if(!_menuKeyPressed && (_inputsPlayers[0]->isKeyPressed(CMD_COUNTERACT) ||
				_inputsPlayers[1]->isKeyPressed(CMD_COUNTERACT) || _inputsPlayers[2]->isKeyPressed(CMD_COUNTERACT) ||
				_inputsPlayers[3]->isKeyPressed(CMD_COUNTERACT) || _inputUI.isKeyPressed(CMD_BACK) ||
				_inputUI.isKeyPressed(CMD_PAUSE)))
            {
                _menuKeyPressed = true;
                if((_menuHeroSelector->PlayerWaiting(1) && _inputsPlayers[0]->isKeyPressed(CMD_COUNTERACT)) ||
                   (_menuHeroSelector->PlayerWaiting(2) && _inputsPlayers[1]->isKeyPressed(CMD_COUNTERACT)) ||
                   (_menuHeroSelector->PlayerWaiting(3) && _inputsPlayers[2]->isKeyPressed(CMD_COUNTERACT)) ||
                   (_menuHeroSelector->PlayerWaiting(4) && _inputsPlayers[3]->isKeyPressed(CMD_COUNTERACT)) ||
                    _inputUI.isKeyPressed(CMD_PAUSE) || _inputUI.isKeyPressed(CMD_BACK))
                {
                    _mainMenu->ResetAnimation();
                    _heroSelectText.setPosition(640,-100);
                    _gameState = MAIN_MENU;
                }
            }
            else if(!_menuKeyPressed && (_inputsPlayers[0]->isKeyPressed(CMD_JUMP) ||
				_inputsPlayers[1]->isKeyPressed(CMD_JUMP) || _inputsPlayers[2]->isKeyPressed(CMD_JUMP) ||
				_inputsPlayers[3]->isKeyPressed(CMD_JUMP) || _inputUI.isKeyPressed(CMD_VALID)))
            {
               _menuKeyPressed = true;
                if (((_menuHeroSelector->PlayerReady(1) && _inputsPlayers[0]->isKeyPressed(CMD_JUMP)) ||
                     (_menuHeroSelector->PlayerReady(2) && _inputsPlayers[1]->isKeyPressed(CMD_JUMP)) ||
                     (_menuHeroSelector->PlayerReady(3) && _inputsPlayers[2]->isKeyPressed(CMD_JUMP)) ||
                     (_menuHeroSelector->PlayerReady(4) && _inputsPlayers[3]->isKeyPressed(CMD_JUMP))) &&
                      _menuHeroSelector->MinTwoPlayerOK())
                {
					//Delete AIs					
					for (int i = 0; i < _AIs.size(); i++)
						if(_AIs[i] != NULL)
							delete _AIs[i];
					_AIs.clear();

					//Recreate AIs and set input
					int playerCount = 0;
					for (int i = 1; i <= 4; i++)
					{
						_inputsPlayers[i - 1]->SetAI(_menuHeroSelector->IsAIPlayer(i));
						if (_menuHeroSelector->PlayerReady(i) && _menuHeroSelector->IsAIPlayer(i))
							_AIs.push_back(new AI(playerCount, _inputsPlayers[i - 1], 5));
						playerCount += _menuHeroSelector->PlayerReady(i) ? 1 : 0;
					}

					//Loading Players + Map + AIs
					_loadNewGame = true;
					_threadLoadPlayers->launch();
					_maps->LoadMap("environment//mapFiles//map.xml");
					
					//Transition + music
					_transitionBlack.setFillColor(Color(0, 0, 0, 255));
					_soundBox.PlayMusic(MUSIC::HHavok);
                    _gameState = GAME;
                }
            }
            break;
        case GAME:
            if(_inputUI.isKeyPressed(CMD_PAUSE) && !_menuKeyPressed && !_maps->IsMapLoading())
            {
                _menuKeyPressed = true;
                _yesNoMenu->ResetAnimation(true);
                _gameState = PAUSE;
            }
            break;
        case GAME_END:
            if((_inputUI.isKeyPressed(CMD_VALID) || _inputUI.isKeyPressed(CMD_PAUSE)) &&
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
				for (int i = 0; i < 4; i++) { _inputsPlayers[i]->SetAI(false); }
                _resultText.setPosition(Vector2f(640,-100));
                _menuKeyPressed = true;
                _soundBox.PlayMusic(MUSIC::Chibi_Ninja);
				_gameState = HERO_SELECT;
            }
            break;
        case PAUSE:
            if((_inputUI.isKeyPressed(CMD_BACK) || _inputUI.isKeyPressed(CMD_PAUSE)) && !_menuKeyPressed)
            {
                _pauseMenu->ResetAnimation();
                _menuKeyPressed = true;
                _gameState = GAME;
            }
            break;
        case YESNO:
            if(_inputUI.isKeyPressed(CMD_BACK) && !_menuKeyPressed)
            {
                _menuKeyPressed = true;
                _yesNoMenu->ResetAnimation(true);
                _gameState = _lastGameState;
            }
            break;
    }
    if(!Keyboard::isKeyPressed(Keyboard::Num0) &&
       !_inputUI.isKeyPressed(CMD_BACK) && !_inputUI.isKeyPressed(CMD_PAUSE) &&
       !_inputUI.isKeyPressed(CMD_VALID) && !Mouse::isButtonPressed(Mouse::Left) &&
       !_inputsPlayers[0]->isKeyPressed(CMD_COUNTERACT, true) && !_inputsPlayers[1]->isKeyPressed(CMD_COUNTERACT, true) &&
       !_inputsPlayers[2]->isKeyPressed(CMD_COUNTERACT, true) && !_inputsPlayers[3]->isKeyPressed(CMD_COUNTERACT, true) &&
       !_inputsPlayers[0]->isKeyPressed(CMD_JUMP, true) && !_inputsPlayers[1]->isKeyPressed(CMD_JUMP) &&
       !_inputsPlayers[2]->isKeyPressed(CMD_JUMP, true) && !_inputsPlayers[3]->isKeyPressed(CMD_JUMP, true))
       _menuKeyPressed = false;
}

void mainGame::MainMenuEvents()
{
    if(ValidMenu(_mainMenu) && !_menuKeyPressed)
    {
		_menuKeyPressed = true;
        switch(_mainMenu->GetSelectedItem())
        {
            case 0:
                _menuHeroSelector->ResetMenu();
                _gameState = HERO_SELECT;
                break;
            case 1:
                _mainMenu->ResetAnimation(false);
				_mainOptions->SetPosition(Vector2f(640, 440), ALIGN_NOCHANGE, BG_HIDE);
                _mainOptions->UpdateOptions(xmlReader::getSettingsValue(), false);
				_lastGameState = MAIN_MENU;
                _gameState = MAIN_OPTIONS;
                break;
            case 2:
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
		_menuKeyPressed = true;
        switch(_mainOptions->GetSelectedItem())
        {
            case 0:
                _gameState = OPTION_LANG;
				_currentOptionMenu = _optionLanguage;
                _optionLanguage->FocusText(_settings->_language);
                break;
            case 1:
                _gameState = OPTION_SCREEN;
				_currentOptionMenu = _optionScreen;
                _optionScreen->FocusText(_settings->_fullscreen);
                break;
            case 2:
                _gameState = OPTION_TIME;
				_currentOptionMenu = _optionTime;
                _optionTime->FocusText(_settings->_timeString);
                break;
			case 3:
				_gameState = OPTION_ANALOG;
				_currentOptionMenu = _optionScreen;
				_optionScreen->FocusText(_settings->_analog);
				break;
            case 4:
                _gameState = OPTION_LIFE;
				_currentOptionMenu = _optionLife;
                _optionLife->FocusText(_settings->_lifeString);
                break;
            case 5:
                _gameState = OPTION_MUSIC;
				_currentOptionMenu = _optionMusic;
                _optionMusic->FocusText(_settings->_musicString);
                break;
            case 6:
                _gameState = OPTION_SFX;
				_currentOptionMenu = _optionEffects;
                _optionEffects->FocusText(_settings->_effectsString);
                break;
            case 7:
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
				for (int i = 0; i < _inputsPlayers.size(); i++)
					_inputsPlayers[i]->SetAnalogControls(_settings->_analog == "ON"); }
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
		_menuKeyPressed = true;
        switch(_mainControls->GetSelectedItem())
        {
            case 0:
				_padMenu->ShowBackground(_lastGameState == PAUSE);
				_padMenu->SetPosition(_lastGameState == PAUSE ? 360 : 440);
                _gameState = SHOW_GAMEPAD;
                _mainControls->ResetAnimation();
                break;
            case 5:
                _gameState = _lastGameState;
                _mainControls->ResetAnimation();
                break;
            case 1:case 2:case 3:case 4:
                stringstream stream;
                stream << _mainControls->GetSelectedItem();
				_keyConfig->ShowBackground(_lastGameState == PAUSE);
				_keyConfig->SetPosition(Vector2f(640, _lastGameState == PAUSE ? 360 : 440));
                _keyConfig->UpdateOptions(xmlReader::getKeysValue(stream.str()), true);
                _editingInputNum = _mainControls->GetSelectedItem();
                if(_mainControls->GetSelectedItem() == 1)
                    _editingInput = _inputsPlayers[0];
                else if(_mainControls->GetSelectedItem() == 2)
                    _editingInput = _inputsPlayers[1];
                else if(_mainControls->GetSelectedItem() == 3)
                    _editingInput = _inputsPlayers[2];
                else
                    _editingInput = _inputsPlayers[3];
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
		_menuKeyPressed = true;
        switch(_pauseMenu->GetSelectedItem())
        {
            case 0:
                _pauseMenu->ResetAnimation();
                for(int i=0 ; i<_players.size() ; i++)
					_players[i]->SetHasJumped(true);
                _gameState = GAME;
                break;
            case 1:case 4: //Validation YES-NO
                _yesNoMenu->ResetAnimation(true);
                _lastGameState = PAUSE;
                _yesnoShape.setPosition(utils::VIEW_WIDTH, _yesnoShape.getPosition().y);
                _yesnoText.setPosition(0, _yesnoText.getPosition().y);
                _gameState = YESNO;
                break;
			case 2:
				_pauseMenu->ResetAnimation(false);
				_mainOptions->SetPosition(Vector2f(640, 360), ALIGN_NOCHANGE, BG_SHOW);
				_mainOptions->UpdateOptions(xmlReader::getSettingsValue(), false);
				_lastGameState = PAUSE;
				_gameState = MAIN_OPTIONS;
				break;
			case 3:
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
					_loadNewGame = false;
					_threadLoadPlayers->launch();
					while (_isPlayersLoading)
					{
						Time time;
						sleep(time);
					}

                    _menuHeroSelector->ResetMenu();
                    _transitionBlack.setFillColor(Color(0, 0, 0));
                    _gameState = GAME;
                    _pauseMenu->ResetAnimation(true);
                    _soundBox.PlayMusic(MUSIC::HHavok);
                }
                else if(_pauseMenu->GetSelectedItem() == 4)
                {
                    _menuKeyPressed = true;
					for (int i = 0; i < 4; i++) { _inputsPlayers[i]->SetAI(false); }
                    _mainMenu->ResetAnimation();
                    _pauseMenu->ResetAnimation(true);
					_soundBox.PlayMusic(MUSIC::Chibi_Ninja);
                    _gameState = MAIN_MENU;
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
	_mainMenu = new menuList(xmlReader::getTranslationTexts("menuMain", _settings->_language), Vector2f(640, 440), &_inputUI, false);
	_mainControls = new menuList(xmlReader::getTranslationTexts("menuControls", _settings->_language), _mainControls->GetPosition(), &_inputUI, _mainControls->GetBackgroundVisibility());
	_mainOptions = new menuOptions(xmlReader::getTranslationTexts("settings", _settings->_language), _mainOptions->GetPosition(), &_inputUI, _mainOptions->GetBackgroundVisibility());
	_mainOptions->UpdateOptions(xmlReader::getSettingsValue(), false);
	_optionLanguage = new menuList(_settings->getSettingsChoices(CHOICE_LANG), Vector2f(640, 440), &_inputUI, true, ALIGN_MIDDLE, _mainOptions);
    _optionScreen = new menuList(_settings->getSettingsChoices(CHOICE_ON_OFF), Vector2f(640, 440), &_inputUI, true, ALIGN_MIDDLE, _mainOptions);
    _optionTime = new menuList(_settings->getSettingsChoices(CHOICE_TIME), Vector2f(640, 440), &_inputUI, true, ALIGN_MIDDLE, _mainOptions);
    _optionLife = new menuList(_settings->getSettingsChoices(CHOICE_LIFE), Vector2f(640, 440), &_inputUI, true, ALIGN_MIDDLE, _mainOptions);
    _optionMusic = new menuList(_settings->getSettingsChoices(CHOICE_SOUND), Vector2f(640, 440), &_inputUI, true, ALIGN_MIDDLE, _mainOptions);
    _optionEffects = new menuList(_settings->getSettingsChoices(CHOICE_SOUND), Vector2f(640, 440), &_inputUI, true, ALIGN_MIDDLE, _mainOptions);
	_keyConfig = new menuOptions(xmlReader::getTranslationTexts("key", _settings->_language), Vector2f(640, 440), &_inputUI, _keyConfig->GetBackgroundVisibility());
	_pauseMenu = new menuList(xmlReader::getTranslationTexts("pause", _settings->_language), Vector2f(640, 360), &_inputUI, true);
	_padMenu = new menuPad(420, _settings->_language, &_inputUI);
	_menuHeroSelector = new menuHeroSelector(_settings->_language, _inputsPlayers, &_soundBox);
	_yesNoMenu = new menuList(xmlReader::getTranslationTexts("yesno", _settings->_language), Vector2f(640, 360), &_inputUI, true);
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

	//Background effects
	_effectBox.DisplayBack(_window, (_settings->_fullscreen == "ON"), _deltaTime);

	//Drawing non dashing players
    for(int i=0 ; i<_players.size() ; i++)
		if(_players[i] != NULL && !_players[i]->IsDashing())
			_players[i]->Display(_window, _deltaTime);
	//Drawing dashing players above others
	for (int i = 0; i<_players.size(); i++)
		if (_players[i] != NULL && _players[i]->IsDashing())
			_players[i]->Display(_window, _deltaTime);

    //Scores / effects / projectiles
    _scoreManager->Display(_window);
    _effectBox.DisplayFront(_window, (_settings->_fullscreen == "ON"), _deltaTime);
    _projectileBox->Display(_window);

	//Drawing players HUD above everything
	for (int i = 0; i < _players.size(); i++)
		if (_players[i] != NULL && _players[i]->GetLife() > 0)
			_players[i]->DrawHUD(_window);

	//Dark background effect
	bool darkBackground = false;
	for (int i = 0; i < _players.size(); i++)
		if (_players[i] != NULL && _players[i]->IsCounteractingProjectile())
			darkBackground = true;
	_maps->ShowBackgroundEffect(darkBackground);
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
           !_inputUI.isKeyPressed(CMD_VALID) && !Mouse::isButtonPressed(Mouse::Left))
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

    //Main switch
    switch(_gameState)
    {
        case GAME:
			if (_maps->IsMapLoading() || _isPlayersLoading)
				break;

			if (Keyboard::isKeyPressed(Keyboard::Space))
				_deltaTime *= 0.2;

			//Step
            _world->Step(_deltaTime / 60.f, 25, 10);
            Box2DTools::ManageBodyLoop(_world);
            _maps->RenderMap(_deltaTime);
            _soundBox.PerformSounds();
            for(int i=0 ; i<_players.size() ; i++)
				_players[i]->Step(_deltaTime);
			_projectileBox->RenderProjectiles(_deltaTime);
			_effectBox.RenderEffects(_world, (_settings->_fullscreen == "ON"), _deltaTime);
			_scoreManager->Step(_deltaTime);

			//Inputs step
			for (int i = 0; i < _inputsPlayers.size(); i++)
				_inputsPlayers[i]->Step(_deltaTime, i == 1);

			//######### TEST AI ##################
			for(int i=0 ; i<_AIs.size() ; i++)
				if (_AIs[i] != NULL)
					_AIs[i]->Step(_players[_AIs[i]->GetNumPlayer()], _projectileBox, deltaTime);
            break;
		case EDITOR:
			//Render effects and map
			_effectBox.RenderEffects(_world, (_settings->_fullscreen == "ON"), deltaTime);
			if (_maps->GetEditor()->StepPhysics())
				_maps->EditMap(NULL, deltaTime, true);

			//Canceling world step during bridge delete operation to avoid crash (if stepping NULL objects)
			/*if ((Mouse::isButtonPressed(Mouse::Left) || Mouse::isButtonPressed(Mouse::Right)) && _maps->GetEditor()->StepWorld())
				_bridgeDeleteTimeout = 10.f;
			else
				_bridgeDeleteTimeout = utils::StepCooldown(_bridgeDeleteTimeout, 1.f, deltaTime);*/

			//World step
			if (!_world->IsLocked() && _maps->GetEditor()->StepPhysics())// && _bridgeDeleteTimeout == 0.f)
				_world->Step(_deltaTime / 60.f, 50, 15);
			break;
    }
}

void mainGame::Display(RenderWindow * window, Keyboard::Key key, bool keyPressed, float deltaTime)
{
    //Updating variables
	_window = window;
    _currentKey = key;
    _globalKeyPressed = keyPressed;
    _deltaTime = deltaTime;

	//Update last key (keyboard or pad)
	_inputUI.PerformLastController(_window);

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

			//######### TEST AI ##################
			for(int i=0 ; i<_AIs.size() ; i++)
				if(_AIs[i] != NULL)
					_AIs[i]->Display(_window);
			/*if (Mouse::isButtonPressed(Mouse::Left))
			{
				Vector2f mousePos = _window->mapPixelToCoords(sf::Mouse::getPosition(*_window));
				_aiTEST->SetPointToReach(mousePos);
			}*/

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
			/*if (!_maps->GetEditor()->StepPhysics()) //If no need to step physics - safer to render and display in the same thread
			{
				_effectBox.RenderEffects(_world, (_settings->_fullscreen == "ON"), deltaTime);
				_maps->EditMap(NULL, deltaTime, true);
			}*/
			_effectBox.Display(_window, (_settings->_fullscreen == "ON"), deltaTime);
			_maps->EditMap(_window, deltaTime, !_maps->GetEditor()->StepPhysics());
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
