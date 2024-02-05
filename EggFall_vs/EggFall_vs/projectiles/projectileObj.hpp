#ifndef projectileObj_HPP
#define projectileObj_HPP

#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <Box2D\Box2D.h>
#include "../effects/animation.hpp"
#include "../effects/effects.hpp"
#include "../effects/sound.hpp"
#include "../characters/hero.hpp"
#include "../tools/Box2DTools.hpp"
#include "../tools/utils.hpp"

using namespace std;
using namespace sf;

class hero;
class projectileObj
{
public:
	~projectileObj();
	projectileObj();
	projectileObj(Vector2f pos, Vector2f direction, Texture* texture, int numAnimation, Color color, b2World* world, b2Body* playerOrigin);
	b2Body* GetBody();
	Vector2f GetPosition();
	Vector2f GetLastContactPos();
	Vector2f GetDirection();
	float GetSpeed();
	void SetDirection(Vector2f direction);
	void DestroyProjectile(effects * effectBox, sound* soundBox);
	bool GetDestroyedOnGround();
	bool IsDestroyed();
	bool IsValidContact(b2Contact* contact);
	bool IsAPlayer(b2Body* body, vector<hero*> players, int* numPlayer);
	void StunPlayer(hero* player);
	void ShowDestructionEffect(effects * effectBox, sound* soundBox, float x, float y);
	void PerformContacts(effects* effectBox, sound* soundBox, vector<hero*> players);
	void PerformAutoAim(vector<hero*> players, float deltaTime);
	void PassThroughPlatforms();
	void Step(effects* effectBox, sound* soundBox, vector<hero*> players, float deltaTime);
	void Display(RenderWindow* window);
private:
	//Variables
	Vector2f _direction;
	Vector2f _lastContactPos;
	b2Vec2 _lastPos;
	bool _destroyed;
	bool _destroyedOnGround;
	bool _isCounteracted;
	float _lightFrame;
	float _lightFrame2;
	float _aimingSpeed;
	float _lifeTimeout;
	int _numAnimation;

	//Objects
	Sprite * _sprite;
	animation * _animation;
	CircleShape * _light;
	b2World * _world;
	b2Body * _body;
	b2Body* _playerOrigin;
};
#endif
