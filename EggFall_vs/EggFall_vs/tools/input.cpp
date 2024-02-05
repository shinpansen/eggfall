#include "input.hpp"

static const float ROUNDANGLE = 4.2f;

input::input()
{
}

input::input(int player, int deadZone, RenderWindow* window, bool analogControls, bool AI)
{
	//Variables
    _deadZone = deadZone;
	_runTimeoutA = 0.f;
	_runTimeoutB = 0.f;
	_runKeyPressed = false;
    _window = window;
    _mouseController = false;
	_analogControls = analogControls;
	_AI = AI;
    _lastMousePos = Mouse::getPosition();

	//AI keys
	_AI_JumpKey = 0.f;
	_AI_AttackKey = 0.f;
	_AI_CounteractKey = 0.f;
	_AI_DashKey = 0.f;
	_AI_RunKey = 0.f;
	_AI_UpKey = 0.f;
	_AI_DownKey = 0.f;
	_AI_LeftKey = 0.f;
	_AI_RightKey = 0.f;
	_AI_Analog = 0.f;
	_AI_AnalogDir = Vector2f();

    //Keyboard
    if(player == -1) //MENU
    {
        _KEY_UP = Keyboard::Up;
        _KEY_DOWN = Keyboard::Down;
        _KEY_LEFT = Keyboard::Left;
        _KEY_RIGHT = Keyboard::Right;
        _KEY_VALID = Keyboard::Return;
        _KEY_BACK = Keyboard::BackSpace;
        _KEY_PAUSE = Keyboard::Escape;
    }
    else if(player == 0)
    {
        _KEY_JUMP = Keyboard::C;
        _KEY_ATTACK = Keyboard::X;
		_KEY_COUNTERACT = Keyboard::V;
		_KEY_DASH = Keyboard::A;
		_KEY_AIM = Keyboard::W;
        _KEY_UP = Keyboard::Z;
        _KEY_DOWN = Keyboard::S;
        _KEY_LEFT = Keyboard::Q;
        _KEY_RIGHT = Keyboard::D;
    }
    else if(player == 1)
    {
        _KEY_JUMP = Keyboard::Numpad1;
        _KEY_ATTACK = Keyboard::Numpad2;
        _KEY_COUNTERACT = Keyboard::Numpad3;
		_KEY_DASH = Keyboard::Numpad4;
		_KEY_AIM = Keyboard::Numpad5;
        _KEY_UP = Keyboard::Up;
        _KEY_DOWN = Keyboard::Down;
        _KEY_LEFT = Keyboard::Left;
        _KEY_RIGHT = Keyboard::Right;
    }
    else if(player == 2)
    {
        _KEY_JUMP = Keyboard::N;
        _KEY_ATTACK = Keyboard::B;
        _KEY_COUNTERACT = Keyboard::Comma;
		_KEY_DASH = Keyboard::Space;
		_KEY_AIM = Keyboard::R;
        _KEY_UP = Keyboard::T;
        _KEY_DOWN = Keyboard::G;
        _KEY_LEFT = Keyboard::F;
        _KEY_RIGHT = Keyboard::H;
    }
    else if(player == 3)
    {
        _KEY_JUMP = Keyboard::Tilde;
        _KEY_ATTACK = Keyboard::M;
        _KEY_COUNTERACT = Keyboard::SemiColon;
		_KEY_DASH = Keyboard::BackSlash;
		_KEY_AIM = Keyboard::U;
        _KEY_UP = Keyboard::I;
        _KEY_DOWN = Keyboard::K;
        _KEY_LEFT = Keyboard::J;
        _KEY_RIGHT = Keyboard::L;
    }

    //Joystick
    _player = player;
    _JOY_JUMP = GetSFMLJoyNum(X360::A);
    _JOY_ATTACK = GetSFMLJoyNum(X360::X);
    _JOY_COUNTERACT = GetSFMLJoyNum(X360::B);
	_JOY_DASH = GetSFMLJoyNum(X360::RB);
	_JOY_AIM = GetSFMLJoyNum(X360::LB);
    _JOY_VALID = GetSFMLJoyNum(X360::A);
    _JOY_BACK = GetSFMLJoyNum(X360::B);
    _JOY_PAUSE = GetSFMLJoyNum(X360::START);

    //Load keys from XML
    if(_player >= 0)
    {
        stringstream stream;
        stream << player+1;
        vector<string> keysFromXml = xmlReader::getKeysValue(stream.str());
        if(keysFromXml.size() >= 9)
        {
            _KEY_JUMP = (Keyboard::Key)atoi(keysFromXml[0].c_str());
            _KEY_ATTACK = (Keyboard::Key)atoi(keysFromXml[1].c_str());
            _KEY_COUNTERACT = (Keyboard::Key)atoi(keysFromXml[2].c_str());
			_KEY_DASH = (Keyboard::Key)atoi(keysFromXml[3].c_str());
			_KEY_AIM = (Keyboard::Key)atoi(keysFromXml[4].c_str());
            _KEY_UP = (Keyboard::Key)atoi(keysFromXml[5].c_str());
            _KEY_DOWN = (Keyboard::Key)atoi(keysFromXml[6].c_str());
            _KEY_LEFT = (Keyboard::Key)atoi(keysFromXml[7].c_str());
			_KEY_RIGHT = (Keyboard::Key)atoi(keysFromXml[8].c_str());
        }
        else
            WriteXML();
    }
}

float input::RoundAngle(float angleFinal)
{
	//Round angle if close to standard direction (0°, 45°, 90° ...)
	for (int angle = 0; angle < 360; angle += 45)
	{
		if (fabs((float)angle - angleFinal) <= ROUNDANGLE)
		{
			angleFinal = (float)angle;
			break;
		}
	}
	return angleFinal;
}

void input::SetKey(int action, Keyboard::Key key)
{
    switch(action)
    {
        case 0:
            _KEY_JUMP = key;
            break;
        case 1:
            _KEY_ATTACK = key;
            break;
        case 2:
            _KEY_COUNTERACT = key;
            break;
        case 3:
            _KEY_DASH = key;
            break;
		case 4:
			_KEY_AIM = key;
			break;
        case 5:
            _KEY_UP = key;
            break;
        case 6:
            _KEY_DOWN = key;
            break;
        case 7:
            _KEY_LEFT = key;
            break;
        case 8:
            _KEY_RIGHT = key;
            break;
        default:
            break;
    }
}

void input::SetJoy(COMMAND cmd, int joy)
{
    switch(cmd)
    {
        case CMD_JUMP:
            _JOY_JUMP = joy;
            break;
        case CMD_ATTACK:
            _JOY_ATTACK = joy;
            break;
        case CMD_DASH:
            _JOY_DASH = joy;
            break;
        case CMD_COUNTERACT:
            _JOY_COUNTERACT = joy;
            break;
        case CMD_VALID:
            _JOY_VALID = joy;
            break;
        default:
            break;
    }
}

void input::SetAI(bool AI)
{
	_AI = AI;
}

void input::SetAnalogControls(bool analog)
{
	_analogControls = analog;
}

bool input::isKeyPressed(COMMAND cmd, bool ignoreAI)
{
    if(_player >= 0)
    {
        switch(cmd)
        {
            case CMD_JUMP:
				if (_AI && !ignoreAI)
					return _AI_JumpKey > 0.f;
				return Keyboard::isKeyPressed(_KEY_JUMP) || Joystick::isButtonPressed(_player, _JOY_JUMP);
            case CMD_ATTACK:
				if (_AI && !ignoreAI)
					return _AI_AttackKey > 0.f;
				return Keyboard::isKeyPressed(_KEY_ATTACK) || Joystick::isButtonPressed(_player, _JOY_ATTACK);
            case CMD_RUN:
				return !_AI && (fabs(_runTimeoutB) >= 6.f || _AI_RunKey > 0.f);
            case CMD_COUNTERACT:
				if (_AI && !ignoreAI)
					return _AI_CounteractKey > 0.f;
				return Keyboard::isKeyPressed(_KEY_COUNTERACT) || Joystick::isButtonPressed(_player, _JOY_COUNTERACT);
			case CMD_DASH:
				if (_AI && !ignoreAI)
					return _AI_DashKey > 0.f;
				return Keyboard::isKeyPressed(_KEY_DASH) || Joystick::isButtonPressed(_player, _JOY_DASH);
			case CMD_AIM:
				return !_AI && (Keyboard::isKeyPressed(_KEY_AIM) || Joystick::isButtonPressed(_player, _JOY_AIM));
            case CMD_UP:
				if (_AI && !ignoreAI)
					return _AI_UpKey > 0.f;
                if(Keyboard::isKeyPressed(_KEY_DOWN) || Joystick::getAxisPosition(_player, Joystick::Y) > _deadZone)
                    return false;
                return Joystick::getAxisPosition(_player, Joystick::Y) < -_deadZone ||
                       Joystick::getAxisPosition(_player, Joystick::PovY) > _deadZone ||
                       Keyboard::isKeyPressed(_KEY_UP);
            case CMD_DOWN:
				if (_AI && !ignoreAI)
					return _AI_DownKey > 0.f;
                if(Keyboard::isKeyPressed(_KEY_UP) || Joystick::getAxisPosition(_player, Joystick::Y) < -_deadZone)
                    return false;
                return Joystick::getAxisPosition(_player, Joystick::Y) > _deadZone ||
                       Joystick::getAxisPosition(_player, Joystick::PovY) < -_deadZone ||
                       Keyboard::isKeyPressed(_KEY_DOWN);
            case CMD_LEFT:
				if (_AI && !ignoreAI)
					return _AI_LeftKey > 0.f;
                if(Keyboard::isKeyPressed(_KEY_RIGHT) || Joystick::getAxisPosition(_player, Joystick::X) > _deadZone)
                    return false;
                return Joystick::getAxisPosition(_player, Joystick::X) < -_deadZone ||
                       Joystick::getAxisPosition(_player, Joystick::PovX) < -_deadZone ||
                       Keyboard::isKeyPressed(_KEY_LEFT);
            case CMD_RIGHT:
				if (_AI && !ignoreAI)
					return _AI_RightKey > 0.f;
                if(Keyboard::isKeyPressed(_KEY_LEFT) || Joystick::getAxisPosition(_player, Joystick::X) < -_deadZone)
                    return false;
                return Joystick::getAxisPosition(_player, Joystick::X) > _deadZone ||
                       Joystick::getAxisPosition(_player, Joystick::PovX) > _deadZone ||
                       Keyboard::isKeyPressed(_KEY_RIGHT);
			case CMD_ANY_DIRECTION:
				if (_AI && !ignoreAI)
					return _AI_UpKey > 0.f || _AI_DownKey > 0.f || _AI_LeftKey > 0.f || _AI_RightKey > 0.f;
				return isKeyPressed(CMD_LEFT) || isKeyPressed(CMD_RIGHT) || isKeyPressed(CMD_UP) || isKeyPressed(CMD_DOWN);
        }
    }
    else
    {
        switch(cmd)
        {
            case CMD_UP:
                if(Keyboard::isKeyPressed(_KEY_UP))
                    return true;
                else
                {
                    for(int i=0 ; i<4 ; i++)
                    {
                        if(!Joystick::isConnected(i)) { continue; }
                        if(Joystick::getAxisPosition(i, Joystick::Y) < -_deadZone ||
                           Joystick::getAxisPosition(i, Joystick::PovY) > _deadZone)
                            return true;
                    }
                    return false;
                }
            case CMD_DOWN:
                if(Keyboard::isKeyPressed(_KEY_DOWN))
                    return true;
                else
                {
                    for(int i=0 ; i<4 ; i++)
                    {
                        if(!Joystick::isConnected(i)) { continue; }
                        if(Joystick::getAxisPosition(i, Joystick::Y) > _deadZone ||
                           Joystick::getAxisPosition(i, Joystick::PovY) < -_deadZone)
                            return true;
                    }
                    return false;
                }
            case CMD_LEFT:
                if(Keyboard::isKeyPressed(_KEY_LEFT))
                    return true;
                else
                {
                    for(int i=0 ; i<4 ; i++)
                    {
                        if(!Joystick::isConnected(i)) { continue; }
                        if(Joystick::getAxisPosition(i, Joystick::X) < -_deadZone ||
                           Joystick::getAxisPosition(i, Joystick::PovX) < -_deadZone)
                            return true;
                    }
                    return false;
                }
            case CMD_RIGHT:
                if(Keyboard::isKeyPressed(_KEY_RIGHT))
                    return true;
                else
                {
                    for(int i=0 ; i<4 ; i++)
                    {
                        if(!Joystick::isConnected(i)) { continue; }
                        if(Joystick::getAxisPosition(i, Joystick::X) > _deadZone ||
                           Joystick::getAxisPosition(i, Joystick::PovX) > _deadZone)
                            return true;
                    }
                    return false;
                }
            case CMD_VALID:
				if (Keyboard::isKeyPressed(_KEY_VALID))
					return true;
                else
                {
                    for(int i=0 ; i<4 ; i++)
                    {
                        if(!Joystick::isConnected(i)) { continue; }
                        if(Joystick::isButtonPressed(i, _JOY_VALID))
                            return true;
                    }
                    return false;
                }
            case CMD_BACK:
                if(Keyboard::isKeyPressed(_KEY_BACK))
                    return true;
                else
                {
                    for(int i=0 ; i<4 ; i++)
                    {
                        if(!Joystick::isConnected(i)) { continue; }
                        if(Joystick::isButtonPressed(i, _JOY_BACK))
                            return true;
                    }
                    return false;
                }
            case CMD_PAUSE:
                if(Keyboard::isKeyPressed(_KEY_PAUSE))
                    return true;
                else
                {
                    for(int i=0 ; i<4 ; i++)
                    {
                        if(!Joystick::isConnected(i)) { continue; }
                        if(Joystick::isButtonPressed(i, _JOY_PAUSE))
                            return true;
                    }
                    return false;
                }
        }
    }
    return false;
}

void input::PerformLastController(RenderWindow* window)
{
    if(!_mouseController && Mouse::getPosition(*window) != _lastMousePos)
    {
        _lastMousePos = Mouse::getPosition(*window);
        _mouseController = true;
    }
    else if(_mouseController &&
            (isKeyPressed(CMD_JUMP) ||
             isKeyPressed(CMD_ATTACK) ||
             isKeyPressed(CMD_RUN) ||
			 isKeyPressed(CMD_COUNTERACT) ||
			 isKeyPressed(CMD_DASH) ||
			 isKeyPressed(CMD_AIM) ||
             isKeyPressed(CMD_UP) ||
             isKeyPressed(CMD_DOWN) ||
             isKeyPressed(CMD_LEFT) ||
             isKeyPressed(CMD_RIGHT) ||
             isKeyPressed(CMD_VALID) ||
             isKeyPressed(CMD_BACK) ||
             isKeyPressed(CMD_PAUSE)))
    {
        _lastMousePos = Mouse::getPosition(*window);
        _mouseController = false;
    }
}

int input::GetPlayerNumber()
{
    return _player;
}

bool input::GetMouseController()
{
    return _mouseController;
}

void input::WriteXML()
{
    try
    {
        ofstream inputFile(files::keysFile[_player].c_str(), ios::out | ios::trunc);
        if(inputFile)
        {
            inputFile << "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n";
            inputFile << "<config>\n";
            inputFile << "\t<key type=\"JUMP\">\n\t\t" << _KEY_JUMP << "\n\t</key>\n";
            inputFile << "\t<key type=\"ATTACK\">\n\t\t" << _KEY_ATTACK << "\n\t</key>\n";
            inputFile << "\t<key type=\"COUNTERACT\">\n\t\t" << _KEY_COUNTERACT << "\n\t</key>\n";
			inputFile << "\t<key type=\"DASH\">\n\t\t" << _KEY_DASH << "\n\t</key>\n";
			inputFile << "\t<key type=\"AIM\">\n\t\t" << _KEY_AIM << "\n\t</key>\n";
            inputFile << "\t<key type=\"UP\">\n\t\t" << _KEY_UP << "\n\t</key>\n";
            inputFile << "\t<key type=\"DOWN\">\n\t\t" << _KEY_DOWN << "\n\t</key>\n";
            inputFile << "\t<key type=\"LEFT\">\n\t\t" << _KEY_LEFT << "\n\t</key>\n";
            inputFile << "\t<key type=\"RIGHT\">\n\t\t" << _KEY_RIGHT << "\n\t</key>\n";
            inputFile << "</config>";
        }
    }
    catch(exception e)
    {
        cout << e.what() << endl;
    }
}

float input::GetStickDeltaX()
{
	if ((Keyboard::isKeyPressed(_KEY_LEFT) ) &&
		fabs(Joystick::getAxisPosition(_player, Joystick::X)) <_deadZone)
		return -1.f;
	else if ((Keyboard::isKeyPressed(_KEY_RIGHT) ) &&
		fabs(Joystick::getAxisPosition(_player, Joystick::PovX)) < _deadZone)
		return 1.f;
	else
	{
		if (fabs(Joystick::getAxisPosition(_player, Joystick::X)) > _deadZone)
			return Joystick::getAxisPosition(_player, Joystick::X) / 100.f;
		else
			return 1.f;
	}
}

Vector2f input::GetStickDirection()
{
	if (!isKeyPressed(CMD_ANY_DIRECTION) && _AI_Analog == 0.f)
		return Vector2f(0, 0);

	//X Y direction value
	float xDir = 0.f, yDir = 0.f;
	if (_analogControls && 
	   (fabs(Joystick::getAxisPosition(_player, Joystick::X)) > _deadZone ||
		fabs(Joystick::getAxisPosition(_player, Joystick::Y)) > _deadZone))
	{
		//Getting X Y value from joystick
		xDir = Joystick::getAxisPosition(_player, Joystick::X);
		yDir = Joystick::getAxisPosition(_player, Joystick::Y);
	}
	else
	{
		//Getting X Y value from keyboard
		xDir = isKeyPressed(CMD_LEFT) ? -1.f : (isKeyPressed(CMD_RIGHT) ? 1.f : 0.f);
		yDir = isKeyPressed(CMD_UP) ? -1.f : (isKeyPressed(CMD_DOWN) ? 1.f : 0.f);
	}

	//AI direction
	if (_AI_Analog > 0.f && _AI_AnalogDir != Vector2f())
	{
		xDir = _AI_AnalogDir.x;
		yDir = _AI_AnalogDir.y;
	}

	//Signe
	float xSigne = xDir / fabs(xDir);
	float ySigne = yDir / fabs(yDir);

	//Calculating X Y value based on Sum 1
	if (xDir == 0.f || yDir == 0.f)
	{
		xDir = yDir == 0.f ? xSigne : xDir;
		yDir = xDir == 0.f ? ySigne : yDir;
	}
	else
	{
		//Calculating based on X direction
		xDir = xDir / (fabs(xDir) + fabs(yDir));
		xDir = roundf(xDir * 100.f) / 100.f; // round 2 decimals

		//Aim help : if close to (1, 0.5, 0) tend to this value
		xDir = fabs(1.f - fabs(xDir)) <= 0.08f ? 1.f*xSigne : xDir;
		xDir = fabs(0.5f - fabs(xDir)) <= 0.08f ? 0.5f*xSigne : xDir;
		xDir = fabs(xDir) <= 0.08f ? 0.f : xDir;

		//Calculating Y direction, based on X
		yDir = (1.f - fabs(xDir))*ySigne;
	}
	return Vector2f(xDir, yDir);
}

int input::GetStickAngle(int defaultAngle)
{
	int initAngle = 0;
	Vector2f dir = GetStickDirection();
	return utils::GetVectorRotation(dir, defaultAngle);
}

void input::SetVibration(int player, float leftMotor, float rightMotor)
{
	/*
	XINPUT_VIBRATION vib;
	//ZeroMemory(&vib, sizeof(XINPUT_VIBRATION));
	memset(&vib, 0, sizeof(XINPUT_VIBRATION));

	vib.wLeftMotorSpeed = static_cast <WORD> (leftMotor  * 65535.0f);
	vib.wRightMotorSpeed = static_cast <WORD> (rightMotor * 65535.0f);

	XInputSetState(player, &vib);*/
}

int input::GetSFMLJoyNum(X360 button)
{
	switch (button)
	{
	case X360::A:
		return 0;
	case X360::B:
		return 1;
	case X360::X:
		return 2;
	case X360::Y:
		return 3;
	case X360::LB:
		return 4;
	case X360::RB:
		return 5;
	case X360::START:
		return 7;
	case X360::SELECT:
		return 6;
	default:
		return 0;
	}
}

void input::PressKeyAI(COMMAND cmd, float duration)
{
	switch (cmd)
	{
		case CMD_JUMP:
			_AI_JumpKey = fabs(duration);
			break;
		case CMD_ATTACK:
			_AI_AttackKey = fabs(duration);
			break;
		case CMD_RUN:
			_AI_RunKey = fabs(duration);
			break;
		case CMD_COUNTERACT:
			_AI_CounteractKey = fabs(duration);
			break;
		case CMD_DASH:
			_AI_DashKey = fabs(duration);
			break;
		case CMD_UP:
			_AI_UpKey = fabs(duration);
			_AI_DownKey = 0.f;
			_AI_RightKey = 0.f;
			_AI_LeftKey = 0.f;
			break;
		case CMD_DOWN:
			_AI_DownKey = fabs(duration);
			_AI_UpKey = 0.f;
			_AI_RightKey = 0.f;
			_AI_LeftKey = 0.f;
			break;
		case CMD_LEFT:
			_AI_LeftKey = fabs(duration);
			_AI_RightKey = 0.f;
			break;
		case CMD_RIGHT:
			_AI_RightKey = fabs(duration);
			_AI_LeftKey = 0.f;
			break;
		default:
			break;
	}
}

void input::PressAnalogDirAI(Vector2f dir, float duration)
{
	_AI_Analog = fabs(duration);
	_AI_AnalogDir = dir;
	/*if (dir.x > dir.y)
		_AI_AnalogDir = Vector2f(1.f * dir.x / fabs(dir.x), dir.y / fabs(dir.x));
	else 
		_AI_AnalogDir = Vector2f(dir.x / fabs(dir.y), 1.f * dir.x / fabs(dir.x));*/
}

void input::Step(float deltaTime, bool AI)
{
	//Double press on direction to run
	if (!AI)
	{
		if (!_runKeyPressed && (isKeyPressed(CMD_LEFT) || isKeyPressed(CMD_RIGHT)))
		{
			_runKeyPressed = true;
			_runTimeoutA = 15.f; //Force to always double tap direction to run, instead of single pressing after a long press
			if (isKeyPressed(CMD_LEFT))
				_runTimeoutB = _runTimeoutB < 0.f ? -6.f : -5.f; //5 frames (based on 60fps) to press a second time the same direction
			else
				_runTimeoutB = _runTimeoutB > 0.f ? 6.f : 5.f;
		}
		else if (!isKeyPressed(CMD_LEFT) && !isKeyPressed(CMD_RIGHT))
		{
			_runKeyPressed = false;
			//Step timeout if not running (before second tap), immediate disable if CMD_RUN was activated (already double tap)
			_runTimeoutB = fabs(_runTimeoutB) <= 5.f ? utils::StepCooldown(_runTimeoutB, 1.f, deltaTime) : 0.f;
		}
		else
			_runTimeoutA = utils::StepCooldown(_runTimeoutA, 1.f, deltaTime);
		_runTimeoutB = _runTimeoutA == 0.f && fabs(_runTimeoutB) <= 5.f ? 0.f : _runTimeoutB;
	}

	//Step AI keys
	_AI_JumpKey = utils::StepCooldown(_AI_JumpKey, 1.f, deltaTime);
	_AI_AttackKey = utils::StepCooldown(_AI_AttackKey, 1.f, deltaTime);
	_AI_CounteractKey = utils::StepCooldown(_AI_CounteractKey, 1.f, deltaTime);
	_AI_DashKey = utils::StepCooldown(_AI_DashKey, 1.f, deltaTime);
	_AI_RunKey = utils::StepCooldown(_AI_RunKey, 1.f, deltaTime);
	_AI_UpKey = utils::StepCooldown(_AI_UpKey, 1.f, deltaTime);
	_AI_DownKey = utils::StepCooldown(_AI_DownKey, 1.f, deltaTime);
	_AI_LeftKey = utils::StepCooldown(_AI_LeftKey, 1.f, deltaTime);
	_AI_RightKey = utils::StepCooldown(_AI_RightKey, 1.f, deltaTime);
	_AI_Analog = utils::StepCooldown(_AI_Analog, 1.f, deltaTime);
	if (_AI_Analog == 0.f)
		_AI_AnalogDir = Vector2f();
}

string input::GetKeyName(const sf::Keyboard::Key key)
{
    switch(key)
    {
        default:
            return "???";
            break;
        case sf::Keyboard::Unknown:
            return "???";
        case sf::Keyboard::A:
            return "A";
        case sf::Keyboard::B:
            return "B";
        case sf::Keyboard::C:
            return "C";
        case sf::Keyboard::D:
            return "D";
        case sf::Keyboard::E:
            return "E";
        case sf::Keyboard::F:
            return "F";
        case sf::Keyboard::G:
            return "G";
        case sf::Keyboard::H:
            return "H";
        case sf::Keyboard::I:
            return "I";
        case sf::Keyboard::J:
            return "J";
        case sf::Keyboard::K:
            return "K";
        case sf::Keyboard::L:
            return "L";
        case sf::Keyboard::M:
            return "M";
        case sf::Keyboard::N:
            return "N";
        case sf::Keyboard::O:
            return "O";
        case sf::Keyboard::P:
            return "P";
        case sf::Keyboard::Q:
            return "Q";
        case sf::Keyboard::R:
            return "R";
        case sf::Keyboard::S:
            return "S";
        case sf::Keyboard::T:
            return "T";
        case sf::Keyboard::U:
            return "U";
        case sf::Keyboard::V:
            return "V";
        case sf::Keyboard::W:
            return "W";
        case sf::Keyboard::X:
            return "X";
        case sf::Keyboard::Y:
            return "Y";
        case sf::Keyboard::Z:
            return "Z";
        case sf::Keyboard::Num0:
            return "Num0";
        case sf::Keyboard::Num1:
            return "Num1";
        case sf::Keyboard::Num2:
            return "Num2";
        case sf::Keyboard::Num3:
            return "Num3";
        case sf::Keyboard::Num4:
            return "Num4";
        case sf::Keyboard::Num5:
            return "Num5";
        case sf::Keyboard::Num6:
            return "Num6";
        case sf::Keyboard::Num7:
            return "Num7";
        case sf::Keyboard::Num8:
            return "Num8";
        case sf::Keyboard::Num9:
            return "Num9";
        case sf::Keyboard::Escape:
            return "Escape";
        case sf::Keyboard::LControl:
            return "LControl";
        case sf::Keyboard::LShift:
            return "LShift";
        case sf::Keyboard::LAlt:
            return "LAlt";
        case sf::Keyboard::LSystem:
            return "LSystem";
        case sf::Keyboard::RControl:
            return "RControl";
        case sf::Keyboard::RShift:
            return "RShift";
        case sf::Keyboard::RAlt:
            return "RAlt";
        case sf::Keyboard::RSystem:
            return "RSystem";
        case sf::Keyboard::Menu:
            return "Menu";
        case sf::Keyboard::LBracket:
            return "LBracket";
        case sf::Keyboard::RBracket:
            return "RBracket";
        case sf::Keyboard::SemiColon:
            return "SemiColon";
        case sf::Keyboard::Comma:
            return "Comma";
        case sf::Keyboard::Period:
            return "Period";
        case sf::Keyboard::Quote:
            return "Quote";
        case sf::Keyboard::Slash:
            return "Slash";
        case sf::Keyboard::BackSlash:
            return "BackSlash";
        case sf::Keyboard::Tilde:
            return "Tilde";
        case sf::Keyboard::Equal:
            return "Equal";
        case sf::Keyboard::Dash:
            return "Dash";
        case sf::Keyboard::Space:
            return "Space";
        case sf::Keyboard::Return:
            return "Return";
        case sf::Keyboard::BackSpace:
            return "BackSpace";
        case sf::Keyboard::Tab:
            return "Tab";
        case sf::Keyboard::PageUp:
            return "PageUp";
        case sf::Keyboard::PageDown:
            return "PageDown";
        case sf::Keyboard::End:
            return "End";
        case sf::Keyboard::Home:
            return "Home";
        case sf::Keyboard::Insert:
            return "Insert";
        case sf::Keyboard::Delete:
            return "Delete";
        case sf::Keyboard::Add:
            return "Add";
        case sf::Keyboard::Subtract:
            return "Subtract";
        case sf::Keyboard::Multiply:
            return "Multiply";
        case sf::Keyboard::Divide:
            return "Divide";
        case sf::Keyboard::Left:
            return "Left";
        case sf::Keyboard::Right:
            return "Right";
        case sf::Keyboard::Up:
            return "Up";
        case sf::Keyboard::Down:
            return "Down";
        case sf::Keyboard::Numpad0:
            return "Numpad0";
        case sf::Keyboard::Numpad1:
            return "Numpad1";
        case sf::Keyboard::Numpad2:
            return "Numpad2";
        case sf::Keyboard::Numpad3:
            return "Numpad3";
        case sf::Keyboard::Numpad4:
            return "Numpad4";
        case sf::Keyboard::Numpad5:
            return "Numpad5";
        case sf::Keyboard::Numpad6:
            return "Numpad6";
        case sf::Keyboard::Numpad7:
            return "Numpad7";
        case sf::Keyboard::Numpad8:
            return "Numpad8";
        case sf::Keyboard::Numpad9:
            return "Numpad9";
        case sf::Keyboard::F1:
            return "F1";
        case sf::Keyboard::F2:
            return "F2";
        case sf::Keyboard::F3:
            return "F3";
        case sf::Keyboard::F4:
            return "F4";
        case sf::Keyboard::F5:
            return "F5";
        case sf::Keyboard::F6:
            return "F6";
        case sf::Keyboard::F7:
            return "F7";
        case sf::Keyboard::F8:
            return "F8";
        case sf::Keyboard::F9:
            return "F9";
        case sf::Keyboard::F10:
            return "F10";
        case sf::Keyboard::F11:
            return "F11";
        case sf::Keyboard::F12:
            return "F12";
        case sf::Keyboard::F13:
            return "F13";
        case sf::Keyboard::F14:
            return "F14";
        case sf::Keyboard::F15:
            return "F15";
        case sf::Keyboard::Pause:
            return "Pause";
    }
}
