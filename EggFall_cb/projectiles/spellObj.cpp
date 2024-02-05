#include "spellObj.hpp"

static const float SCALE = 30.f;
static const float AIMING_AREA = 200.f;
static const float LIFE_TIMEOUT = 300.f;

using namespace std;
using namespace sf;

spellObj::~spellObj()
{
	if (_world != NULL && _body != NULL)
		_world->DestroyBody(_body);
}

spellObj::spellObj()
{
	//Initialization
	_direction = Vector2f(0, 0);
	_destroyed = false;
	_destroyedOnGround = false;
	_lightFrame = 1.f;
	_numAnimation = 0;
	_aimingSpeed = 3.f;
	_lifeTimeout = LIFE_TIMEOUT;
	_world = NULL;
	_body = NULL;
}

spellObj::spellObj(Vector2f pos, Vector2f direction, Texture* texture, int numAnimation, Color color, b2World* world)
{
	//Initialization
	_direction = direction;
	_destroyed = false;
	_destroyedOnGround = false;
	_lightFrame = 1.f;
	_numAnimation = numAnimation;
	_aimingSpeed = 3.f;
	_lifeTimeout = LIFE_TIMEOUT;
	_world = world;
	float angle = (float)utils::GetVectorRotation(direction) - 90.f;

	//Sprite - Animation
	_sprite.setTexture(*texture);
	_sprite.setOrigin(17.5f, 12.5f);
	_sprite.setRotation(angle);
	_animation = animation(&_sprite, 0.3, 35, 25, 6, numAnimation+1, -1, true);
	_animation.SetAnimation(numAnimation, 6);

	//CircleShape light
	_light.setRadius(18);
	_light.setOrigin(18,18);
	_light.setFillColor(Color(color.r, color.g, color.b, 100));
	_light.setOutlineColor(Color(color.r, color.g, color.b, 50));
	_light.setOutlineThickness(7);
	_light.setPosition(pos.x, pos.y);

	//Box2D
	if (_world != NULL)
	{
		_body = Box2DTools::CreateCircleBox(_world, pos, 20, 0.5, 1, Box2DTools::BODY_ENTITY);
		_body->SetUserData(NULL);
		_body->SetGravityScale(0);
		_body->SetLinearVelocity(b2Vec2(direction.x, direction.y));
		_body->SetTransform(b2Vec2(pos.x / SCALE, pos.y / SCALE), utils::DegreeToRadian(angle));
	}
}

b2Body * spellObj::GetBody()
{
	return _body;
}

Vector2f spellObj::GetDirection()
{
	return _direction;
}

Vector2f spellObj::GetLastContactPos()
{
	return _lastContactPos;
}

bool spellObj::GetDestroyedOnGround()
{
	return _destroyedOnGround;
}

bool spellObj::IsDestroyed()
{
	return _destroyed || _lifeTimeout <= 0.f;
}

bool spellObj::IsValidContact(b2Contact * contact)
{
	short entities[3] = { Box2DTools::GROUND_ENTITY, Box2DTools::PLATFORM_ENTITY, Box2DTools::BODY_ENTITY };
	for (int i = 0; i < 3; i++)
		if (contact->GetFixtureA()->GetFilterData().categoryBits == entities[i] ||
			contact->GetFixtureB()->GetFilterData().categoryBits == entities[i])
			return true;
	return false;
}

bool spellObj::IsAPlayer(b2Body* body, vector<hero*> players, int* numPlayer)
{
	for (int i = 0; i < players.size(); i++)
	{
		if (players[i]->GetBodyA() == body || players[i]->GetBodyB() == body)
		{
			*numPlayer = i;
			return true;
		}
	}
	return false;
}

void spellObj::StunPlayer(hero * player)
{
	//Calculating stun direction
	double speedx = _body->GetLinearVelocity().x != 0 ? (_body->GetLinearVelocity().x / fabs(_body->GetLinearVelocity().x)) * 10 : 0;
	double speedy = _body->GetLinearVelocity().y != 0 ? (_body->GetLinearVelocity().y / fabs(_body->GetLinearVelocity().y)) * 6 : 0;
	speedy = fabs(speedy) <= 0.1 ? -2 : speedy;

	//Stunning player + destroying spell + adding effect
	player->SetStunned(Vector2f(speedx, speedy), 15, (b2Body*)_body->GetUserData());// _numAnimation);
}

void spellObj::ShowDestructionEffect(effects * effectBox, sound * soundBox, float x, float y)
{
	effectBox->ShakeScreen(2, 5);
	effectBox->AddAnimation(T_SPELLS, _numAnimation * 2, 0.3, false, drawOption(Vector2f(x, y)), fadeOption(0), physics(false));
	soundBox->PlaySound(SOUND::SND_EXPLOSION_A, false);
}

void spellObj::PerformContacts(effects * effectBox, sound* soundBox, vector<hero*> players)
{
	try
	{
		//Converting body pos into pixel units
		float x = _body->GetPosition().x*SCALE;
		float y = _body->GetPosition().y*SCALE;

		//Iterating through body contact list
		for (b2ContactEdge* cte = _body->GetContactList(); cte != NULL; cte = cte->next)
		{
			b2Contact* contact = cte->contact;
			if (!contact->IsTouching() || !contact->IsEnabled() || !IsValidContact(contact))
				continue;

			//Calculating contact location
			_lastContactPos = Box2DTools::CalculateAdjustedContactPos(contact, _direction);

			int numPlayer;
			if (IsAPlayer(contact->GetFixtureA()->GetBody(), players, &numPlayer) ||
				IsAPlayer(contact->GetFixtureB()->GetBody(), players, &numPlayer))
			{
				//Contact with a player
				if (players[numPlayer]->IsSelfProjectileInvincible(contact))
					return;
				else if (players[numPlayer]->isCounteracting())
				{
					//Finding player body
					b2Body *playerBody = NULL, *projectileBody = NULL;
					if (players[numPlayer]->GetBodyA() == contact->GetFixtureA()->GetBody() ||
						players[numPlayer]->GetBodyB() == contact->GetFixtureA()->GetBody())
						playerBody = contact->GetFixtureA()->GetBody();
					else
						playerBody = contact->GetFixtureB()->GetBody();

					//Testing if countering correctly done
					int ctDir = players[numPlayer]->isCounteracting();
					bool verticalDir = fabs(_body->GetLinearVelocity().x) / fabs(_body->GetLinearVelocity().y) < 0.1;
					if (verticalDir || // When projectile direction is nearly vertical, countering is always ok
						(!verticalDir && ctDir == 1 && _body->GetPosition().x > playerBody->GetPosition().x) ||
						(!verticalDir && ctDir == -1 && _body->GetPosition().x < playerBody->GetPosition().x))
					{
						//Countering - Inverting/increasing speed
						//All contact between the player and the spell will be ignored during some frames
						players[numPlayer]->SetSelfProjectileInvincible(_body);
						b2Vec2 speed = _body->GetLinearVelocity();
						_direction = Vector2f(-speed.x*1.3f, -speed.y*1.3f);
						_aimingSpeed *= 1.2f;
						_lifeTimeout = LIFE_TIMEOUT;
						_body->SetLinearVelocity(b2Vec2(_direction.x, _direction.y));

						//Adding effect
						soundBox->PlaySound(SOUND::SND_COUNTERING, false);
						Sprite* effect = effectBox->AddAnimation(T_SPELLS, _numAnimation * 2 + 1, 0.3, false, drawOption(Vector2f(75, 75)), fadeOption(0), physics(false));
						effect->setRotation(utils::GetVectorRotation(_direction) + 90.f);
						effect->setPosition(Vector2f(_lastContactPos.x, _lastContactPos.y));
					}
					else
					{
						//Failed
						StunPlayer(players[numPlayer]);
						ShowDestructionEffect(effectBox, soundBox, _lastContactPos.x, _lastContactPos.y);
						_destroyed = true;
					}
				}
				else
				{
					//Stunning player + destroying spell + adding effect
					StunPlayer(players[numPlayer]);
					ShowDestructionEffect(effectBox, soundBox, _lastContactPos.x, _lastContactPos.y);
					_destroyed = true;
				}
			}
			else
			{
				//Contact with ground or other projectiles
				ShowDestructionEffect(effectBox, soundBox, _lastContactPos.x, _lastContactPos.y);
				_destroyed = true;

				//Test if body static (helps to show particle effect)
				if (contact->GetFixtureA()->GetFilterData().categoryBits == Box2DTools::GROUND_ENTITY ||
					contact->GetFixtureB()->GetFilterData().categoryBits == Box2DTools::GROUND_ENTITY ||
					contact->GetFixtureA()->GetFilterData().categoryBits == Box2DTools::PLATFORM_ENTITY ||
					contact->GetFixtureB()->GetFilterData().categoryBits == Box2DTools::PLATFORM_ENTITY)
					_destroyedOnGround = true;
			}
			break;
		}
	}
	catch (exception e)
	{
		cout << e.what() << endl;
	}
}

void spellObj::PerformAutoAim(vector<hero*> players, float deltaTime)
{
	try
	{
		Vector2f pos = Vector2f(_body->GetPosition().x*SCALE, _body->GetPosition().y*SCALE);
		Vector2f dir = Vector2f(_body->GetLinearVelocity().x*SCALE, _body->GetLinearVelocity().y*SCALE);
		for (int i = 0; i < players.size(); i++)
		{
			//No need to follow dashing or stunned player, useless
			if (players[i]->IsDashing() || players[i]->IsStunned() || players[i]->GetLife() == 0 ||
				players[i]->GetBodyA() == _body->GetUserData())
				continue;

			//TODO - Choisir aléatoirement joueur a suivre en premier (ici tjr joueur 1)
			Vector2f playerPosA = players[i]->GetPositionA();
			Vector2f playerPosB = players[i]->GetPositionB();
			if (utils::DistanceBetween(pos, playerPosA) <= AIMING_AREA ||
				utils::DistanceBetween(pos, playerPosB) <= AIMING_AREA)
			{
				float distA = utils::DistanceBetween(pos, playerPosA);
				float distB = utils::DistanceBetween(pos, playerPosB);
				Vector2f playerPos = distA < distB ? playerPosA : playerPosB;
				Vector2f dirToPlayer = Vector2f(playerPos.x - pos.x, playerPos.y - pos.y);
				float angleDiff = utils::GetVectorRotation(dirToPlayer, 0) - utils::GetVectorRotation(dir, 0);
				if (fabs(angleDiff) <= 70 && fabs(angleDiff) > 10)
				{
					float angleChange = angleDiff > 0 ? _aimingSpeed : -_aimingSpeed;
					_direction = utils::RotateVector(_direction, angleChange*deltaTime);
					break;
				}
			}
		}
	}
	catch (exception e)
	{
		cout << e.what() << endl;
	}
}

void spellObj::PassThroughPlatforms()
{
	//If going up disable collision with platforms
	int climbUserData = UD_CLIMB;
	if (_body->GetLinearVelocity().y <= 0.f)
	{
		_body->SetUserData((void*)climbUserData);
		return;
	}

	//Testing if on ladder
	CircleShape spellShape(20);
	spellShape.setOrigin(20, 20);
	spellShape.setPosition(_body->GetPosition().x*SCALE, _body->GetPosition().y*SCALE);
	int voidUserData = UD_VOID;
	_body->SetUserData((void*)voidUserData);
	for (b2Body* body = _world->GetBodyList(); body != 0; body = body->GetNext())
	{
		if (body == NULL || body->GetFixtureList()[0].GetFilterData().categoryBits != Box2DTools::LADDER_ENTITY)
			continue;
		Vector2f ladderPos = Vector2f((int)(body->GetPosition().x*SCALE), (int)(body->GetPosition().y*SCALE));
		Vector2f ladderAABB = Box2DTools::GetAABBSize(body, true);
		FloatRect ladderBounds = FloatRect(ladderPos.x - ladderAABB.x / 2.f, ladderPos.y - ladderAABB.y / 2.f, ladderAABB.x, ladderAABB.y);
		if (spellShape.getGlobalBounds().intersects(ladderBounds) && fabs(spellShape.getPosition().x - ladderPos.x) <= 10)
		{
			_body->SetUserData((void*)climbUserData);
			break;
		}
	}
}

void spellObj::Step(effects* effectBox, sound* soundBox, vector<hero*> players, float deltaTime)
{
	if (_destroyed || _lifeTimeout <= 0.f) { return; }

	//Body movement
	if (_body != NULL)
		_body->SetLinearVelocity(b2Vec2(_direction.x, _direction.y));

	//Passing through ladders
	PassThroughPlatforms();

	//Perform contact with game objects + auto aim if close to players
	PerformContacts(effectBox, soundBox, players);
	//if(_body->GetUserData() != "climb")
	PerformAutoAim(players, deltaTime);

	//Converting body pos into pixel units
	float x = _body->GetPosition().x*SCALE;
	float y = _body->GetPosition().y*SCALE;

	//Sprite animation
	_sprite.setPosition(x, y);
	_sprite.setRotation(utils::GetVectorRotation(_direction) - 90.f);
	_animation.PerformAnimation(deltaTime);

	//Light + scale effect
	float speed = fabs(_body->GetLinearVelocity().x) + fabs(_body->GetLinearVelocity().y);
	_sprite.setScale(1.f, 1.f - (speed / 100.f));
	_light.setScale(1.f, 1.f - (speed / 100.f));
	_light.setPosition(x, y);

	//Life timeout
	_lifeTimeout = utils::StepCooldown(_lifeTimeout, 1.f, deltaTime);

	//Destroying
	if (_lifeTimeout <= 0.f)
	{
		effectBox->AddCircleShape(drawOption(
			_sprite.getPosition(),
			Vector2f(17.f, 17.f),
			Vector2f(17.f, 17.f),
			Color(255, 255, 255, _light.getFillColor().a)),
			fadeOption(Vector2f(0.f, 0.f), 15.f, 0.f, 60.f, 0.f, Vector2f(0.025f, 0.025f)),
			physics(false));
		effectBox->AddCircleShape(drawOption(
			_sprite.getPosition(),
			Vector2f(17.f, 17.f),
			Vector2f(17.f, 17.f),
			_light.getFillColor()),
			fadeOption(Vector2f(0.f, 0.f), 5.f, 0.f, 60.f, 0.f, Vector2f(-0.02f, -0.02f)),
			physics(false));
	}

	//Adding light effect
	if (_lightFrame <= 0.f)
	{
		Color lightColor = Color(_light.getFillColor().r, _light.getFillColor().g, _light.getFillColor().b, 150);
		fadeOption fade = fadeOption(Vector2f(0, 0), 16.f, 0.f, -1.f, 1.f, Vector2f(-0.04, -0.04));
		effectBox->AddCircleShape(drawOption(Vector2f(x, y), Vector2f(10, 10), Vector2f(10, 10), lightColor), fade, physics(false));
		_lightFrame = 1.f;
	}
	_lightFrame = utils::StepCooldown(_lightFrame, 1.f, deltaTime);
}

void spellObj::Display(RenderWindow * window)
{
	if (_destroyed || _lifeTimeout <= 0.f) { return; }

	window->draw(_light);
	window->draw(_sprite);
}
