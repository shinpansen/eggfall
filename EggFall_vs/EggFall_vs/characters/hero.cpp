#include "hero.hpp"

static const float SCALE = 30.f;
static const float WALKING_SPEED = 10.f;
static const float RUNNING_SPEED = 15.f;
static const float FALLING_SPEED = 20.f;
static const float DASHING_SPEED = 25.f;

hero::hero()
{
}

hero::~hero()
{
	//Destroy Box2D bodies
	if (_body != NULL && _bodyClone != NULL)
	{
		_world->DestroyBody(_body);
		_world->DestroyBody(_bodyClone);
	}

	//Destroy sprites
	if (_heroSprite != NULL)
		delete _heroSprite;
	if (_heroSpriteFlash != NULL)
		delete _heroSpriteFlash;
	if (_aimArrow != NULL)
		delete _aimArrow;

	//Destroy animation
	if (_animation != NULL)
		delete _animation;

	//Destroy HUD
	if(_playerHUD != NULL)
		delete _playerHUD;
}

hero::hero(int colorID, int life, Vector2f pos, b2World* world, input* input, effects* effectBox,
	       projectiles* projectileBox, sound* soundBox, scoreManager* scoreManager, Texture * heroTexture,
	       Texture * heroTextureFlash, Texture * lifeStaminaTexture, Texture * aimArrowTexture)
{
    //Variable
    _onTheFloor = false;
    _lastOnTheFloor = false;
	_onBridge = false;
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
	_colorID = colorID >= 0 && colorID <= 4 ? colorID : 0;
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
	_attackingCounteract = 0.f;
    _counteract = 0.f;
	_lastCounteract = 0.f;
	_lastCounteractSigne = 0.f;
    _counteractCooldown = 0.f;
    _hasCounteractedCooldown = 0.f;
	_isCounteractingProjectile = 0.f;
	_dash = 0.f;
	_dashCooldown = 0.f;
	_dashEffectFrame = 0.f;
	_canClimb = 0.f;
	_stamina = 100.f;
	_staminaCooldown = 0.f;
	_stunned = 0.f;
	_lastSpeedX = 0.f;
    _dustFrame = 0.f;
	_dustDuration = 0.f;
	_dustCooldown = 0.f;
	_invincibilityFrame = 0.f;
	_ladderHeight = 0.f;
	_flashFrame = 0.f;
	_hasJumpedCooldown = 0.f;
	_spellAngle = 0.f;
	_selfProjectileInvincible = 0.f;
	_lastFallingSpeed = 0.f;
	_updateFallingSpeedFrame = 0.f;
	_aimingSpeedCooldown = 0.f;
	_platformFall = 0.f;
	_deltaTime = 1.f;
	_lastProjectile = NULL;
	_counteractedProjectile = NULL;

    //Vectors
    _initPos = pos;
    _speed = Vector2f(0.f, 0.f); //In Box2D unit
    _maxSpeed = Vector2f(RUNNING_SPEED, FALLING_SPEED);
    _position = Vector2f(pos.x, pos.y); //In SFML pixel unit
    _positionClone = Vector2f(pos.x, pos.y); //In SFML pixel unit
	_dashDirection = Vector2f(0, 0);
	_lastContactPos = Vector2f(0, 0); //In SFML pixel unit
    _hookedPoint = Vector2f(0,0);
    _ladderPos = Vector2f(0,0);
    _stunDirection = Vector2f(0,0);
	_projectileSpeed = Vector2f(0, 0);
	_projectilePos = Vector2f(0, 0);
	_hitRelativePos = Vector2f(0, 0);

    //Objects
    _input = input;
	_effectBox = effectBox;
	_projectileBox = projectileBox;
    _soundBox = soundBox;
    _wallSlidingSound = NULL;
    _scoreManager = scoreManager;
	_projectileEffect = NULL;
	_hitEffect = NULL;
	_shieldEffect = NULL;
	_grindEffect = NULL;

    //Box2D
    _world = world;
    _body = Box2DTools::CreateRectangleBox(_world, pos, 20, 35, 0.1, 0, 0, Box2DTools::BODY_ENTITY);
    _bodyClone = Box2DTools::CreateRectangleBox(_world, pos, 20, 35, 0.1, 0, 0, Box2DTools::BODY_ENTITY);
    _bodyClone->SetActive(false);
    int voidUserData = UD_VOID;
	_body->SetUserData((void*)voidUserData);
	_bodyClone->SetUserData((void*)voidUserData);

	//Textures
	_heroTexture = heroTexture;
	_heroTextureFlash = heroTextureFlash;
	_lifeStaminaTexture = lifeStaminaTexture;
	_aimArrowTexture = aimArrowTexture;

	//Hero sprites
	_heroSprite = new Sprite();
	_heroSprite->setTexture(*_heroTexture);
	_heroSprite->setOrigin(40.f, 84.f);
	_heroSpriteFlash = new Sprite();
	_heroSpriteFlash->setTexture(*_heroTextureFlash);

	//Animation - HUD
	_animation = new animation(_heroSprite, 0.1, 84, 84, 6, 36, 0, false, 3);
	_playerHUD = new playerHUD(_lifeStaminaTexture, _maxLife);

	//Reverse sprite with initial position
	if (_body->GetPosition().x*SCALE > utils::VIEW_WIDTH)
		_animation->ReverseSprite(true);

	//Life sprites
	ResetLifeSprites();

	//Aim arrow sprite - circle
	_aimArrow = new Sprite();
	_aimArrow->setTexture(*_aimArrowTexture);
	_aimArrow->setOrigin(_aimArrow->getLocalBounds().width / 2.f, _aimArrow->getLocalBounds().height / 2.f);
}

void hero::Update()
{
	//Waiting to respawn hero
    if(_life == 0 && _stunned == 0.f) 
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
			_animation->ReverseSprite(_initPos.x > 450);
			_canClimb = 0.f;
			_stamina = 100.f;
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
	_onBridge = false;
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
					_onBridge = _collisionType == Box2DTools::PLATFORM_ENTITY && body->GetType() == b2_dynamicBody;
					_canWallJump = 0.f;
					
					//Block player horizontal speed while dashing
					if (_dashDirection.y > 0.f && _dash > 0.f && _collisionType != Box2DTools::PLATFORM_ENTITY)
					{
						_body->SetLinearVelocity(b2Vec2(0, _speed.y));
						_dash = 0.f;
					}

					//If calculating exact contact position
					if (i == 0)
						_lastContactPos = Vector2f(_position.x, body->GetPosition().y * SCALE - Box2DTools::GetAABBSize(body, true).y / 2.f);

					//Break only if on ground. If on platform, better test all collision points. Down + jump only work if full body is only on platform
					if(_collisionType != Box2DTools::PLATFORM_ENTITY)
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
		_animation->PerformFallingScale(1.f - (_lastFallingSpeed*3.f) / 100.f);
	}

    //Jump tolerance - Permit the hero to jump during some frames (60fps based) after leaving the floor
	if (_onTheFloor)
		_jumpTolerance = 8.f;
	else if (_jumpTolerance > 0.f)
		_jumpTolerance = utils::StepCooldown(_jumpTolerance, 1.f, _deltaTime);

    //Dust of moving
	/*float dustFrameStep = fabs(_speed.x) > 0.1f && fabs(_speed.x) < 9.5f ? 25.f : 10.f;  //_canWallJump != 0.f ? fabs(_speed.y) / 3.f : fabs(_speed.x) / 3.f;
	_dustDuration = utils::StepCooldown(_dustDuration, 1.f, _deltaTime);
	if(_dustDuration > 0.f)
		_dustFrame = utils::StepCooldown(_dustFrame, 1.f, _deltaTime, 15.f);*/

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
			_hooked = Box2DTools::TestHookPoint(_world, posLeft, &_hookedPoint, _animation->GetReversed());
			if (_hooked == 0)
				_hooked = Box2DTools::TestHookPoint(_world, posRight, &_hookedPoint, _animation->GetReversed());
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
		_lastReversed = _animation->GetReversed();
	else if (lastCanClimb == 10.f && _canClimb < 10.f)
		_animation->ReverseSprite(_lastReversed);

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

    //Multiple cooldown update
	float lastDash = _dash;
	_lastCounteract = _counteract;
	_counteract = utils::StepCooldown(_counteract, 1.f, _deltaTime);
	_counteractCooldown = utils::StepCooldown(_counteractCooldown, 1.f, _deltaTime);
	_hasCounteractedCooldown = utils::StepCooldown(_hasCounteractedCooldown, 1.f, _deltaTime);
	_isCounteractingProjectile = utils::StepCooldown(_isCounteractingProjectile, 1.f, _deltaTime);
	_dash = utils::StepCooldown(_dash, 1.f, _deltaTime);
	_dashCooldown = utils::StepCooldown(_dashCooldown, 1.f, _deltaTime);
	_dashStep = _dash >= 15.f && (int)((lastDash+0.5f)*10.f) != (int)((_dash+0.5f)*10.f);
	_flashFrame = utils::StepCooldown(_flashFrame, 1.f, _deltaTime);
	_selfProjectileInvincible = utils::StepCooldown(_selfProjectileInvincible, 1.f, _deltaTime);
	_aimingSpeedCooldown = utils::StepCooldown(_aimingSpeedCooldown, 1.f, _deltaTime);
	_platformFall = utils::StepCooldown(_platformFall, 1.f, _deltaTime);
	_dustCooldown = utils::StepCooldown(_dustCooldown, 1.f, _deltaTime);
	_staminaCooldown = utils::StepCooldown(_staminaCooldown, 1.f, _deltaTime);
	_attackingCounteract = utils::StepCooldown(_attackingCounteract, 1.f, _deltaTime);
	_dustDuration = utils::StepCooldown(_dustDuration, 1.f, _deltaTime);
	_dustFrame = utils::StepCooldown(_dustFrame, 1.f, _deltaTime);
	if(_staminaCooldown == 0.f)
		_stamina = utils::StepCooldown(_stamina, 1.f, _deltaTime, 100.f);

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
		if (_hitEffect != NULL)
		{
			_position = Vector2f(_body->GetPosition().x*SCALE, _body->GetPosition().y*SCALE);
			_hitEffect->setPosition(Vector2f(_position.x + _hitRelativePos.x, _position.y + _hitRelativePos.y*0.8f));
			_hitEffect->setRotation(utils::GetVectorRotation(_speed) + 90);
		}
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

	//Killing hero if running out of life
    if(_life == 0 && _stunned <= 1.f) 
    {
		Vector2f animPos = Vector2f(_position.x + 4.f, _position.y - 18.f);
		_effectBox->AddAnimation(T_DEAD, 0, 0.2, _animation->GetReversed(), drawOption(animPos), fadeOption(0), physics(false));
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

	//Updating HUD
	_playerHUD->Step(this, _deltaTime);
}

void hero::PerformEffects()
{
    //Animation of falling - crush sprite + dust
    if(!_lastOnTheFloor && _onTheFloor && _dash == 0.f && _attacking == 0.f &&
	  (_collisionType == Box2DTools::GROUND_ENTITY || _collisionType == Box2DTools::PLATFORM_ENTITY))
    {
		_animation->PerformFallingScale(1.f - (_lastFallingSpeed*3.f) / 100.f);
		if (_dustCooldown == 0.f)
		{
			_soundBox->PlaySound(SOUND::SND_FALLING, false);
			DrawEffect(DUST_LANDING);
			_dustCooldown = 25.f;
		}
    }

	//If accelerating drawing dust
	if ((_input->isKeyPressed(CMD_LEFT) && _speed.x > -_maxSpeed.x + 3.f ||
		_input->isKeyPressed(CMD_RIGHT) && _speed.x < _maxSpeed.x - 3.f) &&
		fabs(_speed.x) > 0.1f)
		_dustDuration = _maxSpeed.x == RUNNING_SPEED ? 10.f : 20.f;
	_lastSpeedX = _speed.x;


    //Dust moving
	if (_onTheFloor && _canWallJump == 0.f && fabs(_speed.x) > 0.1f && _dustDuration > 0.f && _dustFrame == 0.f &&
		(_collisionType == Box2DTools::GROUND_ENTITY || _collisionType == Box2DTools::PLATFORM_ENTITY))
	{
		_dustFrame = _maxSpeed.x == RUNNING_SPEED ? 3.f : 6.f;
		DrawEffect(DUST_MOVING);
	}

    //Dust wall sliding
    if(_canWallJump != 0.f && !_onTheFloor && _speed.y > 1.f && _dustFrame == 0.f && _dash == 0.f)
    {
		_dustFrame = 6.f;
		DrawEffect(DUST_WALLJUMP);
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
		_animation->CancelJumpFallScale();
		_cancelJumpFallScale = false;
	}
}

void hero::Move()
{
	//Inertia parameters
	float deltaAdjust = _deltaTime;
    float scalingSpeed = (!_onTheFloor || _counteract != 0.f || _canClimb > 0.f) ? 1.f*deltaAdjust : 1.5*deltaAdjust;
    float scalingBrake = _counteract != 0.f || _attacking != 0.f ? 0.5*deltaAdjust: 1.f*deltaAdjust;

	//Counteracting projectile
	if (_isCounteractingProjectile > 0.f)
	{
		_body->SetLinearVelocity(b2Vec2(0, 0));
		_effectBox->ShakeScreen(8.f, 10.f);
		if (_counteractedProjectile == NULL || _counteractedProjectile->IsDestroyed() ||
			_input->isKeyPressed(CMD_ATTACK) || _isCounteractingProjectile < 10.f)
			PerformCounteract(_counteractedProjectile != NULL && !_counteractedProjectile->IsDestroyed());
		return;
	}
	else if (_attackingCounteract > 0.f)
	{
		_body->SetLinearVelocity(b2Vec2(0, 0));
		return;
	}
	else if (_isCounteractingProjectile == 0.f && _attackingCounteract == 0.f && _body->GetAngle() != 0.f)
	{
		_heroSprite->setOrigin(40.f, 84.f);
		_body->SetGravityScale(1.f);
		_body->SetTransform(_body->GetPosition(), 0);
		_body->SetLinearVelocity(b2Vec2(0, -0.1));
	}

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
    if(_body != NULL && _input->isKeyPressed(CMD_COUNTERACT) && !_isAiming && _dash == 0.f && !_stuck && !_crouch && _stamina > 0.f &&
	   _attacking == 0.f && _counteract == 0.f && _counteractCooldown == 0.f && !_hasCounteract && _canClimb < 10.f)
    {
        int signe = _animation->GetReversed() ? -1 : 1;
		_counteract = (float)signe * 32.f;
		_lastCounteract = _counteract;
		_lastCounteractSigne = signe;
		_counteractCooldown = (float)signe * 55.f;
		_stamina = utils::StepCooldown(_stamina, 20.f, _deltaTime);
		_staminaCooldown = _stamina > 0.f ? 42.f : 100.f;
        _soundBox->PlaySound(SOUND::SND_COUNTERACT, false);
		_shieldEffect = _effectBox->AddAnimation(T_SHIELD, 0, 0.45f, _animation->GetReversed(), drawOption(
			Vector2f(_position.x + signe * 15.f, _position.y - 8.f), Vector2f(24, 24)), 
			fadeOption(Vector2f(0,0), 0.f, 0.f, 32.f, 0.f, Vector2f(0,0), 0.f, false), physics(false));
    }
	if (fabs(_counteract) > 20.f && _body->GetLinearVelocity().y >= -0.1f) //Limiting falling speed during counteract
	{
		_body->SetLinearVelocity(b2Vec2(_body->GetLinearVelocity().x, 0.1f));
		_speed.y = 0.1f;
	}
	if (_lastCounteract != 0.f && _counteract == 0.f && _isCounteractingProjectile == 0.f && _invincibilityFrame == 0.f) //Shield disappearing effect
	{
		_lastCounteractSigne = _lastCounteract > 0.f ? 1 : -1;
		_shieldEffect = _effectBox->AddAnimation(T_SHIELD, 1, 0.45f, _animation->GetReversed(), drawOption(
			Vector2f(_position.x + _lastCounteractSigne * 15.f, _position.y - 8.f), Vector2f(24, 24)),
			fadeOption(0), physics(false));
	}
	_shieldEffect = _counteractCooldown == 0.f ? NULL : _shieldEffect;
	_counteractedProjectile = _isCounteractingProjectile == 0.f ? NULL : _counteractedProjectile;

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
			_animation->PerformJumpingScale();
			_canClimb = 0.f;
			DrawEffect(DUST_JUMPING);
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
			if (((_input->isKeyPressed(CMD_LEFT) && _canWallJump > 0.f) ||
				(_input->isKeyPressed(CMD_RIGHT) && _canWallJump < 0.f)))
				_body->SetLinearVelocity(b2Vec2(_canWallJump > 0.f ? 12.f : -12.f, -16.f));
            else
            {
				DrawEffect(DUST_WALLJUMP_LEAVE);
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
		_effectBox->ShakeScreen(3, 5);
		_body->SetLinearVelocity(b2Vec2(0, 0));
		DrawEffect(DASH_STAR);
	}
	if (_dash > 12.f && _stunned == 0.f)
	{
		//Moving character during dash
		_body->SetLinearVelocity(b2Vec2(_dashDirection.x / (_dash / 15.f), _dashDirection.y / (_dash / 15.f)));
		_animation->ReverseSprite(_dashDirection.x < 0.f);

		//Adding effect every frame (based on 60)
		if (_dashEffectFrame == 0.f && (fabs(_body->GetLinearVelocity().x) > 0.1 || fabs(_body->GetLinearVelocity().y) > 0.1))
			DrawEffect(DASH_TRAIL);
		else
			_dashEffectFrame = utils::StepCooldown(_dashEffectFrame, 1.f, _deltaTime);
	}
	else if (_dash > 0.f && _dash <= 12.f)
		_body->SetLinearVelocity(b2Vec2(_dashDirection.x *(_dash/50.f), _dashDirection.y * (_dash / 50.f)));

	// Cancel dash parameters
	if (_dash == 0.f && _canClimb == 0.f && _platformFall == 0.f && _isCounteractingProjectile == 0.f && _attackingCounteract == 0.f)
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

void hero::PerformCounteract(bool success)
{
	//Destroy projectile or project it
	if (success && _counteractedProjectile != NULL)
	{
		Vector2f baseDir = Vector2f(0.f, -_counteractedProjectile->GetSpeed()*1.2f);
		Vector2f newDir = utils::RotateVector(baseDir, _aimArrow->getRotation());
		_counteractedProjectile->SetDirection(newDir);
	}
	else if (_counteractedProjectile != NULL)
		_counteractedProjectile->DestroyProjectile(_effectBox, _soundBox);

	//Change parameters
	_counteractedProjectile = NULL;
	_shieldEffect = NULL;
	_isCounteractingProjectile = 0.f;
	_selfProjectileInvincible = 8.f;
	_hasAttacked = true;
	_attackingCooldown = 30.f;
	if(_grindEffect != NULL)
	{
		_grindEffect->setColor(Color::Transparent);
		_grindEffect = NULL;
	}

	//End
	if (success)
	{
		//Cooldown
		_attackingCounteract = 20.f;
		_stamina = utils::StepCooldown(_stamina, 66.f, _deltaTime);
		_staminaCooldown = _stamina > 0.f ? 42.f : 100.f;

		//Effect
		_effectBox->ShakeScreen(8.f, 10.f);
		PerformCounteractChildEffect();
	}
	else
		_body->SetGravityScale(1.f);
}

void hero::PerformCounteractChildEffect() {}

void hero::AnimSprite()
{
	if (_heroSprite == NULL)
		return;

    //Animation speed
	if (_animation->GetAnimation() == 5 && _body->GetLinearVelocity().y < 0.f)
		_animation->SetFrameDelay(0.f);
	else if ((_animation->GetAnimation() == 7 && _maxSpeed.x > WALKING_SPEED + 1) || _counteract != 0.f || _hooked != 0)
		_animation->SetFrameDelay(0.25f);
	else if (_dash > 0.f || _animation->GetAnimation() == 7)
		_animation->SetFrameDelay(0.35f);
	else if (_animation->GetAnimation() == 0 || _animation->GetAnimation() == 27)
		_animation->SetFrameDelay(0.12f);
	else if (_animation->GetAnimation() == 1)
		_animation->SetFrameDelay(0.2f*(fabs(_input->GetStickDeltaX()) < 0.6f ? 0.6f : fabs(_input->GetStickDeltaX())));
    else
		_animation->SetFrameDelay(0.2f);

    //Reverse sprite
    if(_canWallJump == 0.f && _counteract == 0.f && _attacking == 0.f && _hooked == 0 && 
	   _dash == 0.f && _canClimb < 10.f && _isCounteractingProjectile == 0.f)
    {
        if(_input->isKeyPressed(CMD_LEFT))
            _animation->ReverseSprite(true);
        else if(_input->isKeyPressed(CMD_RIGHT))
            _animation->ReverseSprite(false);
    }

    //Changing opacity if character invincible
    if(_stunned == 0 && _invincibilityFrame > 0)
        _heroSprite->setColor(Color(255,255,255,((int)_invincibilityFrame%3!=0 ? 180 : 40)));
    else if(_dash == 0.f)
        _heroSprite->setColor(Color(255,255,255));

    //{All animation
    if(_stunned != 0)
    {
        if(_stunDirection.x > 0.5)
            _animation->ReverseSprite(true);
        else if(_stunDirection.x < -0.5)
            _animation->ReverseSprite(false);
        _animation->SetAnimation(24,0);
    }
	else if (_isCounteractingProjectile > 0.f)
		_animation->SetAnimation(29, 3);
	else if(_attackingCounteract > 0.f)
		_animation->SetAnimation(30, 3);
	else if (_dash > 0.f)
	{
		if(fabs(_dashDirection.x) >= fabs(_dashDirection.y)) //Horizontal and diagonal dash
			_animation->SetAnimation(_dash <= 15.f ? 21 : 20, 6);
		else if (_dashDirection.y < 0.f) //Vertical up dash
			_animation->SetAnimation(_dash <= 8.f ? 2 : 22, 3);
		else //Vertical down dash
			_animation->SetAnimation(_dash <= 5.f ? 3 : 23, 3);
	}
    else if(_canClimb >= 10.f)
    {
        _animation->ReverseSprite(false);
        if(_speed.y < -0.25f) //Moving up
            _animation->SetAnimation(9, 0);
		else if (_speed.y > 0.25f) //Moving down
			_animation->SetAnimation(28, 2);
        else //Static
            _animation->SetAnimation(10,0);
    }
    else if(_hooked != 0)
    {
        _animation->SetAnimation(8, 4);
        _animation->ReverseSprite(_hooked > 0);
    }
	else if (_isAiming)
		_animation->SetAnimation(!_onTheFloor && _speed.y > 0 ? 19 : 18, 2);
    else if(_attacking != 0.f)
		AnimAttack(); //Hidden method
    else if(_counteract != 0.f)
    {
        _animation->SetAnimation(6, 5);
        _animation->ReverseSprite(_counteract < 0.f);
    }
    else if(_canWallJump != 0.f)
    {
        _animation->ReverseSprite(_canWallJump < 0.f);
        _animation->SetAnimation(5, 2);
    }
    else if(!_onTheFloor && _speed.y < 0)
        _animation->SetAnimation(2, 2);
    else if(!_onTheFloor && _speed.y >= 0)
        _animation->SetAnimation(3, 3);
    else if(_onTheFloor && !_crouch && _input->isKeyPressed(CMD_LEFT) && _speed.x > 0.1)
        _animation->SetAnimation(4, 0);
    else if(_onTheFloor && !_crouch && _input->isKeyPressed(CMD_RIGHT) && _speed.x < -0.1)
        _animation->SetAnimation(4, 0);
    else if(!_crouch && fabs(_speed.x) > 0.5 && (!_onBridge || (_onBridge && (_input->isKeyPressed(CMD_LEFT) || _input->isKeyPressed(CMD_RIGHT)))))
        _animation->SetAnimation(fabs(_speed.x) > WALKING_SPEED+1 ? 7 : 1, 0);
	else if (_crouch && fabs(_speed.x) > 0.5 && (_animation->GetAnimation() == 26 || _animation->GetAnimation() == 27))
		_animation->SetAnimation(27, 1);
	else if (_crouch && _onTheFloor)
		_animation->SetAnimation(26, 5, _animation->GetAnimation() == 27 ? 5.f : 0.f);
	else if (_input->isKeyPressed(CMD_UP) && _onTheFloor)
		_animation->SetAnimation(25, 5);
	else
		_animation->SetAnimation(0, 0);
    //}

    //Perform animation + position + drawing
    _animation->PerformAnimation(_deltaTime);
	if (_isCounteractingProjectile > 0.f || _attackingCounteract > 0.f)
		_heroSprite->setPosition(_position.x, _position.y);
	else if (_canClimb == 0.f)
		_heroSprite->setPosition(_position.x, _position.y + (_crouch ? 9.5 : 17));
	else
	{
		//Positioning hero on ladder smoothly
		float gapSpriteX = (_position.x - _heroSprite->getPosition().x) / 3.f*_deltaTime;
		_heroSprite->setPosition(_heroSprite->getPosition().x + gapSpriteX, _position.y + (_crouch ? 9.5 : 17));
	}

    //Perform sprite rotation + origin change
	if (_dash == 0.f)
	{
		//Origin
		float heroOriginY = _isCounteractingProjectile == 0.f && _attackingCounteract == 0.f ? 84.f : 70.f;
		_heroSprite->setOrigin(40.f, heroOriginY);

		//Rotation
		float heroRotation;
		if (_isCounteractingProjectile > 0.f || _attackingCounteract > 15.f)
			heroRotation = utils::RadianToDegree(_body->GetAngle()) + (_animation->GetReversed() ? 0 : 180.f);
		else if (_onTheFloor && _animation->GetAnimation() != 4)
			heroRotation = _speed.x / 2.f;
		else
			heroRotation = 0.f;

		while (heroRotation > 360.f)
			heroRotation -= 360.f;
		float distHeroRotation = utils::AngleDistance(_heroSprite->getRotation(), heroRotation);
		float rotationStep = _attackingCounteract == 0.f ? 10.f : 5.f;
		if (fabs(distHeroRotation) <= rotationStep*_deltaTime)
			_heroSprite->setRotation(heroRotation);
		else
		{
			float dist1 = utils::AngleDistance(utils::StepRotation(_heroSprite->getRotation(), rotationStep*_deltaTime), heroRotation);
			float dist2 = utils::AngleDistance(utils::StepRotation(_heroSprite->getRotation(), -rotationStep*_deltaTime), heroRotation);
			if (dist1 > dist2)
				_heroSprite->setRotation(utils::StepRotation(_heroSprite->getRotation(), -rotationStep*_deltaTime));
			else
				_heroSprite->setRotation(utils::StepRotation(_heroSprite->getRotation(), rotationStep*_deltaTime));
		}
	}

	//Aim arrow
	if (_isCounteractingProjectile == 0.f)
	{
		int rotation = _input->GetStickAngle(_animation->GetReversed() ? 270 : 90);
		_aimArrow->setPosition(Vector2f(_heroSprite->getPosition().x, _heroSprite->getPosition().y - 20.f));
		_aimArrow->setRotation(rotation);
	}
	else if(_counteractedProjectile != NULL && !_counteractedProjectile->IsDestroyed()) //Counteract arrow
	{
		int defaultRotation = utils::GetVectorRotation(-_counteractedProjectile->GetDirection());
		int stickRotation = _input->GetStickAngle(defaultRotation);
		float distAngle = utils::AngleDistance(defaultRotation, stickRotation);
		float angleProj = stickRotation;
		if (distAngle > 75.f)
		{
			float dist1 = utils::AngleDistance(utils::StepRotation(defaultRotation, 75.f), stickRotation);
			float dist2 = utils::AngleDistance(utils::StepRotation(defaultRotation, -75.f), stickRotation);
			angleProj = dist1 < dist2 ? utils::StepRotation(defaultRotation, 75.f) : utils::StepRotation(defaultRotation, -75.f);
		}
		_aimArrow->setPosition(_counteractedProjectile->GetPosition());
		_aimArrow->setRotation(angleProj);
	}

	//Child method
	AnimateChild();
}

void hero::AnimAttack() {} //Override in child class

void hero::AnimateChild() {} //Override in child class

b2Body* hero::GetBodyA()
{
    return _body;
}

b2Body* hero::GetBodyB()
{
    return _bodyClone != NULL && _cloneVisible ? _bodyClone : _body;
}

Vector2f hero::GetPositionA(bool addSpeed)
{
	Vector2f speed = addSpeed ? _speed : Vector2f(0, 0);
	if (_body != NULL)
		return Vector2f(_body->GetPosition().x * SCALE + speed.x, _body->GetPosition().y * SCALE + speed.y);
	else
		return Vector2f();
}

Vector2f hero::GetPositionB(bool addSpeed)
{
	Vector2f speed = addSpeed ? _speed : Vector2f(0, 0);
	if (_bodyClone != NULL && _cloneVisible && _bodyClone->IsActive())
		return Vector2f(_bodyClone->GetPosition().x*SCALE + speed.x, _bodyClone->GetPosition().y*SCALE + speed.y);
	else if (_body != NULL)
		return Vector2f(_body->GetPosition().x*SCALE + speed.x, _body->GetPosition().y*SCALE + speed.y);
	else
		return Vector2f();
}

int hero::isCounteracting()
{
    return fabs(_counteract) > 5.f && fabs(_counteract) < 30.f && _hasCounteractedCooldown == 0.f ? (int)(_counteract/fabs(_counteract)) : 0;
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

int hero::GetHooked()
{
	return _hooked;
}

Vector2f hero::GetSpeed()
{
	return _speed;
}

int hero::GetSigneSpeed()
{
	return _speed.x == 0.f ? 0 : (int)(_speed.x / fabs(_speed.x));
}

bool hero::IsDashing()
{
	return _dash > 4.f;
}

bool hero::IsStunned()
{
	return fabs(_stunned) > 0.f;
}

bool hero::IsAiming()
{
	return _isAiming;
}

bool hero::IsCounteractingProjectile()
{
	return _isCounteractingProjectile > 0.f;
}

bool hero::OnPlatform()
{
	return _collisionType == Box2DTools::PLATFORM_ENTITY || _onBridge;
}

bool hero::OnLadder()
{
	RectangleShape heroShape(Vector2f(20, 35));
	heroShape.setOrigin(10, 18);
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
			return true;
	}
	return false;
}

bool hero::ClimbingLadder()
{
	return _canClimb > 0.f;
}

void hero::HasCounteracted(projectileObj * projectile)
{
	if (projectile != NULL)
	{
		//Initialization
		_hasCounteractedCooldown = 10.f;
		_isCounteractingProjectile = 80.f;
		_counteractedProjectile = projectile;

		//Effect
		if (_shieldEffect != NULL)
		{
			_shieldEffect->setColor(Color::Transparent);
			_shieldEffect = NULL;
		}
		_effectBox->FlashScreen(8.f);
		_effectBox->ShakeScreen(15.f, 5.f);
		_effectBox->AddCircleShape(drawOption(projectile->GetPosition(), Vector2f(10, 10), Vector2f(10, 10), Color::Transparent, 0.f,
			255, Color::White, 1), fadeOption(Vector2f(0, 0), 20.f, 0.f, 100.f, 0.f, Vector2f(1.1f, 1.1f)), physics(false));
		_grindEffect = _effectBox->AddAnimation(T_GRIND, 0, 0.25f, false, drawOption(projectile->GetPosition()),
			fadeOption(Vector2f(0, 0), 0.f, 0.f, _isCounteractingProjectile, 0.f, Vector2f(0, 0), 0.f, false), physics(false), 0);

		//Positioning hero body
		Vector2f pSpeed = projectile->GetDirection();
		float heroAngle = utils::GetVectorRotation(utils::RotateVector(-projectile->GetDirection(), 90.f));
		Vector2f speedRatio;
		speedRatio.x = fabs(pSpeed.x) > fabs(pSpeed.y) ? pSpeed.x / fabs(pSpeed.x) : pSpeed.x / fabs(pSpeed.y);
		speedRatio.y = fabs(pSpeed.y) > fabs(pSpeed.x) ? pSpeed.y / fabs(pSpeed.y) : pSpeed.y / fabs(pSpeed.x);
		_body->SetTransform(b2Vec2(projectile->GetBody()->GetPosition().x + (speedRatio.x * 25.f) / SCALE,
			projectile->GetBody()->GetPosition().y + (speedRatio.y * 25.f) / SCALE), utils::DegreeToRadian(heroAngle));
		_body->SetGravityScale(0.f);
	}
}

void hero::SetStunned(Vector2f stunDirection, Vector2f contactPos, int stunDuration, b2Body* projectileOrigin)
{
    if(_stunned == 0 && _invincibilityFrame == 0 && _life > 0)
    {
        //Stun
        _life -= _life > 0 ? 1 : 0;
        _stunned = stunDuration;
		_stunDirection = stunDirection;
        _invincibilityFrame = (float)stunDuration*5.f;
		_attacking = 0.f;
		_attackingCounteract = 0.f;
		_isCounteractingProjectile = 0.f;
		_flashFrame = 0.f;
		_dash = 0.f;
		_canWallJump = 0.f;
		_canClimb = 0.f;
		_hooked = 0;
		_body->SetGravityScale(1.f);

		//Cancel counteract
		if (_isCounteractingProjectile > 0.f)
			PerformCounteract(false);
		if (_shieldEffect != NULL)
		{
			_shieldEffect->setColor(Color::Transparent);
			_shieldEffect = NULL;
		}
		if (_grindEffect != NULL)
		{
			_grindEffect->setColor(Color::Transparent);
			_grindEffect = NULL;
		}

        //Score
        if(_life == 0 && projectileOrigin != NULL)
        {
            if(projectileOrigin != _body) //If the b2Body who threw the projectile isn't my b2body (so isn't me)
            {
				_scoreManager->AddPoints(-1, _body);
				_scoreManager->AddPoints(1, projectileOrigin);
            }
            else //suicide :(
				_scoreManager->AddPoints(-2, _body);
        }

		//Hit effect
		/*_effectBox->AddRectangleShape(drawOption(_position, Vector2f(40, 40), Vector2f(20, 20), Color::Transparent, 45.f,
			255, Color::White, 1), fadeOption(Vector2f(0, 0), 15.f, 5.f, 100.f, 0.f, Vector2f(1.00001f, 1.00001f)), physics(false));
		float sfxRotation = utils::GetVectorRotation(contactPos, _position, 0) + 180;
		_hitRelativePos = Vector2f(_position.x - contactPos.x, _position.y - contactPos.y);
		_hitEffect = _effectBox->AddAnimation(T_HIT, 1, 0.3, false, drawOption(contactPos, Vector2f(48, 48), sfxRotation), fadeOption(0), physics(false));
		_hitEffect->setPosition(Vector2f(_position.x + _hitRelativePos.x, _position.y + _hitRelativePos.y*0.8f));
		_hitEffect->setRotation(utils::GetVectorRotation(_speed) + 90);*/

		//Screen shake
		_effectBox->ShakeScreen(6, 2);
    }
}

void hero::SetHasJumped(bool jumped)
{
	_hasJumped = jumped;
	_hasCounteract = jumped;
}

Vector2f hero::GetInitPos()
{
	return _initPos;
}

Vector2f hero::GetScale()
{
	return _heroSprite->getScale();
}

input * hero::GetInput()
{
	return _input;
}

Texture * hero::GetHeroTexture()
{
	return _heroTexture;
}

Texture * hero::GetHeroTextureFlash()
{
	return _heroTextureFlash;
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
    _animation->PerformFallingScale();
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

float hero::GetStamina()
{
	return _stamina;
}

bool hero::IsInvincible()
{
	return _invincibilityFrame > 0.f;
}

bool hero::IsOnTheFloor()
{
	return _onTheFloor;
}

bool hero::IsCrouching()
{
	return _crouch;
}

void hero::ResetHero(int colorID, int life)
{
	//Variables
	_colorID = colorID;
	_hasJumped = true;
	_life = life;
	_maxLife = life;

	//Animation
	_animation->ReverseSprite(_initPos.x > utils::VIEW_WIDTH / 2.f);

	//Box2D bodies
	if (_body != NULL && _bodyClone != NULL)
	{
		_body->SetActive(true);
		_body->SetTransform(b2Vec2(_initPos.x / SCALE, _initPos.y / SCALE), 0);
		_bodyClone->SetActive(false);
	}

	//Life sprites
	ResetLifeSprites();
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

void hero::ResetLifeSprites()
{
	//Delete
	for (int i = 0; i < _lifeSprite.size(); i++)
		if (_lifeSprite[i] != NULL)
			delete _lifeSprite[i];
	_lifeSprite.clear();

	//Create
	for (int i = 0; i<_life; i++)
	{
		_lifeSprite.push_back(new Sprite());
		_lifeSprite[i]->setTexture(*_lifeStaminaTexture);
		_lifeSprite[i]->setTextureRect(IntRect(0, 0, 14, 14));
		_lifeSprite[i]->setOrigin(7, 7);
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

	//Update position value
	_position = Vector2f(_body->GetPosition().x*SCALE, _body->GetPosition().y*SCALE);
	if (fabs((_lastContactPos.y - 20.f) - (_position.y + 20.f)) > 8.f)
		_lastContactPos.y = _position.y + 20.f;

	Vector2f dustPos;
	switch (effectType)
	{
		case DUST_MOVING:
			_effectBox->AddAnimation(T_DUST, 3, 0.22, _speed.x > 0, drawOption(Vector2f(_position.x, _lastContactPos.y - 20.f)), fadeOption(0), physics(false), -1, true);
			break;
		case DUST_JUMPING:
			if(_onTheFloor && _canClimb == 0.f)
				_effectBox->AddAnimation(T_DUST, 0, 0.16, false, drawOption(Vector2f(_position.x, _lastContactPos.y - 20.f)), fadeOption(0), physics(false), -1, true);
			else
				_effectBox->AddAnimation(T_DUST, 1, 0.16, false, drawOption(Vector2f(_position.x, _position.y)), fadeOption(0), physics(false), -1, true);
			break;
		case DUST_LANDING:
			_effectBox->AddAnimation(T_DUST, 2, 0.16, false, drawOption(Vector2f(_position.x, _lastContactPos.y - 20.f)), fadeOption(0), physics(false), -1, true);
			break;
		case DUST_WALLJUMP:
			dustPos = Vector2f(_position.x, _position.y - 14.f);
			dustPos.x += _canWallJump > 0.f ? 6.f : -10.f;
			_effectBox->AddAnimation(T_DUST, 3, 0.16, _canWallJump > 0.f, drawOption(dustPos, Vector2f(16, 32), 
				_canWallJump > 0.f ? 90.f : 270.f), fadeOption(0), physics(false), -1, true);
			break;
		case DUST_WALLJUMP_LEAVE:
			dustPos = Vector2f(_position.x, _position.y - 10.f);
			dustPos.x += _canWallJump > 0.f ? 2.f : -6.f;
			_effectBox->AddAnimation(T_DUST, 1, 0.20, _canWallJump > 0.f, drawOption(dustPos, Vector2f(16, 32),
				_canWallJump > 0.f ? 90.f : 270.f), fadeOption(0), physics(false), -1, true);
			break;
		case DUST_CROUCHING:
			_effectBox->AddDust(Vector2f(_position.x - 15, _position.y + 20), true, false, 0.6, true);
			_effectBox->AddDust(Vector2f(_position.x, _position.y + 20), false, false, 0.7, true);
			_effectBox->AddDust(Vector2f(_position.x + 15, _position.y + 20), false, false, 0.6, true);
			break;
		case DASH_TRAIL:
			_dashEffectFrame = 1.f;
			_effectBox->AddRectangleShape(
				drawOption(Vector2f(_position.x, _position.y - 5.f), Vector2f(6, 6), Vector2f(3, 3),
				Color(0, 0, 0, 0), 45, 255, Color(255, 255, 255), 2),
				fadeOption(Vector2f(0, 0), 10.f, 7.f, 50),
				physics(false), true);
			break;
		case DASH_STAR:
			Vector2f pos = Vector2f(_position.x - _speed.x, _position.y - _speed.y);
			Vector2f dirs[4] = { Vector2f(0.5,0.5), Vector2f(-0.5,0.5), Vector2f(-0.5,-0.5), Vector2f(0.5,-0.5) };
			Color colors[4] = { Color(255,0,0), Color(200,0,0), Color(150,0,0), Color(100,0,0) };
			for(int i=0 ; i<=4 ; i++)
				_effectBox->AddRectangleShape(
					drawOption(pos, Vector2f(6, 6), Vector2f(0, 0), Color(0, 0, 0, 0), i*90+45, 255, colors[i], 2),
					fadeOption(dirs[i], 10.f, 0, 65, 0, Vector2f(0.05, 0.05)),
					physics(false), true);
			break;
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

void hero::DrawHUD(RenderWindow * window)
{
	//Drawing HUD
	if(_isCounteractingProjectile == 0.f && !_isAiming)
		_playerHUD->Display(window);
}

void hero::Display(RenderWindow* window, float deltaTime)
{
    _deltaTime = deltaTime;

	//Hero is dead
    if(_life == 0 && _stunned == 0) { return; }

	//Drawing hero main sprite
	utils::DrawLoopSprite(window, _heroSprite);

	//Flash effect
	if (_flashFrame > 0 && _heroSpriteFlash != NULL)
	{
		utils::CopySprite(_heroSpriteFlash, _heroSprite, _heroTextureFlash, true);
		_heroSpriteFlash->setColor(Color(255, 255, 255, (_flashFrame > 8.f ? 255 : 30 * _flashFrame)));
		utils::DrawLoopSprite(window, _heroSpriteFlash);
	}

	//Drawing aiming arrow
	if (_isAiming || _isCounteractingProjectile > 0.f)
		utils::DrawLoopSprite(window, _aimArrow);

	/*RectangleShape bodyShape = RectangleShape(Vector2f(20.f, 3.f));
	bodyShape.setFillColor(Color::Red);
	bodyShape.setOrigin(10.f, 1.5f);
	bodyShape.setPosition(_position.x, _position.y + 17.5f);
	window->draw(bodyShape);*/
}
