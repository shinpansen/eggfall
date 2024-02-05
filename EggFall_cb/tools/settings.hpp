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

enum MENU_CHOICE
{
	CHOICE_LANG,
	CHOICE_ON_OFF,
	CHOICE_TIME,
	CHOICE_LIFE,
	CHOICE_SOUND
};

class settings
{
public:
	//Methods
    settings();
    void save();
	vector<string> getSettingsChoices(MENU_CHOICE menuChoice);

	//Variables
    string _language;
    string _fullscreen;
    string _timeString;
	string _analog;
    string _lifeString;
    string _musicString;
    string _effectsString;
    int _timeInSec;
    int _lifePoints;
    int _music;
    int _effects;
};
#endif
