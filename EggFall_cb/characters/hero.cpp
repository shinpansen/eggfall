#include "hero.hpp"

static const float SCALE = 30.f;
static const float WALKING_SPEED = 10.f;
static const float RUNNING_SPEED = 15.f;
static const float FALLING_SPEED = 20.f;
static const float DASHING_SPEED = 25.f;

using namespace std;
using namespace sf;

hero::hero()
{
}

hero::~hero()
{
	if (_body != NULL && _bodyClone != NULL)
	{
		_world->DestroyBody(_body);
		_world->DestroyBody(_bodyClone);
	}
}

hero::hero(int colorID, int life, Vector2f pos, b2World* world, input* input, effects* effectBox,
	       projectiles* projectileBox, sound* soundBox, scoreManager* scoreManager)
{
    //Variable
    _onTheFloor = false;
    _lastOnTheFloor = false;
    _hasJumped = false;
    _hasCounteract = false;
	_hasDash = false;
    _hasAttacked = false;
	_isAiming = false;
	_cancelJumpFallScale = false;
	_dashStep = false;
	_crouch = false;
	_cloneVisible = false;
	_crushAttackAnimation = true;
	_stuck = false;
	_drawDust = true;
	_lastReversed = false;
	_colorID = colorID;
    _life = life;
	_maxLife = life;
	_hooked = 0;
	_attackingAnimation = 0;
	_effectAnglePosition = 0;
	_collisionType = -1;
    _canWallJump = 0.f;
	_jumpTolerance = 0.f;
	_attacking = 0.f;
	_attackingCooldown = 0.f;
    _counteract = 0.f;
    _counteractCooldown = 0.f;
    _hasCounteractedCooldown = 0.f;
	_dash = 0.f;
	_dashCooldown = 0.f;
	_canClimb = 0.f;
	_stunned = 0.f;
    _dustFrame = 0.f;
	_dustCooldown = 0.f;
	_invincibilityFrame = 0.f;
	_ladderHeight = 0.f;
	_flashFrame = 0.f;
	_hasJumpedCooldown = 0.f;
	_spellAngle = 0.f;
	_selfProjectileInvincible = 0.f;
	_lastProjectile = NULL;
	_lastFallingSpeed = 0.f;
	_updateFallingSpeedFrame = 0.f;
	_aimingSpeedCooldown = 0.f;
	_platformFall = 0.f;
    _deltaTime = 1.f;

    //Vectors
    _initPos = pos;
    _speed = Vector2f(0.f, 0.f); //In Box2D unit
    _maxSpeed = Vector2f(RUNNING_SPEED, FALLING_SPEED);
    _position = Vector2f(pos.x, pos.y); //In SFML pixel unit
    _positionClone = Vector2f(pos.x, pos.y); //In SFML pixel unit
	_dashDirection = Vector2f(0, 0);
    _hookedPoint = Vector2f(0,0);
    _ladderPos = Vector2f(0,0);
    _stunDirection = Vector2f(0,0);
	_spellSpeed = Vector2f(0, 0);
	_spellPos = Vector2f(0, 0);

    //Objects
    _input = input;
	_effectBox = effectBox;
	_projectileBox = projectileBox;
    _soundBox = soundBox;
    _wallSlidingSound = NULL;
    _scoreManager = scoreManager;

    //Box2D
    _world = world;
    _body = Box2DTools::CreateRectangleBox(_world, pos, 20, 35, 0.1, 0, 0, Box2DTools::BODY_ENTITY);
    _bodyClone = Box2DTools::CreateRectangleBox(_world, pos, 20, 35, 0.1, 0, 0, Box2DTools::BODY_ENTITY);
    _bodyClone->SetActive(false);
    int voidUserData = UD_VOID;
	_body->SetUserData((void*)voidUserData);
	_bodyClone->SetUserData((void*)voidUserData);

	//Hidden method - Load hero texture
	LoadHeroTexture();

    //Sprites
    if (!_lifeTexture.loadFromFile(files::life))
    {
        cout << "Unable to find life texture. Stopping execution." << endl;
        exit(1);
    }
	else if (!_aimArrowTexture.loadFromFile(files::aimArrow))
	{
		cout << "Unable to find aim arrow texture. Stopping execution." << endl;
		exit(1);
	}
    else
        ResetLifeSprite();

	//Aim arrow sprite - circle
	_aimArrow.setTexture(_aimArrowTexture);
	_aimArrow.setOrigin(_aimArrow.getLocalBounds().width / 2.f, _aimArrow.getLocalBounds().height / 2.f);
}

void hero::LoadHeroTexture() {} //Override in child class

void hero::Update()
{
    if(_life == 0 && _stunned == 0.f) //Waiting to respawn hero
    {
        if(_invincibilityFrame > 0)
        {
			_invincibilityFrame -= _deltaTime;
            return;
        }
        else //Respawn
        {
            _life = _maxLife;
            _invincibilityFrame = 60.f;
			_animation.ReverseSprite(_initPos.x > 450);
			_canClimb = 0.f;
			if (_body != NULL)
			{
				_body->SetActive(true);
				_body->SetTransform(b2Vec2(_initPos.x / SCALE, _initPos.y / SCALE), 0);
			}
        }
    }

    //Body speed + position
    _speed = Vector2f(_body->GetLinearVelocity().x, _body->GetLinearVelocity().y);
    _position = Vector2f(_body->GetPosition().x*SCALE, _body->GetPosition().y*SCALE);
    _positionClone = Vector2f(_bodyClone->GetPosition().x*SCALE, _bodyClone->GetPosition().y*SCALE);

	//Collecting last falling speed (useful for crushing animation when landing)
	if (_speed.y != 0.f)
		_updateFallingSpeedFrame = 10.f;
	if(_onTheFloor)
		_updateFallingSpeedFrame = utils::StepCooldown(_updateFallingSpeedFrame, 1.f, _deltaTime);
	if (_speed.y >= _lastFallingSpeed || (_speed.y < _lastFallingSpeed && _updateFallingSpeedFrame == 0.f))
		_lastFallingSpeed = _speed.y;

	//BUG - Correcting Box2D bodies being randomly altered after loading parameters
	if ((int)Box2DTools::GetAABBSize(_body, true).x > 20)
		ResetBody(20, 35);

    //Testing if body on the floor
    _lastOnTheFloor = _onTheFloor;
	_onTheFloor = false;
	if (_platformFall == 0.f)
	{
		float posxToTest[3] = { _position.x, _position.x - 10.3f, _position.x + 10.3f };
		for (int i = 0; i < 3; i++)
		{
			b2Body* body;
			if (Box2DTools::TestCollisionPoint(_world, b2Vec2(posxToTest[i] / SCALE, (_position.y + (_crouch ? 18 : 22)) / SCALE), true,
				&_collisionType, &body) && ((body->GetAngle() == 0.f && _speed.y >= -0.1f) || (body->GetAngle() != 0.f && _speed.y >= -8.f)))
			{
				if (_collisionType == Box2DTools::GROUND_ENTITY || (_canClimb == 0.f && _collisionType == Box2DTools::PLATFORM_ENTITY))
				{
					_drawDust = i == 0 || _collisionType == Box2DTools::PLATFORM_ENTITY;
					_onTheFloor = true;
					_canWallJump = 0.f;
					if (_dashDirection.y > 0.f && _dash > 0.f && _collisionType != Box2DTools::PLATFORM_ENTITY)
					{
						_body->SetLinearVelocity(b2Vec2(0, _speed.y));
						_dash = 0.f;
					}
					break;
				}
				//Bounce on other wizards' head
				else if (_dash == 0.f &&_body != NULL && _bodyClone != NULL && body != _body && body != _bodyClone &&
					IsAPlayer(body) >= 0 && (int)body->GetUserData() != UD_DASH)
				{
					_soundBox->PlaySound(SOUND::SND_FALLING, false);
					_players[IsAPlayer(body)]->CrushHero();
					_body->SetLinearVelocity(b2Vec2(_speed.x, -15));
					_cancelJumpFallScale = true;
					_speed.y = -10;
					break;
				}
			}
		}
	}
    if(!_onTheFloor || _platformFall > 0.f)
        _collisionType = -1;

	//Crush head on the ceiling
	short collisionTest;
	bool lastHeadOnTheCeiling = _headOnTheCeiling;
	_headOnTheCeiling = Box2DTools::TestCollisionPoint(_world, b2Vec2(_position.x / SCALE, (_position.y - 25) / SCALE), false, &collisionTest);
	if (!lastHeadOnTheCeiling && _headOnTheCeiling && _dash == 0.f)
	{
		_lastFallingSpeed = fabs(_speed.y)/1.5f;
		_animation.PerformFallingScale(1.f - (_lastFallingSpeed*3.f) / 100.f);
	}

    //Jump tolerance - Permit the hero to jump during some frames (60fps based) after leaving the floor
	if (_onTheFloor)
		_jumpTolerance = 8.f;
	else if (_jumpTolerance > 0.f)
		_jumpTolerance = utils::StepCooldown(_jumpTolerance, 1.f, _deltaTime);

    //Dust of moving
	float dustFrameStep = _canWallJump != 0.f ? fabs(_speed.y) / 3.f : fabs(_speed.x) / 3.f;
	_dustFrame = utils::StepCooldown(_dustFrame, dustFrameStep, _deltaTime, 3.f);

	//Wall jump duration
	if(_position.y < utils::VIEW_HEIGHT && _positionClone.y < utils::VIEW_HEIGHT)
		_canWallJump = utils::StepCooldown(_canWallJump, 1.f, _deltaTime);

	//Testing if body can wall jump
    if(!_onTheFloor && _hooked == 0 && _canClimb == 0.f && _input->isKeyPressed(CMD_LEFT) &&
       Box2DTools::TestCollisionPoint(_world, b2Vec2((_position.x-11)/SCALE, (_position.y-14)/SCALE), false, &_collisionType) &&
       Box2DTools::TestCollisionPoint(_world, b2Vec2((_position.x-11)/SCALE, (_position.y+14)/SCALE), false, &_collisionType))
       _canWallJump = 6.f;
    else if(!_onTheFloor && _hooked == 0 && _canClimb == 0.f && _input->isKeyPressed(CMD_RIGHT) &&
       Box2DTools::TestCollisionPoint(_world, b2Vec2((_position.x+11)/SCALE, (_position.y-14)/SCALE), false, &_collisionType) &&
       Box2DTools::TestCollisionPoint(_world, b2Vec2((_position.x+11)/SCALE, (_position.y+14)/SCALE), false, &_collisionType))
       _canWallJump = -6.f;

    //Testing if body can hook to a wall
	int lastHooked = _hooked;
    if(_body != NULL && !_onTheFloor && !_isAiming && _speed.y >= -0.1 && _canClimb < 10.f)
    {
		//Searching for hook point
		if (_hooked == 0)
		{
			b2Vec2 posLeft = b2Vec2(_body->GetPosition().x - (11.f / SCALE), _body->GetPosition().y - (14.f / SCALE));
			b2Vec2 posRight = b2Vec2(_body->GetPosition().x + (11.f / SCALE), _body->GetPosition().y - (14.f / SCALE));
			_hooked = Box2DTools::TestHookPoint(_world, posLeft, &_hookedPoint, _animation.GetReversed());
			if (_hooked == 0)
				_hooked = Box2DTools::TestHookPoint(_world, posRight, &_hookedPoint, _animation.GetReversed());
		}

		//Cancel hook if too close from ground
		if (Box2DTools::TestCollisionPoint(_world, b2Vec2(_position.x / SCALE, (_position.y + 33) / SCALE), true, &_collisionType))
			_hooked = 0.f;

        //Stay hooked
		if (_hooked != 0 && _speed.y > 0)
		{
			_canWallJump = 0.f;
			_body->SetLinearVelocity(b2Vec2(0, 0));
			_body->SetTransform(b2Vec2(_hookedPoint.x + ((_hooked > 0 ? 11 : -11) / SCALE), _hookedPoint.y + (14 / SCALE)), 0);
			_speed = Vector2f(_body->GetLinearVelocity().x, _body->GetLinearVelocity().y);
			_body->ApplyForce(b2Vec2(0.f, -42 * _body->GetMass()), _body->GetWorldCenter(), true);
		}
    }
	//Flash hero if hook
	if (lastHooked != _hooked)
	{
		_flashFrame = 5.f;
		_cancelJumpFallScale = true;
	}

    //Testing if hero can climb to a ladder
	float lastCanClimb = _canClimb;
    if(_canWallJump == 0.f && _speed.y > -10.f && !_isAiming && _attacking == 0.f && _dash == 0.f &&
      (!_onTheFloor || (_onTheFloor && fabs(_speed.x)*SCALE < WALKING_SPEED)))
    {
        if(_canClimb >= 10.f || (_canClimb < 10.f && (_input->isKeyPressed(CMD_UP) || _input->isKeyPressed(CMD_DOWN))))
        {
            RectangleShape heroShape(Vector2f(20,35));
			heroShape.setOrigin(10,18);
			heroShape.setPosition(_position);
			bool onLadder = false;
			for (b2Body* body = _world->GetBodyList(); body != 0; body = body->GetNext())
			{
				if (body == NULL || body->GetFixtureList()[0].GetFilterData().categoryBits != Box2DTools::LADDER_ENTITY)
					continue;
				Vector2f ladderPos = Vector2f((int)(body->GetPosition().x*SCALE), (int)(body->GetPosition().y*SCALE));
				Vector2f ladderAABB = Box2DTools::GetAABBSize(body, true);
				FloatRect ladderBounds = FloatRect(ladderPos.x - ladderAABB.x / 2.f, ladderPos.y - ladderAABB.y / 2.f, ladderAABB.x, ladderAABB.y);
				if (heroShape.getGlobalBounds().intersects(ladderBounds) && _position.y - 15.f >= ladderPos.y - (ladderAABB.y / 2.f))
				{
					onLadder = true;
					_canClimb = 10.f;
					_hooked = 0.f;
					_ladderPos = ladderPos;
					_ladderHeight = ladderAABB.y + 10.f;
					break;
				}
			}
			if(!onLadder)
				_canClimb = utils::StepCooldown(_canClimb, 1.f, _deltaTime);
        }
		else if (_canClimb > 0.f)
			_canClimb = utils::StepCooldown(_canClimb, 1.f, _deltaTime);
    }
	else if (_canClimb > 0.f)
		_canClimb = utils::StepCooldown(_canClimb, 1.f, _deltaTime);

	//If can climb changing body user data to disable collisions with ground if necessary
	if (_canClimb == 10.f || _platformFall > 0.f)
	{
	    int climbUserData = UD_CLIMB;
		_body->SetUserData((void*)climbUserData);
		_bodyClone->SetUserData((void*)climbUserData);
	}
	else if (_dash == 0.f)
	{
	    int voidUserData = UD_VOID;
		_body->SetUserData((void*)voidUserData);
		_bodyClone->SetUserData((void*)voidUserData);
	}
	//Positioning hero on ladder smoothly
	if (_canClimb >= 10.f)
	{
		if (_position.x != _ladderPos.x)
		{
			float gapX = fabs(_ladderPos.x - _position.x) / 3.f*_deltaTime;
			float newPosX = _position.x > _ladderPos.x ? _position.x - gapX : _position.x + gapX;
			if (_body != NULL)
				_body->SetTransform(b2Vec2(newPosX / SCALE, _position.y / SCALE), 0);
		}
	}

	//Turning hero after leaving ladder from bottom
	if (lastCanClimb == 0.f && _canClimb != 0.f)
		_lastReversed = _animation.GetReversed();
	else if (lastCanClimb == 10.f && _canClimb < 10.f)
		_animation.ReverseSprite(_lastReversed);

	//Testing if hero can stand up after crouch
	_stuck = false;
	if (_crouch)
	{
		b2Vec2 posTop = _body->GetPosition();
		posTop.y -= 35.f / SCALE;
		short collisionTrash;
		if (Box2DTools::TestCollisionPoint(_world, b2Vec2(posTop.x - (14.f / SCALE), posTop.y), false, &collisionTrash) ||
			Box2DTools::TestCollisionPoint(_world, b2Vec2(posTop.x + (14.f / SCALE), posTop.y), false, &collisionTrash))
			_stuck = true;
	}

    //Counteract - dash duration / cooldown
	float lastDash = _dash;
	_counteract = utils::StepCooldown(_counteract, 1.f, _deltaTime);
	_counteractCooldown = utils::StepCooldown(_counteractCooldown, 1.f, _deltaTime);
	_hasCounteractedCooldown = utils::StepCooldown(_hasCounteractedCooldown, 1.f, _deltaTime);
	_dash = utils::StepCooldown(_dash, 1.f, _deltaTime);
	_dashCooldown = utils::StepCooldown(_dashCooldown, 1.f, _deltaTime);
	_dashStep = _dash >= 15.f && (int)((lastDash+0.5f)*10.f) != (int)((_dash+0.5f)*10.f);
	_flashFrame = utils::StepCooldown(_flashFrame, 1.f, _deltaTime);
	_selfProjectileInvincible = utils::StepCooldown(_selfProjectileInvincible, 1.f, _deltaTime);
	_aimingSpeedCooldown = utils::StepCooldown(_aimingSpeedCooldown, 1.f, _deltaTime);
	_platformFall = utils::StepCooldown(_platformFall, 1.f, _deltaTime);
	_dustCooldown = utils::StepCooldown(_dustCooldown, 1.f, _deltaTime);

    //Changing hero collision mask after countering duration is over
    if(_body != NULL && fabs(_counteract) == 10)
    {
        _world->DestroyBody(_body);
        _body = Box2DTools::CreateRectangleBox(_world, Vector2f(0,0), 20, 35, 0.1, 0, 0, Box2DTools::BODY_ENTITY);
        _body->SetTransform(b2Vec2(_position.x/SCALE, _position.y/SCALE), 0);
    }

    //Attack duration / cooldown
	_attacking = utils::StepCooldown(_attacking, 1.f, _deltaTime);
	_attackingCooldown = utils::StepCooldown(_attackingCooldown, 1.f, _deltaTime);

    //Direction if stunned
    if(_stunned > 0)
    {
        if((_hooked != 0 ||_canClimb != 0.f) && fabs(_stunDirection.y) <= 2)
        {
            _stunDirection.x = 0.f;
            _stunDirection.y = 10.f;
			if (_body != NULL)
				_body->ApplyForce(b2Vec2(0.f,42*_body->GetMass()), _body->GetWorldCenter(), true);
        }
        _hooked = 0;
        _canWallJump = 0.f;
		_canClimb = 0.f;
		if (_body != NULL)
			_body->SetLinearVelocity(b2Vec2(_stunDirection.x, _stunDirection.y));
    }

    //Stun + Invincibility duration
	_stunned = utils::StepCooldown(_stunned, 1.f, _deltaTime);
	_invincibilityFrame = utils::StepCooldown(_invincibilityFrame, 1.f, _deltaTime);

    //Limiting running speed
    if(_body != NULL && _body->GetLinearVelocity().x > _maxSpeed.x)
        _body->SetLinearVelocity(b2Vec2(_body->GetLinearVelocity().x-1.f*_deltaTime, _body->GetLinearVelocity().y));
    else if(_body != NULL &&_body->GetLinearVelocity().x < -_maxSpeed.x)
        _body->SetLinearVelocity(b2Vec2(_body->GetLinearVelocity().x+1.f*_deltaTime, _body->GetLinearVelocity().y));

    //Limiting falling speed
    if(_body != NULL && _body->GetLinearVelocity().y > _maxSpeed.y)
        _body->SetLinearVelocity(b2Vec2(_body->GetLinearVelocity().x, _maxSpeed.y));

    //Body speed + position (re-update)
	if (_body != NULL && _body->IsActive())
	{
		_speed = Vector2f(_body->GetLinearVelocity().x, _body->GetLinearVelocity().y);
		_position = Vector2f(_body->GetPosition().x*SCALE, _body->GetPosition().y*SCALE);
	}

    if(_life == 0 && _stunned <= 1.f) //Killing hero if running out of life
    {
		Vector2f animPos = Vector2f(_position.x + 4.f, _position.y - 18.f);
		_effectBox->AddAnimation(T_DEAD, 0, 0.2, _animation.GetReversed(), drawOption(animPos), fadeOption(0), physics(false));
        _soundBox->PlaySound(SOUND::SND_DYING, false);
        _stunned = 0.f;
        _stunDirection = Vector2f(0,0);
		if (_body != NULL && _bodyClone != NULL)
		{
			_body->SetLinearVelocity(b2Vec2(0, 0));
			_body->SetActive(false);
			_bodyClone->SetActive(false);
		}
    }
}

void hero::PerformEffects()
{
    //Animation of falling - crush sprite + dust
    if(!_lastOnTheFloor && _onTheFloor && _dash == 0.f && _attacking == 0.f &&
	  (_collisionType == Box2DTools::GROUND_ENTITY || _collisionType == Box2DTools::PLATFORM_ENTITY))
    {
		_animation.PerformFallingScale(1.f - (_lastFallingSpeed*3.f) / 100.f);
		if (_dustCooldown == 0.f)
		{
			_soundBox->PlaySound(SOUND::SND_FALLING, false);
			DrawEffect(DUST_LANDING);
			_dustCooldown = 25.f;
		}
    }

    //Dust moving
    if(fabs(_speed.x) > 2.f && _onTheFloor && _canWallJump == 0.f && fabs(_speed.y) < 0.1f && _dash == 0.f &&
	   _dustFrame >= 3.f && (_collisionType == Box2DTools::GROUND_ENTITY || _collisionType == Box2DTools::PLATFORM_ENTITY))
    {
        int pos = _speed.x > 0 ? -10 : 10;
		_dustFrame = 0.f;
        _effectBox->AddDust(Vector2f(_position.x+pos, _position.y + (_crouch ? 9.5f : 15.f)), _speed.x > 0.f, true);
    }

    //Dust wall sliding
    if(_canWallJump != 0.f && !_onTheFloor && _speed.y > 1.f && _dustFrame >= 3.f && _dash == 0.f)
    {
        int pos = _canWallJump > 0.f ? -10 : 10;
		_dustFrame = 0.f;
		DrawEffect(DUST_WALLJUMP, pos);
    }

    //Wall sliding sound
    if(_canWallJump != 0.f && _wallSlidingSound == NULL)
        _wallSlidingSound = _soundBox->PlaySound(SOUND::SND_WALLSLIDING, true);
    else if(_canWallJump == 0.f && _wallSlidingSound != NULL)
    {
        _wallSlidingSound->stop();
        _wallSlidingSound = NULL;
    }

    //Clone of the sprite, if loop horizontal
    if(_position.x-40 <= 0)
    {
        _bodyClone->SetActive(true);
        _bodyClone->SetTransform(b2Vec2((utils::VIEW_WIDTH+_position.x)/SCALE, _position.y/SCALE),0);
    }
    else if(_position.x+40 >= utils::VIEW_WIDTH)
    {
        _bodyClone->SetActive(true);
        _bodyClone->SetTransform(b2Vec2((_position.x- utils::VIEW_WIDTH)/SCALE, _position.y/SCALE),0);
    }
    if(_position.y-40 <= 0)
    {
        _bodyClone->SetActive(true);
        _bodyClone->SetTransform(b2Vec2(_position.x/SCALE, (_position.y+utils::VIEW_HEIGHT)/SCALE),0);
    }
    else if(_position.y+40 >= utils::VIEW_HEIGHT)
    {
        _bodyClone->SetActive(true);
        _bodyClone->SetTransform(b2Vec2(_position.x/SCALE, (_position.y-utils::VIEW_HEIGHT)/SCALE),0);
    }

    //Disabling collision of clone
    if(_position.x-40 > 0 && _position.x+40 < utils::VIEW_WIDTH && _position.y-40 > 0 && _position.y+40 < utils::VIEW_HEIGHT)
    {
        _bodyClone->SetTransform(b2Vec2(0,0),0);
        _bodyClone->SetActive(false);
    }

	//Cancel scale animation (jump / fall)
	if (_canWallJump != 0.f || _cancelJumpFallScale)
	{
		_animation.CancelJumpFallScale();
		_cancelJumpFallScale = false;
	}
}

void hero::Move()
{
	//Inertia parameters
	float deltaAdjust = _deltaTime;
    float scalingSpeed = (!_onTheFloor || _counteract != 0.f || _canClimb > 0.f) ? 1.f*deltaAdjust : 1.5*deltaAdjust;
    float scalingBrake = _counteract != 0.f || _attacking != 0.f ? 0.5*deltaAdjust: 1.f*deltaAdjust;

    //Running
	if ((_input->isKeyPressed(CMD_RUN) || _counteract != 0.f) && !_crouch)
	{
		if (_maxSpeed.x < RUNNING_SPEED && _hooked == 0.f && !_canWallJump && !_canClimb)
			_flashFrame = 5.f; //Flashing hero when starting running
		_maxSpeed.x = RUNNING_SPEED;
	}
    else if(_onTheFloor || _maxSpeed.x < WALKING_SPEED || _canClimb > 0.f)
        _maxSpeed.x = _crouch ? WALKING_SPEED/4.f : WALKING_SPEED * fabs(_input->GetStickDeltaX());

    //Counteracting
    if(_body != NULL && _input->isKeyPressed(CMD_COUNTERACT) && !_isAiming && _dash == 0.f && !_stuck &&
	   _attacking == 0.f && _counteract == 0.f && _counteractCooldown == 0.f && !_hasCounteract && _canClimb < 10.f)
    {
        //Changing body collision mask
        _world->DestroyBody(_body);
        _body = Box2DTools::CreateRectangleBox(_world, Vector2f(0,0), 32, 35, 0.1, 0, 0, Box2DTools::BODY_ENTITY);
        _body->SetTransform(b2Vec2(_position.x/SCALE, _position.y/SCALE), 0);

        int signe = _animation.GetReversed() ? -1 : 1;
		_counteract = (float)signe * 22.f;
		_counteractCooldown = (float)signe * 50.f;
        if(_speed.y >= 0)
        {
            _body->SetLinearVelocity(b2Vec2(_speed.x, 0));
            _speed.y = 0;
        }
        _soundBox->PlaySound(SOUND::SND_ROLLING, false);
    }

    //Locking counteract key if already pressed once
    if(_input->isKeyPressed(CMD_COUNTERACT))
        _hasCounteract = true;
    else
        _hasCounteract = false;

    //Attacking
	if (PerformAttack()) //Override in child class
		return;

    //Unhooking
    if((_hooked != 0 && _input->isKeyPressed(CMD_DOWN)) ||
       (_hooked < 0 && _input->isKeyPressed(CMD_LEFT)) ||
       (_hooked > 0 && _input->isKeyPressed(CMD_RIGHT)))
    {
        _hooked = 0;
		if (_body != NULL)
		{
			_body->ApplyForce(b2Vec2(0.f, 42 * _body->GetMass()), _body->GetWorldCenter(), true);
			_body->SetLinearVelocity(b2Vec2(_speed.x, 5));
		}
    }

    //Moving
	if (_body != NULL && _canClimb > 0.f && (_input->isKeyPressed(CMD_LEFT) || _input->isKeyPressed(CMD_RIGHT)))
	{
		_body->ApplyForce(b2Vec2(0.f, 0 * _body->GetMass()), _body->GetWorldCenter(), true);
		_canClimb = utils::StepCooldown(_canClimb, 1.f, _deltaTime);
	}
	if (_body != NULL && _dash == 0.f && _hooked == 0.f)
	{
		if (_input->isKeyPressed(CMD_LEFT) && !_isAiming && _speed.x - scalingSpeed > -_maxSpeed.x && _counteract == 0.f && _attacking == 0.f)
			_body->SetLinearVelocity(b2Vec2(_speed.x - scalingSpeed, _speed.y));
		else if (_input->isKeyPressed(CMD_RIGHT) && !_isAiming && _speed.x + scalingSpeed < _maxSpeed.x && _counteract == 0.f && _attacking == 0.f)
			_body->SetLinearVelocity(b2Vec2(_speed.x + scalingSpeed, _speed.y));
		else if ((!_input->isKeyPressed(CMD_LEFT) || _counteract != 0.f || _attacking != 0.f || _isAiming) && _speed.x <= -scalingBrake)
			_body->SetLinearVelocity(b2Vec2(_speed.x + scalingBrake, _speed.y));
		else if ((!_input->isKeyPressed(CMD_RIGHT) || _counteract != 0.f || _attacking != 0.f || _isAiming) && _speed.x >= scalingBrake)
			_body->SetLinearVelocity(b2Vec2(_speed.x - scalingBrake, _speed.y));
		else if ((!_input->isKeyPressed(CMD_LEFT) || !_input->isKeyPressed(CMD_RIGHT) || _counteract != 0.f || _attacking != 0.f || _isAiming) && fabs(_speed.x) < scalingBrake)
			_body->SetLinearVelocity(b2Vec2(0, _speed.y));
	}

    //Jumping / Wall Jump
    if(!_hasJumped && _counteract == 0.f && !_isAiming && _dash == 0.f && !_crouch && _body != NULL)
    {
        //Jumping
        if((_onTheFloor || _canClimb > 0.f || _jumpTolerance > 0.f) && _input->isKeyPressed(CMD_JUMP))
        {
			//Dust of jumping
			_hasJumped = true;
			_body->ApplyForce(b2Vec2(0.f, 0 * _body->GetMass()), _body->GetWorldCenter(), true);
			_body->SetLinearVelocity(b2Vec2(_speed.x, -18 - fabs(_speed.x / 5)));
			_jumpTolerance = 0.f;
			_hooked = 0.f;
            _soundBox->PlaySound(SOUND::SND_JUMP, false);
			_animation.PerformJumpingScale();
			if(_canClimb == 0.f)
				DrawEffect(DUST_LANDING);
			_canClimb = 0.f;
        }
        else if(!_onTheFloor && !_input->isKeyPressed(CMD_JUMP) && _speed.y < -7)
            _body->SetLinearVelocity(b2Vec2(_speed.x, -7));
        else if(_hooked != 0 && _input->isKeyPressed(CMD_JUMP))
        {
			_hasJumped = true;
			_body->ApplyForce(b2Vec2(0.f, 0 * _body->GetMass()), _body->GetWorldCenter(), true);
			_body->SetLinearVelocity(b2Vec2(_speed.x, -16));
			_hooked = 0;
			_jumpTolerance = 0.f;
            _soundBox->PlaySound(SOUND::SND_JUMP, false);
        }

        //Wall jump
        if(_canWallJump != 0.f && _input->isKeyPressed(CMD_JUMP))
        {
            if(((_input->isKeyPressed(CMD_LEFT) && _canWallJump > 0.f) ||
               (_input->isKeyPressed(CMD_RIGHT) && _canWallJump < 0.f)))
                _body->SetLinearVelocity(b2Vec2(_canWallJump > 0.f ? 12.f : -12.f, -16.f));
            else
            {
                _maxSpeed = Vector2f(WALKING_SPEED, FALLING_SPEED);
				_body->SetLinearVelocity(b2Vec2(_canWallJump > 0.f ? 20.f : -20.f, -14.f));
            }
            _soundBox->PlaySound(SOUND::SND_JUMP, false);
            _canWallJump = 0.f;
			_jumpTolerance = 0.f;
			_hooked = 0.f;
			_hasJumped = true;
        }
    }

	//Locking jumping key if already pressed once
	if (_input->isKeyPressed(CMD_JUMP))
	{
		//Permit player to perform jump even if button pressed 6 frames too early (before landing)
		_hasJumpedCooldown = utils::StepCooldown(_hasJumpedCooldown, 1.f, _deltaTime, 6.f);
		if(_hasJumpedCooldown >= 6.f)
			_hasJumped = true;
	}
	else
	{
		_hasJumpedCooldown = 0.f;
		_hasJumped = false;
	}

	//Aiming
	if (_input->isKeyPressed(CMD_AIM))
		cout << "frame = " << _animation.GetFrame() << "  numAnimation = " << _animation.GetAnimation() << endl;
	if (_input->isKeyPressed(CMD_AIM) && _attacking == 0.f && _counteract == 0.f && _canWallJump == 0.f && _dash == 0.f && _canClimb == 0.f)
	{
		if (!_isAiming && _speed.y >= 0)
		{
			if (_aimingSpeedCooldown == 0.f)
			{
				_body->SetLinearVelocity(b2Vec2(_speed.x, 0));
				_aimingSpeedCooldown = 60.f;
			}
			_speed.y = 0;
		}
		_isAiming = true;
	}
	else
		_isAiming = false;

    //Climbing on ladders
    if(_body != NULL && !_isAiming && _canClimb >= 10.f && _dash == 0.f && _attacking == 0.f &&
	   !_input->isKeyPressed(CMD_LEFT) && !_input->isKeyPressed(CMD_RIGHT))
    {
		//Climbing up/down and stay
		if (_input->isKeyPressed(CMD_UP) && _position.y - 25.f > _ladderPos.y - (_ladderHeight / 2.f))
		{
			_body->ApplyForce(b2Vec2(0.f, 0 * _body->GetMass()), _body->GetWorldCenter(), true);
			_body->SetLinearVelocity(b2Vec2(_body->GetLinearVelocity().x, -5));
		}
		else if (!_input->isKeyPressed(CMD_DOWN))
		{
			_body->ApplyForce(b2Vec2(0.f, -42 * _body->GetMass()), _body->GetWorldCenter(), true);
			_body->SetLinearVelocity(b2Vec2(_body->GetLinearVelocity().x, 0));
		}
		else if (_speed.y < 0)
			_body->SetLinearVelocity(b2Vec2(_speed.x, 0.f));

		//Re-positionning if above ladder
		if (_position.y - 25.f < _ladderPos.y - (_ladderHeight / 2.f))
			_body->SetTransform(b2Vec2(_position.x / SCALE, (_ladderPos.y - (_ladderHeight / 2.f) + 25.f) / SCALE), 0);
    }

	//Dashing
	if (_body != NULL && _stunned == 0.f && _dashCooldown == 0.f && _attacking == 0.f && _input->isKeyPressed(CMD_DASH) &&
		_input->isKeyPressed(CMD_ANY_DIRECTION) && !_hasDash && _canWallJump == 0.f && !_stuck &&
		(!_onTheFloor || (_onTheFloor && !_input->isKeyPressed(CMD_DOWN))))
	{
		//Dash direction
		Vector2f stickDirection = _input->GetStickDirection();
		_dashDirection = Vector2f(DASHING_SPEED * stickDirection.x, DASHING_SPEED * stickDirection.y);

		//If diagonal dash, speed is increased regarding angle (feels better than radius direction)
		if (_dashDirection.x != 0.f && _dashDirection.y != 0.f)
			_dashDirection = Vector2f(_dashDirection.x * (1.f + fabs(stickDirection.y)/2.f),
									  _dashDirection.y * (1.f + fabs(stickDirection.x)/2.f));


		//If hooked, can dash on in some directions
		if ((_hooked < 0 && _dashDirection.x > 0) ||
			(_hooked > 0 && _dashDirection.x < 0))
			return;

		//Cancel every command
		_attacking = 0.f;
		_counteract = 0.f;
		_canClimb = 0.f;
		_canWallJump = 0.f;
		_hooked = 0;

		//Dash duration / cooldown
		_dash = 21.f;
		_dashCooldown = 45.f;
		_dashEffectFrame = 1.f;
		_flashFrame = 18.f;
		_body->SetGravityScale(0.f);
		int dashUserData = UD_DASH;
		_body->SetUserData((void*)dashUserData);
		_bodyClone->SetUserData((void*)dashUserData);
		_effectBox->ShakeScreen(2, 8);
		_body->SetLinearVelocity(b2Vec2(0, 0));
		DrawEffect(DASH_STAR);
	}
	if (_dash > 12.f)
	{
		//Moving character during dash
		_body->SetLinearVelocity(b2Vec2(_dashDirection.x / (_dash / 15.f), _dashDirection.y / (_dash / 15.f)));

		//Adding effect every frame (based on 60)
		if (_dashEffectFrame == 0.f && (fabs(_body->GetLinearVelocity().x) > 0.1 || fabs(_body->GetLinearVelocity().y) > 0.1))
			DrawEffect(DASH_TRAIL);
		else
			_dashEffectFrame = utils::StepCooldown(_dashEffectFrame, 1.f, _deltaTime);
	}
	else if (_dash > 0.f && _dash <= 12.f)
		_body->SetLinearVelocity(b2Vec2(_dashDirection.x *(_dash/50.f), _dashDirection.y * (_dash / 50.f)));

	// Cancel dash parameters
	if (_dash == 0.f && _canClimb == 0.f && _platformFall == 0.f)
	{
	    int voidUserData = UD_VOID;
		_body->SetGravityScale(1.f);
		_body->SetUserData((void*)voidUserData);
		_bodyClone->SetUserData((void*)voidUserData);
	}

	//Locking dashing key if already pressed once
	if (_input->isKeyPressed(CMD_DASH))
		_hasDash = true;
	else
		_hasDash = false;

	// Crouch - Command down
	if (_attacking == 0.f && _canClimb == 0.f && _hooked == 0 && _canWallJump == 0.f && _onTheFloor && _dash == 0.f && !_isAiming &&
		_input->isKeyPressed(CMD_DOWN) && !_input->isKeyPressed(CMD_LEFT) && !_input->isKeyPressed(CMD_RIGHT))
	{
		if (!_crouch)
			DrawEffect(DUST_CROUCHING);
		_crouch = true;
		if ((int)Box2DTools::GetAABBSize(_body, true).y > 20)
		{
			float lastSpeedX = _body->GetLinearVelocity().x;
			ResetBody(20, 20);
			_body->SetTransform(b2Vec2(_body->GetPosition().x, _body->GetPosition().y + 7.5f / SCALE), 0);
			_body->SetLinearVelocity(b2Vec2(0, _body->GetLinearVelocity().y));
		}
		else if (_input->isKeyPressed(CMD_JUMP) && _collisionType == Box2DTools::PLATFORM_ENTITY)
		{
			_crouch = false;
			_hasJumped = true;
			_onTheFloor = false;
			_collisionType = -1;
			ResetBody(20, 35);
			_platformFall = 15.f;
		}
	}
	if((!_input->isKeyPressed(CMD_DOWN) || !_onTheFloor) && _crouch && !_stuck && _attacking == 0.f)
	{
		_crouch = false;
		if ((int)Box2DTools::GetAABBSize(_body, true).y < 35)
		{
			ResetBody(20, 35);
			_body->SetTransform(b2Vec2(_body->GetPosition().x, _body->GetPosition().y - 7.5f / SCALE), 0);
		}
	}

	//Limiting falling speed
    if(_canWallJump != 0.f)
        _maxSpeed.y = 5.f;
	else if (_canClimb >= 10.f)
		_maxSpeed.y = 10.f;
    else if(_dash == 0.f)
        _maxSpeed.y = FALLING_SPEED;

	//Body clone visible
	_cloneVisible = _position.x - 40 <= 0 || _position.x + 40 >= utils::VIEW_WIDTH || _position.y - 40 <= 0 || _position.y + 40 >= utils::VIEW_HEIGHT;
}

bool hero::PerformAttack() { return false; } //Override in child class

void hero::AnimSprite()
{
    //Animation speed
	if (_animation.GetAnimation() == 5 && _body->GetLinearVelocity().y < 0.f)
		_animation.SetFrameDelay(0.f);
	else if ((_animation.GetAnimation() == 7 && _maxSpeed.x > WALKING_SPEED + 1) || _counteract != 0.f || _hooked != 0)
		_animation.SetFrameDelay(0.25f);
	else if (_dash > 0.f || _animation.GetAnimation() == 7)
		_animation.SetFrameDelay(0.35f);
	else if (_animation.GetAnimation() == 0 || _animation.GetAnimation() == 27)
		_animation.SetFrameDelay(0.12f);
	else if (_animation.GetAnimation() == 1)
		_animation.SetFrameDelay(0.23f*(fabs(_input->GetStickDeltaX()) < 0.6f ? 0.6f : fabs(_input->GetStickDeltaX())));
    else
		_animation.SetFrameDelay(0.2f);

    //Reverse sprite
    if(_canWallJump == 0.f && _counteract == 0.f && _attacking == 0.f && _hooked == 0 && _dash == 0.f && _canClimb < 10.f)
    {
        if(_input->isKeyPressed(CMD_LEFT))
            _animation.ReverseSprite(true);
        else if(_input->isKeyPressed(CMD_RIGHT))
            _animation.ReverseSprite(false);
    }

    //Changing opacity if character invincible
    if(_stunned == 0 && _invincibilityFrame > 0)
        _sprite.setColor(Color(255,255,255,((int)_invincibilityFrame%3!=0 ? 180 : 40)));
    else if(_dash == 0.f)
        _sprite.setColor(Color(255,255,255));

    //{All animation
    if(_stunned != 0)
    {
        if(_stunDirection.x > 0.5)
            _animation.ReverseSprite(true);
        else if(_stunDirection.x < -0.5)
            _animation.ReverseSprite(false);
        _animation.SetAnimation(24,0);
    }
	else if (_dash > 0.f)
	{
		if(fabs(_dashDirection.x) >= fabs(_dashDirection.y)) //Horizontal and diagonal dash
			_animation.SetAnimation(_dash <= 15.f ? 21 : 20, 6);
		else if (_dashDirection.y < 0.f) //Vertical up dash
			_animation.SetAnimation(_dash <= 8.f ? 2 : 22, 3);
		else //Vertical down dash
			_animation.SetAnimation(_dash <= 5.f ? 3 : 23, 3);
	}
    else if(_canClimb >= 10.f)
    {
        _animation.ReverseSprite(false);
        if(_speed.y < -0.25f) //Moving up
            _animation.SetAnimation(9, 0);
		else if (_speed.y > 0.25f) //Moving down
			_animation.SetAnimation(28, 2);
        else //Static
            _animation.SetAnimation(10,0);
    }
    else if(_hooked != 0)
    {
        _animation.SetAnimation(8, 4);
        _animation.ReverseSprite(_hooked > 0);
    }
	else if (_isAiming)
		_animation.SetAnimation(!_onTheFloor && _speed.y > 0 ? 19 : 18, 2);
    else if(_attacking != 0.f)
		AnimAttack(); //Hidden method
    else if(_counteract != 0.f)
    {
        _animation.SetAnimation(6, 0);
        _animation.ReverseSprite(_counteract < 0.f);
    }
    else if(_canWallJump != 0.f)
    {
        _animation.ReverseSprite(_canWallJump < 0.f);
        _animation.SetAnimation(5, 2);
    }
    else if(!_onTheFloor && _speed.y < 0)
        _animation.SetAnimation(2, 2);
    else if(!_onTheFloor && _speed.y >= 0)
        _animation.SetAnimation(3, 3);
    else if(_onTheFloor && !_crouch && _input->isKeyPressed(CMD_LEFT) && _speed.x > 0.1)
        _animation.SetAnimation(4, 0);
    else if(_onTheFloor && !_crouch && _input->isKeyPressed(CMD_RIGHT) && _speed.x < -0.1)
        _animation.SetAnimation(4, 0);
    else if(!_crouch && fabs(_speed.x) > 0.5)
        _animation.SetAnimation(fabs(_speed.x) > WALKING_SPEED+1 ? 7 : 1, 0);
	else if (_crouch && fabs(_speed.x) > 0.5 && (_animation.GetAnimation() == 26 || _animation.GetAnimation() == 27))
		_animation.SetAnimation(27, 1);
	else if (_crouch && _onTheFloor)
		_animation.SetAnimation(26, 5, _animation.GetAnimation() == 27 ? 5.f : 0.f);
	else if (_input->isKeyPressed(CMD_UP) && _onTheFloor)
		_animation.SetAnimation(25, 5);
	else
		_animation.SetAnimation(0, 0);
    //}

    //Perform animation + position + drawing
    _animation.PerformAnimation(_deltaTime);
	if(_canClimb == 0.f)
		_sprite.setPosition(_position.x, _position.y + (_crouch ? 9.5 : 17));
	else
	{
		//Positioning hero on ladder smoothly
		float gapSpriteX = (_position.x - _sprite.getPosition().x) / 3.f*_deltaTime;
		_sprite.setPosition(_sprite.getPosition().x + gapSpriteX, _position.y + (_crouch ? 9.5 : 17));
	}

    //Perform sprite rotation
	if (_dash == 0.f)
	{
		if (_onTheFloor && _animation.GetAnimation() != 4)
			_sprite.setRotation(_speed.x / 2);
		else
			_sprite.setRotation(0);
	}

	//Aim arrow
	_aimArrow.setPosition(Vector2f(_sprite.getPosition().x, _sprite.getPosition().y - 20.f));
	int rotation = _input->GetStickAngle(_animation.GetReversed() ? 270 : 90);
	/*rotation = rotation == 0 && _aimArrow.getRotation() > 180 ? 360 : rotation;
	float actualRotation = _aimArrow.getRotation() == 0.f && rotation > 180 ? 360.f : _aimArrow.getRotation();
	float newRotation = utils::StepCooldown(actualRotation, 30.f, _deltaTime, rotation);
	_aimArrow.setRotation(newRotation);*/
	_aimArrow.setRotation(rotation);
}

void hero::AnimAttack() {} //Override in child class

b2Body* hero::GetBodyA()
{
    return _body;
}

b2Body* hero::GetBodyB()
{
    return _bodyClone != NULL && _cloneVisible ? _bodyClone : _body;
}

Vector2f hero::GetPositionA()
{
	if (_body != NULL)
		return Vector2f(_body->GetPosition().x*SCALE + _speed.x, _body->GetPosition().y*SCALE +_speed.y);
	else
		return Vector2f();
}

Vector2f hero::GetPositionB()
{
	if (_bodyClone != NULL && _cloneVisible && _bodyClone->IsActive())
		return Vector2f(_bodyClone->GetPosition().x*SCALE + _speed.x, _bodyClone->GetPosition().y*SCALE + _speed.y);
	else if (_body != NULL)
		return Vector2f(_body->GetPosition().x*SCALE + _speed.x, _body->GetPosition().y*SCALE + _speed.y);
	else
		return Vector2f();
}

int hero::isCounteracting()
{
    return fabs(_counteract) > 10.f && _hasCounteractedCooldown == 0.f ? (int)(_counteract/fabs(_counteract)) : 0;
}

void hero::SetSelfProjectileInvincible(b2Body * projectileBody)
{
	_selfProjectileInvincible = 8.f;
	_lastProjectile = projectileBody;
}

bool hero::IsSelfProjectileInvincible(b2Contact* contact)
{
	if (_selfProjectileInvincible > 0.f &&
	    (contact->GetFixtureA()->GetBody() == _lastProjectile ||
		 contact->GetFixtureB()->GetBody() == _lastProjectile))
		return true;
	else
		return false;
}

bool hero::IsDashing()
{
	return _dash > 4.f;
}

bool hero::IsStunned()
{
	return fabs(_stunned) > 0.f;
}

void hero::HasCounteracted()
{
    _hasCounteractedCooldown = 10.f;
}

void hero::SetStunned(Vector2f stunDirection, int stunDuration, b2Body* projectileOrigin)
{
    if(_stunned == 0 && _invincibilityFrame == 0 && _life > 0)
    {
        //Stun
        _life -= _life > 0 ? 1 : 0;
        _stunned = stunDuration;
		_stunDirection = stunDirection;
        _invincibilityFrame = (float)stunDuration*5.f;
		_attacking = 0.f;

        //Score
        if(_life == 0 && projectileOrigin != NULL)
        {
            if(projectileOrigin != _body) //If the b2Body who threw the projectile isn't my b2body (so isn't me)
            {
				_scoreManager->AddPoints(-1, _body);
				_scoreManager->AddPoints(1, projectileOrigin);
            }
            else //so it's me - suicide :(
				_scoreManager->AddPoints(-2, _body);
        }

		//Screen shake
		_effectBox->ShakeScreen(6, 4);
    }
}

void hero::SetHasJumped(bool jumped)
{
	_hasJumped = jumped;
	_hasCounteract = jumped;
}

int hero::GetColorID()
{
	return _colorID;
}

Color hero::GetHeroColor(int i)
{
    switch(i)
    {
        case 0:
            return Color(255,50,50);
            break;
        case 1:
            return Color(255,137,0);
            break;
        case 2:
            return Color(242,13,255);
            break;
        case 3:
            return Color(130,255,13);
            break;
        case 4:
            return Color(13,110,255);
            break;
        default:
            return Color(255,50,50);
            break;
    }
}

void hero::CrushHero()
{
    _animation.PerformFallingScale();
}

int hero::IsAPlayer(b2Body* body)
{
    for(int i=0 ; i<_players.size() ; i++)
    {
        if(_players[i]->GetBodyA() != NULL && _players[i]->GetBodyB() != NULL && body != NULL &&
		  (_players[i]->GetBodyA() == body || _players[i]->GetBodyB() == body))
            return i;
    }
    return -1;
}

int hero::GetLife()
{
    return _life;
}

void hero::ResetHero(int numColor, int life)
{
	_colorID = numColor;
    LoadHeroTexture();
    _hasJumped = true;
    _life = life;
    ResetLifeSprite();
    _maxLife = life;
	_animation.ReverseSprite(_initPos.x > 450);
	if (_body != NULL && _bodyClone != NULL)
	{
		_body->SetActive(true);
		_body->SetTransform(b2Vec2(_initPos.x / SCALE, _initPos.y / SCALE), 0);
		_bodyClone->SetActive(false);
	}
}

void hero::ResetBody(int width, int height)
{
	if (_body != NULL && _bodyClone != NULL)
	{
		_world->DestroyBody(_body);
		_world->DestroyBody(_bodyClone);
	}
	_body = Box2DTools::CreateRectangleBox(_world, _position, width, height, 0.1, 0, 0, Box2DTools::BODY_ENTITY);
	_bodyClone = Box2DTools::CreateRectangleBox(_world, _position, width, height, 0.1, 0, 0, Box2DTools::BODY_ENTITY);
}

void hero::ResetLifeSprite()
{
    //Delete
    for(int i=0 ; i<_lifeSprite.size() ; i++)
        delete _lifeSprite[i];
    _lifeSprite.clear();

    //Recreate
    for(int i=0 ; i<_life ; i++)
    {
        _lifeSprite.push_back(new Sprite());
        _lifeSprite[_lifeSprite.size()-1]->setTexture(_lifeTexture);
        _lifeSprite[_lifeSprite.size()-1]->setTextureRect(IntRect(0,0,14,12));
        _lifeSprite[_lifeSprite.size()-1]->setOrigin(7,6);
    }
}

void hero::DisableBody()
{
	if (_body != NULL && _bodyClone != NULL)
	{
		_body->SetActive(false);
		_bodyClone->SetActive(false);
	}
}

void hero::DrawEffect(CHARA_EFFECT effectType, int pos)
{
	if (effectType != DASH_TRAIL && effectType != DASH_STAR && _dash > 0.f)
		return;

	switch (effectType)
	{
		case DUST_LANDING:
			_effectBox->AddDust(Vector2f(_position.x - 10, _position.y + 20), true, false);
			_effectBox->AddDust(Vector2f(_position.x - 10, _position.y + 20), true, false);
			_effectBox->AddDust(Vector2f(_position.x - 10, _position.y + 20), true, false);
			_effectBox->AddDust(Vector2f(_position.x + 10, _position.y + 20), false, false);
			_effectBox->AddDust(Vector2f(_position.x + 10, _position.y + 20), false, false);
			_effectBox->AddDust(Vector2f(_position.x + 10, _position.y + 20), false, false);
			break;
		case DUST_WALLJUMP:
			_effectBox->AddDust(Vector2f(_position.x + pos, _position.y + 8), _animation.GetReversed(), true);
			_effectBox->AddDust(Vector2f(_position.x + pos, _position.y - 20), _animation.GetReversed(), true);
			break;
		case DUST_CROUCHING:
			_effectBox->AddDust(Vector2f(_position.x - 15, _position.y + 20), true, false, 0.6);
			_effectBox->AddDust(Vector2f(_position.x, _position.y + 20), false, false, 0.7);
			_effectBox->AddDust(Vector2f(_position.x + 15, _position.y + 20), false, false, 0.6);
			break;
		case DASH_TRAIL:
			_dashEffectFrame = 1.f;
			_effectBox->AddRectangleShape(
				drawOption(Vector2f(_position.x, _position.y - 5.f), Vector2f(6, 6), Vector2f(3, 3),
				Color(0, 0, 0, 0), 45, 255, Color(255, 255, 255), 2),
				fadeOption(Vector2f(0, 0), 10.f, 7.f, 50),
				physics(false));
			break;
		case DASH_STAR:
			Vector2f pos = Vector2f(_position.x - _speed.x, _position.y - _speed.y);
			Vector2f dirs[4] = { Vector2f(0.5,0.5), Vector2f(-0.5,0.5), Vector2f(-0.5,-0.5), Vector2f(0.5,-0.5) };
			Color colors[4] = { Color(255,0,0), Color(200,0,0), Color(150,0,0), Color(100,0,0) };
			for(int i=0 ; i<=4 ; i++)
				_effectBox->AddRectangleShape(
					drawOption(pos, Vector2f(6, 6), Vector2f(0, 0), Color(0, 0, 0, 0), i*90+45, 255, colors[i], 2),
					fadeOption(dirs[i], 10.f, 0, 65, 0, Vector2f(0.05, 0.05)),
					physics(false));
			break;
	}
}

void hero::DrawLife(RenderWindow* window)
{
    if(_lifeSprite.size() <= 1)
        return;

    for(int i=0 ; i<_lifeSprite.size() ; i++)
    {
        if(_invincibilityFrame > 0)
            _lifeSprite[i]->setColor(Color(255,255,255));
        else
            _lifeSprite[i]->setColor(Color(255,255,255,200));

        _lifeSprite[i]->setPosition((_position.x+i*15)-((_lifeSprite.size()*14)/2)+4, _position.y + (_crouch ? 17.5 : 25));
        _lifeSprite[i]->setTextureRect(IntRect(i < _life ? 0 : 14,0,14,12));
		utils::DrawLoopSprite(window, _lifeSprite[i]);
    }
}

void hero::Step(float deltaTime)
{
    //Update delta time
    _deltaTime = deltaTime;

    //Update main variables
	Update();

	//Dead
	if (_life == 0 && _stunned == 0) { return; }

	//Inputs
	if (_stunned == 0 && _life > 0) { Move(); }

	//Calculating effects and animations
	PerformEffects();
	AnimSprite();
}

void hero::Display(RenderWindow* window, float deltaTime)
{
    _deltaTime = deltaTime;

	//Hero is dead
    if(_life == 0 && _stunned == 0) { return; }

	//Drawing common sprites
    DrawLife(window);
	utils::DrawLoopSprite(window, &_sprite);
	if (_flashFrame > 0)
	{
		utils::CopySprite(&_spriteFlash, &_sprite, &_textureFlash[_animation.GetTextureNum()], true);
		_spriteFlash.setColor(Color(255, 255, 255, (_flashFrame > 8.f ? 255 : 30 * _flashFrame)));
		utils::DrawLoopSprite(window, &_spriteFlash);
	}

	//Drawing aiming arrow
	if (_isAiming)
		utils::DrawLoopSprite(window, &_aimArrow);


	/*RectangleShape bodyShape = RectangleShape(Vector2f(20.f, 3.f));
	bodyShape.setFillColor(Color::Red);
	bodyShape.setOrigin(10.f, 1.5f);
	bodyShape.setPosition(_position.x, _position.y + 17.5f);
	window->draw(bodyShape);*/
}
