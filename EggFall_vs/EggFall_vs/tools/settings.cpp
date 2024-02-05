#include "settings.hpp"

using namespace std;
using namespace sf;

settings::settings()
{
    //Init
    _language = "Francais";
    _fullscreen = "OFF";
    _timeInSec = 120;
	_analog = "ON";
	_lifePoints = 1;

    //Read xml file
    vector<string> settingsFromXml = xmlReader::getSettingsValue();
    if(settingsFromXml.size() >= 6)
    {
        _language = settingsFromXml[0];
        _fullscreen = settingsFromXml[1];
        _timeString = settingsFromXml[2];
		_analog = settingsFromXml[3];
		_lifeString = settingsFromXml[4];
        _musicString = settingsFromXml[5];
        _effectsString = settingsFromXml[6];

        string time = settingsFromXml[2].substr(0,2);
        _timeInSec = atoi(time.c_str())*60;
        _lifePoints = atoi(settingsFromXml[4].c_str());
        _music = atoi(settingsFromXml[5].c_str());
        _effects = atoi(settingsFromXml[6].c_str());
    }
    else
        save();
}

void settings::save()
{
    files _files;
    stringstream stream;
    stream << _timeInSec/60;
    string time = stream.str() + " min";
    ofstream inputFile(files::settingsFile.c_str(), ios::out | ios::trunc);
    if(inputFile)
    {
        inputFile << "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n";
        inputFile << "<settings>\n";
        inputFile << "\t<option type=\"language\">\n\t\t" << _language << "\n\t</option>\n";
        inputFile << "\t<option type=\"fullscreen\">\n\t\t" << _fullscreen << "\n\t</option>\n";
		inputFile << "\t<option type=\"time\">\n\t\t" << time << "\n\t</option>\n";
		inputFile << "\t<option type=\"analog\">\n\t\t" << _analog << "\n\t</option>\n";
        inputFile << "\t<option type=\"life\">\n\t\t" << _lifePoints << "\n\t</option>\n";
        inputFile << "\t<option type=\"music\">\n\t\t" << _music << "\n\t</option>\n";
        inputFile << "\t<option type=\"effects\">\n\t\t" << _effects << "\n\t</option>\n";
        inputFile << "</settings>";
    }
}

vector<string> settings::getSettingsChoices(MENU_CHOICE menuChoice)
{
	vector<string> choices;
	switch (menuChoice)
	{
		case CHOICE_LANG:
			choices.push_back("Français");
			choices.push_back("English");
			choices.push_back("Español");
			break;
		case CHOICE_ON_OFF:
			choices.push_back("ON");
			choices.push_back("OFF");
			break;
		case CHOICE_TIME:
			choices.push_back("1 min");
			choices.push_back("2 min");
			choices.push_back("3 min");
			choices.push_back("5 min");
			choices.push_back("10 min");
			choices.push_back("15 min");
			break;
		case CHOICE_LIFE:
			choices.push_back("1");
			choices.push_back("2");
			choices.push_back("3");
			break;
		case CHOICE_SOUND:
			choices.push_back("0");
			choices.push_back("1");
			choices.push_back("2");
			choices.push_back("3");
			choices.push_back("4");
			choices.push_back("5");
			choices.push_back("6");
			choices.push_back("7");
			choices.push_back("8");
			choices.push_back("9");
			choices.push_back("10");
			break;
	}
	return choices;
}
