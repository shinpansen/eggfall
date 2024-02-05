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
    static vector<float> ReadLine(string line);
    static Vector2f GetLineValue(int i, int width, string content);
    static bool FileExists(string name);
	static string* keys();

    //Resources textures
	static const string wizardBlueTile_1;
	static const string wizardBlueTile_2;
	static const string wizardBlueTile_3;
	static const string wizardRedTile;
	static const string wizardGreenTile;
	static const string wizardPurpleTile;
	static const string wizardWhiteTile;
	static const string wizardDying;
	static const string wizardSelect;
	static const string dustTile;
	static const string spellTile;
	static const string spellExplosionTile;
	static const string trees;
	static const string leeves;
	static const string groundTile;
	static const string grassClumps;
	static const string ladderPlatform;
	static const string layer0;
	static const string layer1;
	static const string life;
	static const string titleScreen;
	static const string titleScreenBackground;
	static const string titleScreenTitle;
	static const string titleScreenHats;
	static const string menuTopLeft;
	static const string menuTopRight;
	static const string menuBottomLeft;
	static const string menuBottomRight;
	static const string menuMiddleBottom;
	static const string menuDuplicate;
	static const string aimArrow;
	static const string pad360;
	static const string pad360Keys;
	static const string keyboardKey;
	static const string keyboardKeyMiddle;
	static const string hatSelector;
	static const string maoSun;
	static const string cross;
	static const string logo;
	static const string cups;
	static const string backgroundA;
	static const string backgroundB;
	static const string backgroundC;
	static const string cloudsA;
	static const string cloudsB;
	static const string fog;

    //Configuration files
	static const string layer0File;
	static const string layer1File;
	static const string collisionFile;
	static const string ladderFile;
	static const string treeFile;
	static const string* keysFile;
	static const string settingsFile;

    //Fonts
	static const string font1;
	static const string font2;
	static const string font3;
};
#endif
