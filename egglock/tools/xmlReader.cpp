#include "xmlReader.hpp"

using namespace std;

xmlReader::xmlReader()
{
}

string xmlReader::getTranslationText(string category, string language, string id)
{
    try
    {
        files files;
        string filePath = "resources//xml//translation.xml";
        if(!files.FileExists(filePath))
            return "null";
        TiXmlDocument doc(filePath.c_str());
        doc.LoadFile();
        TiXmlElement* root = doc.FirstChildElement();
        for(TiXmlElement* elem = root->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement())
        {
            if(elem->Value() == category &&
               elem->Attribute("language") == language &&
               elem->Attribute("id") == id)
                return elem->GetText();
        }
        return "null";
    }
    catch(exception e)
    {
        cout << e.what() << endl;
    }
}

vector<string> xmlReader::getTranslationTexts(string category, string language)
{
    try
    {
        files files;
        vector<string> elements;
        string filePath = "resources//xml//translation.xml";
        if(!files.FileExists(filePath))
        {
            elements.push_back("translation.xml missing...");
            return elements;
        }
        TiXmlDocument doc(filePath.c_str());
        doc.LoadFile();
        TiXmlElement* root = doc.FirstChildElement();
        for(TiXmlElement* elem = root->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement())
        {
            if(elem->Value() == category &&
               elem->Attribute("language") == language)
                elements.push_back(elem->GetText());
        }
        return elements;
    }
    catch(exception e)
    {
        cout << e.what() << endl;
    }
}

vector<string> xmlReader::getKeysValue(string key)
{
    try
    {
        files files;
        vector<string> elements;
        string filePath = "resources//xml//keys_" + key + ".xml";
        if(!files.FileExists(filePath))
            return elements;
        TiXmlDocument doc(filePath.c_str());
        doc.LoadFile();
        TiXmlElement* root = doc.FirstChildElement();
        for(TiXmlElement* elem = root->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement())
        {
            elements.push_back(elem->GetText());
        }
        return elements;
    }
    catch(exception e)
    {
        cout << e.what() << endl;
    }
}

string xmlReader::getKeyValue(string key, string type)
{
    try
    {
        files files;
        string filePath = "resources//xml//keys_" + key + ".xml";
        if(!files.FileExists(filePath))
            return "";
        TiXmlDocument doc(filePath.c_str());
        doc.LoadFile();
        TiXmlElement* root = doc.FirstChildElement();
        for(TiXmlElement* elem = root->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement())
        {
            if(elem->Attribute("type") == type)
                return elem->GetText();
        }
        return "";
    }
    catch(exception e)
    {
        cout << e.what() << endl;
    }
}

vector<string> xmlReader::getSettingsValue()
{
    try
    {
        files files;
        vector<string> elements;
        string filePath = "resources//xml//settings.xml";
        if(!files.FileExists(filePath))
            return elements;
        TiXmlDocument doc(filePath.c_str());
        doc.LoadFile();
        TiXmlElement* root = doc.FirstChildElement();
        for(TiXmlElement* elem = root->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement())
        {
            elements.push_back(elem->GetText());
        }
        return elements;
    }
    catch(exception e)
    {
        cout << e.what() << endl;
    }
}

vector<string> xmlReader::getSettingsChoices(int menu)
{
    vector<string> choices;
    switch(menu)
    {
        case 0:
            choices.push_back("Francais");
            choices.push_back("English");
            choices.push_back("Espanol");
            break;
        case 1:
            choices.push_back("ON");
            choices.push_back("OFF");
            break;
        case 2:
            choices.push_back("1 min");
            choices.push_back("2 min");
            choices.push_back("3 min");
            choices.push_back("5 min");
            choices.push_back("10 min");
            choices.push_back("15 min");
            break;
        case 3:
            choices.push_back("1");
            choices.push_back("2");
            choices.push_back("3");
            choices.push_back("4");
            choices.push_back("5");
            break;
        case 4:
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
