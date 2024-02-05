#include "xmlReader.hpp"

using namespace std;

xmlReader::xmlReader()
{
}

string xmlReader::getTranslationText(string category, string language, string id)
{
    try
    {
        string filePath = "resources//xml//translation.xml";
        if(!files::FileExists(filePath))
            return "translation.xml not found...";
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
        return "NO VALUE";
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
        vector<string> elements;
        string filePath = "resources//xml//translation.xml";
        if(!files::FileExists(filePath))
        {
            elements.push_back("translation.xml not found...");
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
        vector<string> elements;
        string filePath = "resources//xml//keys_" + key + ".xml";
        if(!files::FileExists(filePath))
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
        string filePath = "resources//xml//keys_" + key + ".xml";
        if(!files::FileExists(filePath))
            return "";
        TiXmlDocument doc(filePath.c_str());
        doc.LoadFile();
        TiXmlElement* root = doc.FirstChildElement();
        for(TiXmlElement* elem = root->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement())
        {
            if(elem->Attribute("type") == type)
                return elem->GetText();
        }
        return "NO VALUE";
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
        vector<string> elements;
        string filePath = "resources//xml//settings.xml";
        if(!files::FileExists(filePath))
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
