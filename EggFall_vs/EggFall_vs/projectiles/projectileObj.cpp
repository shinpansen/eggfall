#include "projectileObj.hpp"

static const float SCALE = 30.f;
static const float AIMING_AREA = 200.f;
static const float LIFE_TIMEOUT = 300.f;

using namespace std;
using namespace sf;

projectileObj::~projectileObj()
{
	if (_world != NULL && _body != NULL)
		_world->DestroyBody(_body);
	if (_sprite != NULL)
		delete _sprite;
	if (_light != NULL)
		delete _light;
	if (_animation != NULL)
		delete _animation;
}

projectileObj::projectileObj()
{
	//Initialization
	_direction = Vector2f(0, 0);
	_destroyed = false;
	_destroyedOnGround = false;
	_isCounteracted = false;
	_lightFrame = 1.f;
	_lightFrame2 = 10.f;
	_numAnimation = 0;
	_aimingSpeed = 3.f;
	_lifeTimeout = LIFE_TIMEOUT;
	_world = NULL;
	_body = NULL;
	_lastPos = b2Vec2(0, 0);
}

projectileObj::projectileObj(Vector2f pos, Vector2f direction, Texture * texture, int numAnimation, Color color, b2World * world, b2Body * playerOrigin)
{
	//Initialization
	_direction = direction;
	_destroyed = false;
	_destroyedOnGround = false;
	_lightFrame = 1.f;
	_lightFrame2 = 10.f;
	_numAnimation = numAnimation;
	_aimingSpeed = 3.f;
	_lifeTimeout = LIFE_TIMEOUT;
	_world = world;
	_playerOrigin = playerOrigin;
	_lastPos = b2Vec2(0, 0);

	//Sprite - Animation
	float angle = (float)utils::GetVectorRotation(direction) - 90.f;
	_sprite = new Sprite();
	_sprite->setTexture(*texture);
	_sprite->setOrigin(24.f, 24.f);
	_sprite->setRotation(angle);
	_animation = new animation(_sprite, 0.3, 48, 48, 6, numAnimation + 1, -1, true);
	_animation->SetAnimation(numAnimation, 6);

	//CircleShape light
	_light = new CircleShape(24);
	_light->setOrigin(24, 24);
	_light->setFillColor(Color(color.r, color.g, color.b, 60));
	/*_light.setOutlineColor(Color(color.r, color.g, color.b, 50));
	_light.setOutlineThickness(7);*/
	_light->setPosition(pos.x, pos.y);

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

b2Body * projectileObj::GetBody()
{
	return _body;
}

Vector2f projectileObj::GetPosition()
{
	return _body != NULL ? Vector2f(_body->GetPosition().x*SCALE, _body->GetPosition().y*SCALE) : Vector2f();
}

Vector2f projectileObj::GetLastContactPos()
{
	return _lastContactPos;
}

Vector2f projectileObj::GetDirection()
{
	return _direction;
}

float projectileObj::GetSpeed()
{
	return fabs(_direction.x) + fabs(_direction.y);
}

void projectileObj::SetDirection(Vector2f direction)
{
	if (direction != Vector2f())
	{
		_direction = direction;
		_isCounteracted = false;
		_lifeTimeout = LIFE_TIMEOUT;
	}
}

void projectileObj::DestroyProjectile(effects * effectBox, sound* soundBox)
{
	_destroyed = true;
	ShowDestructionEffect(effectBox, soundBox, _lastPos.x*SCALE, _lastPos.y*SCALE);
}

bool projectileObj::GetDestroyedOnGround()
{
	return _destroyedOnGround;
}

bool projectileObj::IsDestroyed()
{
	return _destroyed || _lifeTimeout <= 0.f;
}

bool projectileObj::IsValidContact(b2Contact * contact)
{
	short entities[3] = { Box2DTools::GROUND_ENTITY, Box2DTools::PLATFORM_ENTITY, Box2DTools::BODY_ENTITY };
	for (int i = 0; i < 3; i++)
		if (contact->GetFixtureA()->GetFilterData().categoryBits == entities[i] ||
			contact->GetFixtureB()->GetFilterData().categoryBits == entities[i])
			return true;
	return false;
}

bool projectileObj::IsAPlayer(b2Body* body, vector<hero*> players, int* numPlayer)
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

void projectileObj::StunPlayer(hero * player)
{
	//Calculating stun direction
	double speedx = _body->GetLinearVelocity().x != 0 ? (_body->GetLinearVelocity().x / fabs(_body->GetLinearVelocity().x)) * 10 : 0;
	double speedy = _body->GetLinearVelocity().y != 0 ? (_body->GetLinearVelocity().y / fabs(_body->GetLinearVelocity().y)) * 6 : 0;
	speedy = fabs(speedy) <= 0.1 ? -2 : speedy;

	//Stunning player + destroying spell + adding effect
	player->SetStunned(Vector2f(speedx, speedy), _lastContactPos, 15, _playerOrigin);
}

void projectileObj::ShowDestructionEffect(effects * effectBox, sound * soundBox, float x, float y)
{
	Vector2f sfxPos = Vector2f(x, y);
	if (_body != NULL && (fabs(x - _body->GetPosition().x*SCALE) > 50.f || fabs(y - _body->GetPosition().y*SCALE) > 50.f))
		sfxPos = Vector2f(_body->GetPosition().y * SCALE, _body->GetPosition().y * SCALE);
	effectBox->ShakeScreen(4, 2);
	effectBox->AddAnimation(T_SPELLS_EXPLOSION, _numAnimation * 2, 0.3, false, drawOption(sfxPos), fadeOption(0), physics(false));
	soundBox->PlaySound(SOUND::SND_EXPLOSION_A, false);
}

void projectileObj::PerformContacts(effects * effectBox, sound* soundBox, vector<hero*> players)
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
			if (!_isCounteracted && (IsAPlayer(contact->GetFixtureA()->GetBody(), players, &numPlayer) ||
				IsAPlayer(contact->GetFixtureB()->GetBody(), players, &numPlayer)))
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
						players[numPlayer]->HasCounteracted(this);
						_isCounteracted = true;
						break;
						/*b2Vec2 speed = _body->GetLinearVelocity();
						_direction = Vector2f(-speed.x*1.3f, -speed.y*1.3f);
						_aimingSpeed *= 1.2f;
						_lifeTimeout = LIFE_TIMEOUT;
						_body->SetLinearVelocity(b2Vec2(_direction.x, _direction.y));

						//Adding effect
						soundBox->PlaySound(SOUND::SND_COUNTERING, false);
						Sprite* effect = effectBox->AddAnimation(T_SPELLS_EXPLOSION, _numAnimation * 2 + 1, 0.3, false, drawOption(Vector2f(75, 75)), fadeOption(0), physics(false));
						effect->setRotation(utils::GetVectorRotation(_direction) + 90.f);
						effect->setPosition(Vector2f(_lastContactPos.x, _lastContactPos.y));*/
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

void projectileObj::PerformAutoAim(vector<hero*> players, float deltaTime)
{
	try
	{
		if (_isCounteracted)
			return;

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

void projectileObj::PassThroughPlatforms()
{
	if (_isCounteracted)
		return;

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

void projectileObj::Step(effects* effectBox, sound* soundBox, vector<hero*> players, float deltaTime)
{
	if (_destroyed || _lifeTimeout <= 0.f || _body == NULL)
	{
		_destroyed = true;
		return;
	}

	//Last position
	if (!_isCounteracted)
		_lastPos = _body->GetPosition();

	//Body movement
	if (!_isCounteracted)
		_body->SetLinearVelocity(b2Vec2(_direction.x, _direction.y));
	else
		_body->SetTransform(_lastPos, 0);

	//Passing through ladders
	PassThroughPlatforms();

	//Perform contact with game objects + auto aim if close to players
	PerformContacts(effectBox, soundBox, players);
	PerformAutoAim(players, deltaTime);

	//Converting body pos into pixel units
	float x = _body->GetPosition().x*SCALE;
	float y = _body->GetPosition().y*SCALE;

	//Sprite animation
	_sprite->setPosition(x, y);
	_sprite->setRotation(utils::GetVectorRotation(_direction) - 90.f);
	_animation->PerformAnimation(deltaTime);

	//Light + scale effect
	float speed = fabs(_body->GetLinearVelocity().x) + fabs(_body->GetLinearVelocity().y);
	float scaleY = 1.f - (speed / 100.f) >= 0.42f ? 1.f - (speed / 100.f) : 0.42f;
	_sprite->setScale(1.f, scaleY);
	_light->setScale(1.f, scaleY);
	_light->setPosition(x, y);

	//Life timeout
	if (!_isCounteracted)
	{
		//Destroying
		_lifeTimeout = utils::StepCooldown(_lifeTimeout, 1.f, deltaTime);
		if (_lifeTimeout <= 0.f)
		{
			effectBox->AddAnimation(TEXTURE_ID::T_SPELLS, _numAnimation + 5, 0.2, false,
				drawOption(_sprite->getPosition() + _direction, Vector2f(24, 24), _sprite->getRotation()),
				fadeOption(0, 100), physics(false));
		}
	}

	//Adding light effect
	if (_lightFrame <= 0.f && !_isCounteracted)
	{
		Color lightColor = Color(_light->getFillColor().r, _light->getFillColor().g, _light->getFillColor().b, 120);
		float scaleStep = (float)utils::RandomNumber(8, 4);
		fadeOption fade = fadeOption(Vector2f(0, 0), utils::RandomNumber(15, 5), 0.f, -1.f, 1.f, Vector2f(-scaleStep / 100.f, -scaleStep / 100.f));// -0.04, -0.04));
		effectBox->AddCircleShape(drawOption(Vector2f(x, y), Vector2f(10, 10), Vector2f(10, 10), lightColor), fade, physics(false));
		_lightFrame = 1.f;
	}
	if (_lightFrame2 <= 0.f && !_isCounteracted && GetSpeed() >= 30.f)
	{
		_lightFrame2 = 45.f - GetSpeed() >= 5.f ? 45.f - GetSpeed() : 5.f;
		effectBox->AddAnimation(TEXTURE_ID::T_SPELLS, _numAnimation + 5, 0.2f, false,
			drawOption(_sprite->getPosition() + _direction, Vector2f(24, 24), _sprite->getRotation()),
			fadeOption(0, 100), physics(false));
	}
	_lightFrame = utils::StepCooldown(_lightFrame, 1.f, deltaTime);
	_lightFrame2 = utils::StepCooldown(_lightFrame2, 1.f, deltaTime);
}

void projectileObj::Display(RenderWindow * window)
{
	if (_destroyed || _lifeTimeout <= 0.f) { return; }

	if(_light != NULL)
		window->draw(*_light);
	if(_sprite != NULL)
		window->draw(*_sprite);
}
