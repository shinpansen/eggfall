#ifndef xmlReader_HPP
#define xmlReader_HPP

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <fstream>
#include <vector>
#include "../tinyXML/tinyxml.h"
#include "files.hpp"

using namespace std;

class xmlReader
{
public:
    xmlReader();
    static string getTranslationText(string category, string language, string id);
    static vector<string> getTranslationTexts(string category, string language);
    static vector<string> getKeysValue(string key);
    static string getKeyValue(string key, string type);
    static vector<string> getSettingsValue();
};
#endif
