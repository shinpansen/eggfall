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

b2Body* projectiles::AddSpell(Vector2f pos, float frameDelay, int width, int height, int nbFrames, int numAnimation, 
	int angle, Vector2f speed, Color color, b2Body * playerOrigin)
{
    try
    {
		projectileObj * pObj = new projectileObj(pos, speed, &_spellsTexture, numAnimation, color, _world, playerOrigin);
		_projectiles.push_back(pObj);
		return pObj->GetBody();
    }
    catch(exception e)
    {
        cout << "An error occurred while adding spell : " << e.what() << endl;
		return NULL;
    }
}

vector<Vector2f> projectiles::GetProjectilesPos(bool movingOnly)
{
	vector<Vector2f> projectilesPos;
	for (int i = 0; i < _projectiles.size(); i++)
		if((movingOnly && (fabs(_projectiles[i]->GetDirection().x) > 0.1f ||
			fabs(_projectiles[i]->GetDirection().y) > 0.1f)) || !movingOnly)
			projectilesPos.push_back(_projectiles[i]->GetPosition());
	return projectilesPos;
}

void projectiles::ShowGroundParticleEffect(Vector2f contactPos, Vector2f projectileDir)
{
	//Finding ground type
	string groundType = _map->GetContactGroundType(contactPos);
	if (groundType == "")
		return;

	//Fading options - 100 frames alive - starts to dissapear 80 frames later (at frame 20)
	fadeOption foParticle = fadeOption(Vector2f(0, 0), 12.f, 0.f, 100.f, 0.f, Vector2f(0, 0), 20.f);
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
		if (groundType == "dirt_a" || groundType == "dirt_b")
		{
			int dirtSize = utils::RandomNumber(3, 1);
			int randomColor = utils::RandomNumber(_map->dirtAColors.size() - 1, 0);
			drawOption doDirt = drawOption(contactPos, Vector2f(dirtSize, dirtSize), groundType == "dirt_a" ? _map->dirtAColors[randomColor] : _map->dirtBColors[randomColor]);
			_effectBox->AddCircleShape(doDirt, foParticle, phyParticle);
		}
		else if (groundType == "rock_a" || groundType == "rock_b")
		{
			int rockWidth = utils::RandomNumber(6, 3);
			int rockHeight = utils::RandomNumber(rockWidth, rockWidth/2);
			int randomColor = utils::RandomNumber(_map->rockAColors.size() - 1, 0);
			drawOption doRock = drawOption(contactPos, Vector2f(rockWidth, rockHeight), groundType == "rock_a" ? _map->rockAColors[randomColor] : _map->rockBColors[randomColor]);
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
	for(int i=0 ; i<_projectiles.size() ; i++)
		if(_projectiles[i] != NULL)
			delete _projectiles[i];
	_projectiles.clear();
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
		vector<projectileObj*> projectilesClean; // Clean vector without NULL objects
		for (int i = 0; i < _projectiles.size(); i++)
		{
			if (_projectiles[i] != NULL && _projectiles[i]->IsDestroyed())
			{
				//Show particle effect
				if (_projectiles[i]->GetDestroyedOnGround())
					ShowGroundParticleEffect(_projectiles[i]->GetLastContactPos(), _projectiles[i]->GetDirection());

				//Delete
				delete _projectiles[i];
				_projectiles[i] = NULL;
			}
			else if (_projectiles[i] != NULL)
			{
				_projectiles[i]->Step(_effectBox, _soundBox, *_players, _deltaTime);
				projectilesClean.push_back(_projectiles[i]);
			}
		}
		_projectiles = projectilesClean;
    }
    catch(exception e)
    {
        cout << e.what() << endl;
    }
}

void projectiles::Display(RenderWindow* window)
{
	//Drawing spells
	for (int i = 0; i < _projectiles.size(); i++)
		if (_projectiles[i] != NULL)
			_projectiles[i]->Display(window);
}
