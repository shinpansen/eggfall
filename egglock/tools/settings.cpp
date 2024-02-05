#include "settings.hpp"

using namespace std;
using namespace sf;

settings::settings()
{
    //Init
    _language = "Francais";
    _fullscreen = "OFF";
    _timeInSec = 120;
    _lifePoints = 2;

    //Read xml file
    vector<string> settingsFromXml = xmlReader::getSettingsValue();
    if(settingsFromXml.size() >= 6)
    {
        _language = settingsFromXml[0];
        _fullscreen = settingsFromXml[1];
        _timeString = settingsFromXml[2];
        _lifeString = settingsFromXml[3];
        _musicString = settingsFromXml[4];
        _effectsString = settingsFromXml[5];

        string time = settingsFromXml[2].substr(0,2);
        _timeInSec = atoi(time.c_str())*60;
        _lifePoints = atoi(settingsFromXml[3].c_str());
        _music = atoi(settingsFromXml[4].c_str());
        _effects = atoi(settingsFromXml[5].c_str());
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
    ofstream inputFile(_files.settingsFile.c_str(), ios::out | ios::trunc);
    if(inputFile)
    {
        inputFile << "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n";
        inputFile << "<settings>\n";
        inputFile << "\t<option type=\"language\">\n\t\t" << _language << "\n\t</option>\n";
        inputFile << "\t<option type=\"fullscreen\">\n\t\t" << _fullscreen << "\n\t</option>\n";
        inputFile << "\t<option type=\"time\">\n\t\t" << time << "\n\t</option>\n";
        inputFile << "\t<option type=\"life\">\n\t\t" << _lifePoints << "\n\t</option>\n";
        inputFile << "\t<option type=\"music\">\n\t\t" << _music << "\n\t</option>\n";
        inputFile << "\t<option type=\"effects\">\n\t\t" << _effects << "\n\t</option>\n";
        inputFile << "</settings>";
    }
}
