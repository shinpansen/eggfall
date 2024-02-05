#include "projectiles.hpp"

static const float SCALE = 30.f;
static const int SPELL_MAXSPEED = 30;
static const short GROUND_ENTITY = 0x0001;
static const short PLATFORM_ENTITY = 0x0002;
static const short BODY_ENTITY = 0x0008;

using namespace std;
using namespace sf;

projectiles::projectiles()
{
}

projectiles::projectiles(b2World* world, effects* effectBox, sound* soundBox, maps * map)
{
	//Objects
    _world = world;
    _effectBox = effectBox;
    _soundBox = soundBox;
	_map = map;
    
	//Loading textures
	if (!_spellsTexture.loadFromFile(files::spellTile))
        cout << "Unable to find spells texture '" << files::spellTile << "'" << endl;
}

b2Body* projectiles::AddSpell(Vector2f pos, float frameDelay, int width, int height, int nbFrames,
							  int numAnimation, int angle, Vector2f speed, Color color)
{
    try
    {
		_spells.push_back(new spellObj(pos, speed, &_spellsTexture, numAnimation, color, _world));
		return _spells[_spells.size() - 1]->GetBody();
    }
    catch(exception e)
    {
        cout << "An error occurred while adding spell : " << e.what() << endl;
		return NULL;
    }
}

void projectiles::ShowGroundParticleEffect(Vector2f contactPos, Vector2f projectileDir)
{
	//Finding ground type
	string groundType = _map->GetContactGroundType(contactPos);
	if (_map->GetContactGroundType(contactPos) == "")
		return;

	//Fading options - 100 frames alive - starts to dissapear 80 frames later (at frame 20)
	fadeOption foParticle = fadeOption(Vector2f(0, 0), 12.f, 0.f, 100.f, 0.f, Vector2f(0, 0), 20.f);;
	int numberOfParticle = utils::RandomNumber(10, 6);
	
	//Drawing particles
	for (int i = 0; i < numberOfParticle; i++)
	{
		//Random common parameters
		int particleSpeed = utils::RandomNumber(15, 8);
		int particleGravity = utils::RandomNumber(9, 6);
		int particleAngleChange = i < numberOfParticle / 2 ? -1 * utils::RandomNumber(35, 0) : utils::RandomNumber(35, 0);
		Vector2f particleDir = Vector2f(-projectileDir.x*particleSpeed / 2.f, -fabs(projectileDir.y)*particleSpeed*1.5f);
		particleDir = utils::RotateVector(particleDir, particleAngleChange);
		while (fabs(particleDir.x) > 100.f)
			particleDir.x /= 2.f;
		physics phyParticle = physics(true, _world, particleDir, 10, 0.02, ((float)particleGravity) / 10.f);

		//Specific effect (shape, size and color) depending ground type
		if (groundType == "dirt")
		{
			int dirtSize = utils::RandomNumber(3, 1);
			int randomColor = utils::RandomNumber(_map->dirtColors.size() - 1, 0);
			drawOption doDirt = drawOption(contactPos, Vector2f(dirtSize, dirtSize), _map->dirtColors[randomColor]);
			_effectBox->AddCircleShape(doDirt, foParticle, phyParticle);
		}
		else if (groundType == "rock")
		{
			int rockWidth = utils::RandomNumber(6, 3);
			int rockHeight = utils::RandomNumber(rockWidth, rockWidth/2);
			int randomColor = utils::RandomNumber(_map->rockColors.size() - 1, 0);
			drawOption doRock = drawOption(contactPos, Vector2f(rockWidth, rockHeight), _map->rockColors[randomColor]);
			_effectBox->AddRectangleShape(doRock, foParticle, phyParticle);
		}
		else if (groundType == "wood_a" || groundType == "wood_b")
		{
			int rockWidth = utils::RandomNumber(7, 4);
			int rockHeight = utils::RandomNumber(rockWidth/2, 1);
			int randomColor = utils::RandomNumber(_map->woodAColors.size() - 1, 0);
			if(groundType == "wood_b")
				utils::RandomNumber(_map->woodBColors.size() - 1, 0);
			Color woodColor = groundType == "wood_a" ? _map->woodAColors[randomColor] : _map->woodBColors[randomColor];
			drawOption doWood = drawOption(contactPos, Vector2f(rockWidth, rockHeight), woodColor);
			_effectBox->AddRectangleShape(doWood, foParticle, phyParticle);
		}
	}
}

void projectiles::DestroyEverything()
{
	//Destroying spells objetcs
	while (_spells.size() > 0)
		delete _spells[0];
	_spells.clear();
}

void projectiles::UpdatePlayersPointers(vector<hero*>* players)
{
	_players = players;
}

void projectiles::RenderProjectiles(float deltaTime)
{
    try
    {
        _deltaTime = deltaTime;

		//Rendering spells
		vector<spellObj*> spellsClean; // Clean vector without NULL objects
		for (int i = 0; i < _spells.size(); i++)
		{
			if (_spells[i] != NULL && _spells[i]->IsDestroyed())
			{
				//Show particle effect
				if (_spells[i]->GetDestroyedOnGround())
					ShowGroundParticleEffect(_spells[i]->GetLastContactPos(), _spells[i]->GetDirection());

				//Delete
				delete _spells[i];
				_spells[i] = NULL;
			}
			else if (_spells[i] != NULL)
			{
				_spells[i]->Step(_effectBox, _soundBox, *_players, _deltaTime);
				spellsClean.push_back(_spells[i]);
			}
		}
		_spells = spellsClean;
    }
    catch(exception e)
    {
        cout << e.what() << endl;
    }
}

void projectiles::Display(RenderWindow* window)
{
	//Drawing spells
	for (int i = 0; i < _spells.size(); i++)
		if (_spells[i] != NULL)
			_spells[i]->Display(window);
}
