#ifndef projectiles_hpp
#define projectiles_hpp

#include <iostream>
#include <math.h>
#include <vector>
#include <random>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <Box2D/Box2D.h>
#include "../tools/Box2DTools.hpp"
#include "../tools/files.hpp"
#include "../tools/utils.hpp"
#include "../characters/hero.hpp"
#include "../effects/effects.hpp"
#include "../effects/animation.hpp"
#include "../effects/sound.hpp"
#include "../environment/maps.hpp"
#include "projectileObj.hpp"

using namespace std;
using namespace sf;

class hero;
class maps;
class projectileObj;
class projectiles
{
public:
	projectiles();
	projectiles(b2World* world, effects* effectBox, sound* soundBox, maps* map);
    b2Body* AddSpell(Vector2f pos, float frameDelay, int width, int height, int nbFrames, int numAnimation, 
		int angle, Vector2f speed, Color color, b2Body * playerOrigin);
	vector<Vector2f> GetProjectilesPos(bool movingOnly);
	void ShowGroundParticleEffect(Vector2f contactPos, Vector2f projectileDir);
    void DestroyEverything();
    void UpdatePlayersPointers(vector<hero*>* players);
    void RenderProjectiles(float deltaTime);
    void Display(RenderWindow* window);
private:
	//Variables
    float _deltaTime;

	//Objects
    b2World* _world;
    effects* _effectBox;
    sound* _soundBox;
	maps* _map;
	vector<hero*>* _players;
	vector<projectileObj*> _projectiles;
    Texture _spellsTexture;
};
#endif
