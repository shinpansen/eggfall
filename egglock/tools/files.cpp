#include "files.hpp"

using namespace std;
using namespace sf;

files::files()
{
    //Resources textures
    wizardBlueTile = "resources//characters//redCat.png";// blueWizard.png";
    wizardRedTile = "resources//characters//redWizard.png";
    wizardGreenTile = "resources//characters//greenWizard.png";
    wizardPurpleTile = "resources//characters//purpleWizard.png";
    wizardWhiteTile = "resources//characters//whiteWizard.png";
    wizardDying = "resources//effects//wizardDying.png";
    wizardSelect = "resources//characters//wizardSelect.png";
    dustTile = "resources//effects//dust.png";
    spellTile = "resources//effects//spells.png";
    spellExplosionTile = "resources//effects//spellsExplosions.png";
    trees = "resources//environment//trees.png";
    leeves = "resources//environment//leeves.png";
    layer0 = "resources//tiles//layer0.png";
    layer1 = "resources//tiles//layer1.png";
    life = "resources//UI//life.png";
    titleScreen = "resources//UI//title_screen.png";
    titleScreenBackground = "resources//UI//title_background.png";
    titleScreenTitle = "resources//UI//title.png";
    titleScreenHats = "resources//UI//title2.png";
    menuTopLeft = "resources//UI//menuTopLeft.png";
    menuTopRight = "resources//UI//menuTopRight.png";
    menuBottomLeft = "resources//UI//menuBottomLeft.png";
    menuBottomRight = "resources//UI//menuBottomRight.png";
    menuMiddleBottom = "resources//UI//menuMiddleBottom.png";
    menuDuplicate = "resources//UI//menuDuplicate.png";
    pad360 = "resources//UI//pad360.png";
    pad360Keys = "resources//UI//padKeys.png";
    keyboardKey = "resources//UI//keyboardKey.png";
    keyboardKeyMiddle = "resources//UI//keyboardKeyMiddle.png";
    hatSelector = "resources//UI//hatSelector.png";
    maoSun = "resources//UI//maoSunTile.png";
    cross = "resources//UI//cross.png";
    logo = "resources//UI//logos.png";
    cups = "resources//UI//cups.png";

    //Configuration files
    layer0File = "environment//mapFiles//layer0";
    layer1File = "environment//mapFiles//layer1";
    collisionFile = "environment//mapFiles//collision";
    ladderFile = "environment//mapFiles//ladder";
    treeFile = "environment//mapFiles//tree";

    //Keys
    keysFile = new string[4];
    keysFile[0] = "resources//xml//keys_1.xml";
    keysFile[1] = "resources//xml//keys_2.xml";
    keysFile[2] = "resources//xml//keys_3.xml";
    keysFile[3] = "resources//xml//keys_4.xml";
    settingsFile = "resources//xml//settings.xml";

    //Fonts
    font1 = "resources//fonts//rainyhearts.ttf";
    font2 = "resources//fonts//PIXEL-LI_EDIT.ttf";
}

vector<float> files::readLine(string line)
{
    try
    {
        int i = 0, width = 0;
        Vector2f flux;
        vector<float> resultVector;
        while (i < line.size())
        {
            flux = getLineValue(i, width, line);
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

Vector2f files::getLineValue(int i, int width, string content)
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
