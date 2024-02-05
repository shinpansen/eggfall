#ifndef settings_HPP
#define settings_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include "xmlReader.hpp"

using namespace std;
using namespace sf;

class settings
{
public:
    settings();
    void save();
    string _language;
    string _fullscreen;
    string _timeString;
    string _lifeString;
    string _musicString;
    string _effectsString;
    int _timeInSec;
    int _lifePoints;
    int _music;
    int _effects;
};
#endif
