#ifndef scoreManager_HPP
#define scoreManager_HPP

#include <iostream>
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>
#include "../../characters/hero.hpp"
#include "../../effects/sound.hpp"
#include "../../tools/files.hpp"
#include "../../tools/xmlReader.hpp"
#include "../../effects/effects.hpp"

using namespace std;
using namespace sf;

class hero;
class scoreManager
{
public:
    scoreManager(effects* effectBox);
    bool timesOver();
    double timePassed();
	void UpdatePlayersPointers(vector<hero*>* players);
    void Reset(int timeInSec = 10, string language = "", bool onlyGraphics = false);
    string GetPlayerText(int i, int type);
	void AddPoints(int points, b2Body* projectileOrigin);// int numWizard);
    void CalculateRanks();
    int MaxScores(vector<int> * forbiddenIDs);
    bool VectorContainsID(vector<int> vect, int id);
    Color GetWizardColor(int numWizard);
    bool isAnimationFinished();
    void Step(float deltaTime);
    void Display(RenderWindow* window, bool results = false);
    string _language;
private:
    //Objects - variables
    double _time;
    int _timeInSec;
    int _scores[4] = {};
    int _kills[4] = {};
    int _death[4] = {};
    int _suicides[4] = {};
    int _ranks[4] = {};
    bool _animationFinished;
	effects* _effectBox;
	vector<hero*>* _players;

    //Graphics
    RectangleShape _timeLeftA;
    RectangleShape _timeLeftB;
    RectangleShape _timeRightA;
    RectangleShape _timeRightB;
    RectangleShape _timeTop;
    RectangleShape _timeBottom;

    //Score display
    Texture _wizardSelectTexture;
    Texture _cupsTexture;
    Sprite _wizardsResult[4];
    Sprite _cups[4];
    RectangleShape _scoreShapes[4];
    RectangleShape _colorShapes[4];

    //Texts
    Font _font;
    Text _scoreText[4];
    Text _killText[4];
    Text _deathText[4];
    Text _suicideText[4];
};
#endif
