#include "files.hpp"

using namespace std;
using namespace sf;

//Resources textures
const string files::wizardBlueTile_1 = "resources//characters//blue_wizard_1.png";
const string files::wizardBlueTile_2 = "resources//characters//blue_wizard_2.png";
const string files::wizardBlueTile_3 = "resources//characters//blue_wizard_3.png";
const string files::wizardRedTile = "resources//characters//redWizard.png";
const string files::wizardGreenTile = "resources//characters//greenWizard.png";
const string files::wizardPurpleTile = "resources//characters//purpleWizard.png";
const string files::wizardWhiteTile = "resources//characters//whiteWizard.png";
const string files::wizardDying = "resources//effects//wizard_dying.png";
const string files::wizardSelect = "resources//characters//wizard_select.png";
const string files::dustTile = "resources//effects//dust.png";
const string files::spellTile = "resources//effects//spells.png";
const string files::spellExplosionTile = "resources//effects//spells_explosions.png";
const string files::trees = "resources//environment//trees.png";
const string files::leeves = "resources//environment//leeves.png";
const string files::groundTile = "resources//environment//ground_tile.png";
const string files::grassClumps = "resources//environment//grass_clumps.png";
const string files::ladderPlatform = "resources//environment//ladder_platform.png";
const string files::layer0 = "resources//tiles//layer0.png";
const string files::layer1 = "resources//tiles//layer1.png";
const string files::life = "resources//UI//life.png";
const string files::titleScreen = "resources//UI//title_screen.png";
const string files::titleScreenBackground = "resources//UI//title_background.png";
const string files::titleScreenTitle = "resources//UI//title.png";
const string files::titleScreenHats = "resources//UI//title2.png";
const string files::menuTopLeft = "resources//UI//menu_top_left.png";
const string files::menuTopRight = "resources//UI//menu_top_right.png";
const string files::menuBottomLeft = "resources//UI//menu_bottom_left.png";
const string files::menuBottomRight = "resources//UI//menu_bottom_right.png";
const string files::menuMiddleBottom = "resources//UI//menu_middle_bottom.png";
const string files::menuDuplicate = "resources//UI//menu_duplicate.png";
const string files::aimArrow = "resources//UI//aim_arrow.png";
const string files::pad360 = "resources//UI//pad360.png";
const string files::pad360Keys = "resources//UI//pad_keys.png";
const string files::keyboardKey = "resources//UI//keyboard_key.png";
const string files::keyboardKeyMiddle = "resources//UI//keyboard_key_middle.png";
const string files::hatSelector = "resources//UI//hat_selector.png";
const string files::maoSun = "resources//UI//mao_sun_tile.png";
const string files::cross = "resources//UI//cross.png";
const string files::logo = "resources//UI//logos.png";
const string files::cups = "resources//UI//cups.png";
const string files::backgroundA = "resources//backgrounds//mountains_sun.png";
const string files::backgroundB = "resources//backgrounds//mountains_sun2.png";
const string files::backgroundC = "resources//backgrounds//lowland.png";
const string files::cloudsA = "resources//backgrounds//clouds.png";
const string files::cloudsB = "resources//backgrounds//clouds2.png";
const string files::fog = "resources//backgrounds//fog.png";

//Configuration files
const string files::layer0File = "environment//mapFiles//layer0";
const string files::layer1File = "environment//mapFiles//layer1";
const string files::collisionFile = "environment//mapFiles//collision";
const string files::ladderFile = "environment//mapFiles//ladder";
const string files::treeFile = "environment//mapFiles//tree";

//Keys
const string* files::keysFile = keys();
const string files::settingsFile = "resources//xml//settings.xml";

//Fonts
const string files::font1 = "resources//fonts//rainyhearts.ttf";
const string files::font2 = "resources//fonts//PIXEL-LI_EDIT.ttf";
const string files::font3 = "resources//fonts//thirteen_pixel_fonts.ttf";

files::files()
{
    
}

vector<float> files::ReadLine(string line)
{
    try
    {
        int i = 0, width = 0;
        Vector2f flux;
        vector<float> resultVector;
        while (i < line.size())
        {
            flux = GetLineValue(i, width, line);
            i = width = flux.x;
            resultVector.push_back(flux.y);
        }
        return resultVector;
    }
    catch(exception e)
    {
        cout << e.what() << endl;
    }
}

Vector2f files::GetLineValue(int i, int width, string content)
{
    try
    {
        string str;
        float nb = 0;
        while(i <= content.size())
        {
            if(content.substr(i,1) == ",")
            {
                str = content.substr(width, i-width);
                istringstream(str) >> nb;
                width = i + 1;
                break;
            }
            i++;
        }
        return Vector2f(width, nb);
    }
    catch(exception e)
    {
        cout << e.what() << endl;
    }
}

bool files::FileExists(string name)
{
    ifstream f(name.c_str());
    if (f.good())
    {
        f.close();
        return true;
    }
    else
    {
        f.close();
        return false;
    }
}

string * files::keys()
{
	string* keysFile = new string[4];
	keysFile[0] = "resources//xml//keys_1.xml";
	keysFile[1] = "resources//xml//keys_2.xml";
	keysFile[2] = "resources//xml//keys_3.xml";
	keysFile[3] = "resources//xml//keys_4.xml";
	return keysFile;
}
