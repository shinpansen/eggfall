#include "scoreManager.hpp"

using namespace std;
using namespace sf;

scoreManager::scoreManager(effects* effectBox)
{
    //Objects - variables
    _timeInSec = 360;
    _time = 0;
    _animationFinished = false;
	_effectBox = effectBox;

    //Graphics
    _timeLeftA.setSize(Vector2f(3,600));
    _timeLeftA.setOrigin(0,300);
    _timeLeftA.setPosition(0,0);
    _timeRightA.setSize(Vector2f(3,600));
    _timeRightA.setOrigin(3,300);
    _timeRightA.setPosition(800,0);
    _timeTop.setSize(Vector2f(800,3));
    _timeTop.setOrigin(400,0);
    _timeTop.setPosition(400,0);
    _timeBottom.setSize(Vector2f(800,3));
    _timeBottom.setOrigin(400,3);
    _timeBottom.setPosition(400,600);
    _timeLeftA.setFillColor(Color(248,29,67));
    _timeRightA.setFillColor(Color(248,29,67));
    _timeTop.setFillColor(Color(248,29,67));
    _timeBottom.setFillColor(Color(248,29,67));
    _timeLeftB = _timeLeftA;
    _timeLeftB.setPosition(0,600);
    _timeRightB = _timeRightA;
    _timeRightB.setPosition(800,600);

    //Score display
    _wizardSelectTexture.loadFromFile(files::wizardSelect);
    _cupsTexture.loadFromFile(files::cups);
    _font.loadFromFile(files::font2);
    for(int i=0 ; i<4 ; i++)
    {
        //Wizard - cups sprites
        _wizardsResult[i].setTexture(_wizardSelectTexture);
        _wizardsResult[i].setTextureRect(IntRect(0,0,108,138));
        _wizardsResult[i].setOrigin(50,70);
        _cups[i].setTexture(_cupsTexture);
        _cups[i].setTextureRect(IntRect(0,0,96,171));
        _cups[i].setOrigin(48,85.5);

        //Shape
        _scoreShapes[i].setSize(Vector2f(175,450));
        _scoreShapes[i].setOrigin(_scoreShapes[i].getLocalBounds().width/2,225);
        _scoreShapes[i].setFillColor(Color(0,0,0));
        _scoreShapes[i].setOutlineColor(Color(15,15,15));
        _scoreShapes[i].setOutlineThickness(5);
        _colorShapes[i].setSize(Vector2f(175,80));
        _colorShapes[i].setOrigin(_colorShapes[i].getLocalBounds().width/2,40);
        _colorShapes[i].setFillColor(Color(200,0,0));

        //Texts
        _scoreText[i].setFont(_font); _scoreText[i].setCharacterSize(34);
        _scoreText[i].setFillColor(Color(255,205,0));
        _killText[i].setFont(_font); _killText[i].setCharacterSize(22);
        _deathText[i].setFont(_font); _deathText[i].setCharacterSize(22);
        _suicideText[i].setFont(_font); _suicideText[i].setCharacterSize(22);
    }
}

bool scoreManager::timesOver()
{
    return (_time >= _timeInSec);
}

double scoreManager::timePassed()
{
    return _time;
}

void scoreManager::Reset(int timeInSec, string language, bool onlyGraphics)
{
    _animationFinished = false;
    if(!onlyGraphics)
    {
        //Time
        _language = language;
        _timeInSec = timeInSec;
        _time = 0;

        //Scores
        for(int i=0 ; i<_players.size() ; i++)
        {
            _scores[i] = 0;
            _kills[i] = 0;
            _death[i] = 0;
            _suicides[i] = 0;
            _ranks[i] = 1;
        }
    }

    //Graphics
    _timeLeftA.setScale(1,0);
    _timeLeftB.setScale(1,0);
    _timeRightA.setScale(1,0);
    _timeRightB.setScale(1,0);
    _timeTop.setScale(0,1);
    _timeBottom.setScale(0,1);

    //Score display
    int posZero = _players.size() == 2 ? 300 : _players.size() == 3 ? 200 : 100;
    for(int i=0 ; i<_players.size() ; i++)
    {
        //Visuals
        _wizardsResult[i].setTextureRect(IntRect(_players[i]->GetColorID()*108,0,108,138));
        _cups[i].setScale(51,51);
        _cups[i].setColor(Color(255,255,255,0));
        _cups[i].setTextureRect(IntRect(_ranks[i]*96,0,96,171));
        _scoreText[i].setString(GetPlayerText(i, 0));
        _scoreText[i].setOrigin(_scoreText[i].getLocalBounds().width/2, _scoreText[i].getLocalBounds().height/2);
        _killText[i].setString(GetPlayerText(i, 1));
        _killText[i].setOrigin(_killText[i].getLocalBounds().width/2, _killText[i].getLocalBounds().height/2);
        _deathText[i].setString(GetPlayerText(i, 2));
        _deathText[i].setOrigin(_deathText[i].getLocalBounds().width/2, _deathText[i].getLocalBounds().height/2);
        _suicideText[i].setString(GetPlayerText(i, 3));
        _suicideText[i].setOrigin(_suicideText[i].getLocalBounds().width/2, _suicideText[i].getLocalBounds().height/2);
        _colorShapes[i].setFillColor(GetWizardColor(_players[i]->GetColorID()));

        //Positions
        _cups[i].setPosition(posZero+i*200, 465);
        _scoreShapes[i].setPosition(posZero+i*200, i % 2 > 0 ? -840 : 1500); //330
        _colorShapes[i].setPosition(posZero+i*200, _scoreShapes[i].getPosition().y+2);
        _wizardsResult[i].setPosition(posZero+i*200,_scoreShapes[i].getPosition().y-150);
        _scoreText[i].setPosition(posZero+i*200,260); _scoreText[i].setScale(0,0);
        _killText[i].setPosition(posZero+i*200,300); _killText[i].setScale(0,0);
        _killText[i].setFillColor(_players[i]->GetColorID() == 4 ? Color(0,0,0) : Color(255,255,255));
        _deathText[i].setPosition(posZero+i*200,325); _deathText[i].setScale(0,0);
        _deathText[i].setFillColor(_players[i]->GetColorID() == 4 ? Color(0,0,0) : Color(255,255,255));
        _suicideText[i].setPosition(posZero+i*200,350); _suicideText[i].setScale(0,0);
        _suicideText[i].setFillColor(_players[i]->GetColorID() == 4 ? Color(0,0,0) : Color(255,255,255));
    }
}

string scoreManager::GetPlayerText(int i, int type)
{
    if(i < 0 || i > 3)
        return "";

    string typeStr = "";
    stringstream sstream;
    switch(type)
    {
        case 0: //Score
            typeStr = xmlReader::getTranslationText("miscellaneous", _language, "score");
            sstream << _scores[i];
            return typeStr + " " + sstream.str();
            break;
        case 1: //Kills
            typeStr = xmlReader::getTranslationText("miscellaneous", _language, "kill");
            sstream << _kills[i];
            return typeStr + " " + sstream.str();
            break;
        case 2: //Death
            typeStr = xmlReader::getTranslationText("miscellaneous", _language, "death");
            sstream << _death[i];
            return typeStr + " " + sstream.str();
            break;
        case 3: //Suicide
            typeStr = xmlReader::getTranslationText("miscellaneous", _language, "suicide");
            sstream << _suicides[i];
            return typeStr + " " + sstream.str();
            break;
    }
    return "";
}

void scoreManager::Step(float deltaTime)
{
    double sidePart = (double)_timeInSec*3.0/7.0;
    double topPart = (double)_timeInSec*4.0/7.0;
    _timeLeftA.setScale(1, _time > topPart ? (_time-topPart)/sidePart : 0);
    _timeLeftB.setScale(1, _time > topPart ? (_time-topPart)/sidePart : 0);
    _timeRightA.setScale(1, _time > topPart ? (_time-topPart)/sidePart : 0);
    _timeRightB.setScale(1, _time > topPart ? (_time-topPart)/sidePart : 0);
    _timeTop.setScale(_time/topPart, 1);
    _timeBottom.setScale(_time/topPart, 1);
    if(_time < _timeInSec)
        _time += 1.0/60.0*deltaTime;
}

void scoreManager::AddPoints(int points, b2Body* projectileOrigin)
{
    if(projectileOrigin == NULL)
        return;

    for(int i=0 ; i<_players.size() ; i++)
    {
        if(_players[i] != NULL && (_players[i]->GetBodyA() == projectileOrigin || _players[i]->GetBodyB() == projectileOrigin))
        {
            //Change global score
            _scores[i] += points;

            //Stats
			Vector2f textPos = Vector2f(_players[i]->GetPositionA().x - 15, _players[i]->GetPositionA().y - 25);
			if (points == 1)
			{
				_kills[i] += points;
				_effectBox->AddText(drawOption("+1", 20, textPos, Vector2f(0, 0), Color(80, 170, 60)), fadeOption(Vector2f(0, -0.5), 3.f, 0.f, 40), physics(false));
			}
			else if (points == -1)
			{
				_death[i] += 1;
				_effectBox->AddText(drawOption("-1", 20, textPos, Vector2f(0, 0), Color(200, 0, 60)), fadeOption(Vector2f(0, -0.5), 3.f, 0.f, 40), physics(false));
			}
            else if(points == -2)
            {
                _death[i] += 1;
                _suicides[i] += 1;
				_effectBox->AddText(drawOption("-2", 20, textPos, Vector2f(0, 0), Color(200, 0, 60)), fadeOption(Vector2f(0, -0.5), 3.f, 0.f, 40), physics(false));
            }
            break;
        }
    }
    CalculateRanks();
}

void scoreManager::CalculateRanks()
{
    vector<int> forbiddenIDs;
    int lastMax = -99999;
    int maximum = -99999;
    int pos = 0;
    while (forbiddenIDs.size() < _players.size())
    {
        maximum = MaxScores(&forbiddenIDs);
        if(lastMax == maximum)
            _ranks[forbiddenIDs[forbiddenIDs.size()-1]] = _ranks[forbiddenIDs[forbiddenIDs.size()-2]];
        else
            _ranks[forbiddenIDs[forbiddenIDs.size()-1]] = pos;
        lastMax = maximum;
        pos ++;
    }
}

int scoreManager::MaxScores(vector<int> * forbiddenIDs)
{
    int maximum = -99999;
    int id = -1;
    for(int i=0 ; i<_players.size() ; i++)
    {
        if(_scores[i] >= maximum && !VectorContainsID(*forbiddenIDs, i))
        {
            maximum = _scores[i];
            id = i;
        }
    }
    forbiddenIDs->push_back(id);
    return maximum;
}

bool scoreManager::VectorContainsID(vector<int> vect, int id)
{
    for(int i=0 ; i<vect.size() ; i++)
        if(vect[i] == id)
            return true;
    return false;
}

Color scoreManager::GetWizardColor(int numWizard)
{
    switch(numWizard)
    {
        case 0:
            return Color(0,83,119);
        case 1:
            return Color(165,0,14);
        case 2:
            return Color(0,119,0);
        case 3:
            return Color(119,0,119);
        case 4:
            return Color(225,225,225);
        default:
            return Color(0,0,0);
    }
}

bool scoreManager::isAnimationFinished()
{
    return _animationFinished;
}

void scoreManager::Display(RenderWindow* window, bool results)
{
    if(!results)
    {
        window->draw(_timeLeftA);
        window->draw(_timeLeftB);
        window->draw(_timeRightA);
        window->draw(_timeRightB);
        window->draw(_timeTop);
        window->draw(_timeBottom);
    }
    else
    {
        for(int i=0 ; i<_players.size() ; i++)
        {
            window->draw(_scoreShapes[i]);
            window->draw(_wizardsResult[i]);
            window->draw(_colorShapes[i]);
            if(_scoreShapes[i].getPosition().y != 330)
            {
                _scoreShapes[i].move(0, _scoreShapes[i].getPosition().y > 330 ? -30 : 30);
                _wizardsResult[i].setPosition(_wizardsResult[i].getPosition().x,_scoreShapes[i].getPosition().y-150);
                _colorShapes[i].setPosition(_colorShapes[i].getPosition().x, _scoreShapes[i].getPosition().y+2);
            }
            else
            {
                if(_killText[i].getScale().x < 1)
                {
                    float scale = _killText[i].getScale().x;
                    _killText[i].setScale(scale+0.05,scale+0.05);
                    _deathText[i].setScale(scale+0.05,scale+0.05);
                    _suicideText[i].setScale(scale+0.05,scale+0.05);
                }
                else if(_scoreText[i].getScale().x < 1)
                    _scoreText[i].setScale(_scoreText[i].getScale().x+0.05,_scoreText[i].getScale().y+0.05);
                else if(_cups[i].getScale().x > 1)
                {
                    _cups[i].setScale(_cups[i].getScale().x-1, _cups[i].getScale().y-1);
                    _cups[i].setColor(Color(255,255,255,_cups[i].getColor().a+5));
                }

                else
                    _animationFinished = true;

                window->draw(_scoreText[i]);
                window->draw(_killText[i]);
                window->draw(_deathText[i]);
                window->draw(_suicideText[i]);
                window->draw(_cups[i]);
            }
        }
    }

    /*cout << "Scores : ";
    for(int i=0 ; i<_players.size() ; i++)
        cout << i << " : " << _scores[i] << " / ";
    cout << endl;

    cout << "RANK : ";
    for(int i=0 ; i<_players.size() ; i++)
        cout << i << " : " << _ranks[i] << " / ";
    cout << endl;

    cout << "Kills : ";
    for(int i=0 ; i<_players.size() ; i++)
        cout << i << " : " << _kills[i] << " / ";
    cout << endl;

    cout << "Death : ";
    for(int i=0 ; i<_players.size() ; i++)
        cout << i << " : " << _death[i] << " / ";
    cout << endl;

    cout << "Suicide : ";
    for(int i=0 ; i<_players.size() ; i++)
        cout << i << " : " << _suicides[i] << " / ";
    cout << endl;*/
}
