#include "input.hpp"

using namespace std;
using namespace sf;

input::input()
{
}

input::input(int player, int deadZone, RenderWindow* window)
{
    _deadZone = deadZone;
    _window = window;
    _mouseController = false;
    _lastMousePos = Mouse::getPosition();

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
        _KEY_RUN = Keyboard::W;
        _KEY_ROLL = Keyboard::V;
        _KEY_UP = Keyboard::Z;
        _KEY_DOWN = Keyboard::S;
        _KEY_LEFT = Keyboard::Q;
        _KEY_RIGHT = Keyboard::D;
    }
    else if(player == 1)
    {
        _KEY_JUMP = Keyboard::Numpad1;
        _KEY_ATTACK = Keyboard::Numpad2;
        _KEY_RUN = Keyboard::Numpad4;
        _KEY_ROLL = Keyboard::Numpad3;
        _KEY_UP = Keyboard::Up;
        _KEY_DOWN = Keyboard::Down;
        _KEY_LEFT = Keyboard::Left;
        _KEY_RIGHT = Keyboard::Right;
    }
    else if(player == 2)
    {
        _KEY_JUMP = Keyboard::N;
        _KEY_ATTACK = Keyboard::B;
        _KEY_RUN = Keyboard::Space;
        _KEY_ROLL = Keyboard::Comma;
        _KEY_UP = Keyboard::T;
        _KEY_DOWN = Keyboard::G;
        _KEY_LEFT = Keyboard::F;
        _KEY_RIGHT = Keyboard::H;
    }
    else if(player == 3)
    {
        _KEY_JUMP = Keyboard::Tilde;
        _KEY_ATTACK = Keyboard::M;
        _KEY_RUN = Keyboard::BackSlash;
        _KEY_ROLL = Keyboard::SemiColon;
        _KEY_UP = Keyboard::I;
        _KEY_DOWN = Keyboard::K;
        _KEY_LEFT = Keyboard::J;
        _KEY_RIGHT = Keyboard::L;
    }

    //Joystick
    _player = player;
    _JOY_JUMP = 0;
    _JOY_ATTACK = 2;
    _JOY_RUN = 5;
    _JOY_ROLL = 1;
    _JOY_VALID = 0;
    _JOY_BACK = 1;
    _JOY_PAUSE = 7;

    //lock keys
    _LOCK_KEY_JUMP = 0;
    _LOCK_KEY_ATTACK = 0;
    _LOCK_KEY_RUN = 0;
    _LOCK_KEY_ROLL = 0;
    _LOCK_KEY_UP = 0;
    _LOCK_KEY_DOWN = 0;
    _LOCK_KEY_LEFT = 0;
    _LOCK_KEY_RIGHT = 0;
    _LOCK_KEY_VALID = 0;
    _LOCK_KEY_BACK = 0;

    //Load keys from XML
    if(_player >= 0)
    {
        stringstream stream;
        stream << player+1;
        vector<string> keysFromXml = xmlReader::getKeysValue(stream.str());
        if(keysFromXml.size() >= 8)
        {
            _KEY_JUMP = (Keyboard::Key)atoi(keysFromXml[0].c_str());
            _KEY_ATTACK = (Keyboard::Key)atoi(keysFromXml[1].c_str());
            _KEY_ROLL = (Keyboard::Key)atoi(keysFromXml[2].c_str());
            _KEY_RUN = (Keyboard::Key)atoi(keysFromXml[3].c_str());
            _KEY_UP = (Keyboard::Key)atoi(keysFromXml[4].c_str());
            _KEY_DOWN = (Keyboard::Key)atoi(keysFromXml[5].c_str());
            _KEY_LEFT = (Keyboard::Key)atoi(keysFromXml[6].c_str());
            _KEY_RIGHT = (Keyboard::Key)atoi(keysFromXml[7].c_str());
        }
        else
            WriteXML();
    }
}

void input::setKey(int action, Keyboard::Key key)
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
            _KEY_ROLL = key;
            break;
        case 3:
            _KEY_RUN = key;
            break;
        case 4:
            _KEY_UP = key;
            break;
        case 5:
            _KEY_DOWN = key;
            break;
        case 6:
            _KEY_LEFT = key;
            break;
        case 7:
            _KEY_RIGHT = key;
            break;
        default:
            break;
    }
}

void input::setJoy(string action, int joy)
{
    switch(ResolveOption(action))
    {
        case JUMP:
            _JOY_JUMP = joy;
            break;
        case ATTACK:
            _JOY_ATTACK = joy;
            break;
        case RUN:
            _JOY_RUN = joy;
            break;
        case ROLL:
            _JOY_ROLL = joy;
            break;
        case VALID:
            _JOY_VALID = joy;
            break;
        default:
            break;
    }
}

bool input::isKeyPressed(string action)
{
    if(_player >= 0)
    {
        switch(ResolveOption(action))
        {
            case JUMP:
                return Joystick::isButtonPressed(_player, _JOY_JUMP) ||
                       Keyboard::isKeyPressed(_KEY_JUMP);
            case ATTACK:
                return Joystick::isButtonPressed(_player, _JOY_ATTACK) ||
                       Keyboard::isKeyPressed(_KEY_ATTACK);
             case RUN:
                return Joystick::isButtonPressed(_player, _JOY_RUN) ||
                       Keyboard::isKeyPressed(_KEY_RUN);
            case ROLL:
                return Joystick::isButtonPressed(_player, _JOY_ROLL) ||
                       Keyboard::isKeyPressed(_KEY_ROLL);
            case UP:
                if(Keyboard::isKeyPressed(_KEY_DOWN) || Joystick::getAxisPosition(_player, Joystick::Y) > _deadZone) //Avoid bugs when pressing up and down simultaneously
                    return false;
                return Joystick::getAxisPosition(_player, Joystick::Y) < -_deadZone ||
                       Joystick::getAxisPosition(_player, Joystick::PovY) > _deadZone ||
                       Keyboard::isKeyPressed(_KEY_UP);
            case DOWN:
                if(Keyboard::isKeyPressed(_KEY_UP) || Joystick::getAxisPosition(_player, Joystick::Y) < -_deadZone) //Avoid bugs when pressing up and down simultaneously
                    return false;
                return Joystick::getAxisPosition(_player, Joystick::Y) > _deadZone ||
                       Joystick::getAxisPosition(_player, Joystick::PovY) < -_deadZone ||
                       Keyboard::isKeyPressed(_KEY_DOWN);
            case LEFT:
                if(Keyboard::isKeyPressed(_KEY_RIGHT) || Joystick::getAxisPosition(_player, Joystick::X) > _deadZone) //Avoid bugs when pressing left and right simultaneously
                    return false;
                return Joystick::getAxisPosition(_player, Joystick::X) < -_deadZone ||
                       Joystick::getAxisPosition(_player, Joystick::PovX) < -_deadZone ||
                       Keyboard::isKeyPressed(_KEY_LEFT);
            case RIGHT:
                if(Keyboard::isKeyPressed(_KEY_LEFT) || Joystick::getAxisPosition(_player, Joystick::X) < -_deadZone) //Avoid bugs when pressing left and right simultaneously
                    return false;
                return Joystick::getAxisPosition(_player, Joystick::X) > _deadZone ||
                       Joystick::getAxisPosition(_player, Joystick::PovX) > _deadZone ||
                       Keyboard::isKeyPressed(_KEY_RIGHT);
        }
    }
    else
    {
        switch(ResolveOption(action))
        {
            case UP:
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
            case DOWN:
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
            case LEFT:
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
            case RIGHT:
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
            case VALID:
                if(Keyboard::isKeyPressed(_KEY_VALID))
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
            case BACK:
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
            case PAUSE:
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

Options input::ResolveOption(string option)
{
    if(option == "JUMP")
        return JUMP;
    else if(option == "ATTACK")
        return ATTACK;
    else if(option == "RUN")
        return RUN;
    else if(option == "ROLL")
        return ROLL;
    else if(option == "UP")
        return UP;
    else if(option == "DOWN")
        return DOWN;
    else if(option == "LEFT")
        return LEFT;
    else if(option == "RIGHT")
        return RIGHT;
    else if(option =="VALID")
        return VALID;
    else if(option =="BACK")
        return BACK;
    else if(option =="PAUSE")
        return PAUSE;
    else
        return NONE;
}

void input::LockKey(string action, int frames)
{
    switch(ResolveOption(action))
    {
        case JUMP:
            _LOCK_KEY_JUMP = frames;
        case ATTACK:
            _LOCK_KEY_ATTACK = frames;
        case RUN:
            _LOCK_KEY_RUN = frames;
        case ROLL:
            _LOCK_KEY_ROLL = frames;
        case UP:
            _LOCK_KEY_UP = frames;
        case DOWN:
            _LOCK_KEY_DOWN = frames;
        case LEFT:
            _LOCK_KEY_LEFT = frames;
        case RIGHT:
            _LOCK_KEY_RIGHT = frames;
        case VALID:
            _LOCK_KEY_VALID = frames;
        case BACK:
            _LOCK_KEY_BACK = frames;
    }
}

void input::PerformLastController(RenderWindow* window)
{
    if(!_mouseController && Mouse::getPosition(*window) != _lastMousePos)
    {
        _lastMousePos = Mouse::getPosition(*window);
        _mouseController = true;
    }
    else if(_mouseController &&
            (isKeyPressed("JUMP") ||
             isKeyPressed("ATTACK") ||
             isKeyPressed("RUN") ||
             isKeyPressed("ROLL") ||
             isKeyPressed("UP") ||
             isKeyPressed("DOWN") ||
             isKeyPressed("LEFT") ||
             isKeyPressed("RIGHT") ||
             isKeyPressed("VALID") ||
             isKeyPressed("BACK") ||
             isKeyPressed("PAUSE")))
    {
        _lastMousePos = Mouse::getPosition(*window);
        _mouseController = false;
    }
}

void input::PerformUnlockKeys()
{
    if(_LOCK_KEY_JUMP > 0)
        _LOCK_KEY_JUMP --;
    if(_LOCK_KEY_ATTACK > 0)
        _LOCK_KEY_ATTACK --;
    if(_LOCK_KEY_RUN > 0)
        _LOCK_KEY_RUN --;
    if(_LOCK_KEY_ROLL > 0)
        _LOCK_KEY_ROLL --;
    if(_LOCK_KEY_UP > 0)
        _LOCK_KEY_DOWN --;
    if(_LOCK_KEY_LEFT > 0)
        _LOCK_KEY_LEFT --;
    if(_LOCK_KEY_RIGHT > 0)
        _LOCK_KEY_RIGHT --;
    if(_LOCK_KEY_VALID > 0)
        _LOCK_KEY_VALID --;
    if(_LOCK_KEY_BACK > 0)
        _LOCK_KEY_BACK --;
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
        ofstream inputFile(_files.keysFile[_player].c_str(), ios::out | ios::trunc);
        if(inputFile)
        {
            inputFile << "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n";
            inputFile << "<config>\n";
            inputFile << "\t<key type=\"JUMP\">\n\t\t" << _KEY_JUMP << "\n\t</key>\n";
            inputFile << "\t<key type=\"ATTACK\">\n\t\t" << _KEY_ATTACK << "\n\t</key>\n";
            inputFile << "\t<key type=\"ROLL\">\n\t\t" << _KEY_ROLL << "\n\t</key>\n";
            inputFile << "\t<key type=\"RUN\">\n\t\t" << _KEY_RUN << "\n\t</key>\n";
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
