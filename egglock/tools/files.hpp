#ifndef files_HPP
#define files_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>

using namespace std;
using namespace sf;

class files
{
public:
    files();
    static vector<float> readLine(string line);
    static Vector2f getLineValue(int i, int width, string content);
    static bool FileExists(string name);

    //Resources textures
    string wizardBlueTile;
    string wizardRedTile;
    string wizardGreenTile;
    string wizardPurpleTile;
    string wizardWhiteTile;
    string wizardDying;
    string wizardSelect;
    string dustTile;
    string spellTile;
    string spellExplosionTile;
    string trees;
    string leeves;
    string layer0;
    string layer1;
    string life;
    string titleScreen;
    string titleScreenBackground;
    string titleScreenTitle;
    string titleScreenHats;
    string menuTopLeft;
    string menuTopRight;
    string menuBottomLeft;
    string menuBottomRight;
    string menuMiddleBottom;
    string menuDuplicate;
    string pad360;
    string pad360Keys;
    string keyboardKey;
    string keyboardKeyMiddle;
    string hatSelector;
    string maoSun;
    string cross;
    string logo;
    string cups;

    //Configuration files
    string layer0File;
    string layer1File;
    string collisionFile;
    string ladderFile;
    string treeFile;
    string* keysFile;
    string settingsFile;

    //Fonts
    string font1;
    string font2;
};
#endif
