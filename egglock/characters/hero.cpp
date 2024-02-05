#include "hero.hpp"

static const float SCALE = 30.f;
static const float WALKING_SPEED = 10.f;
static const float RUNNING_SPEED = 15.f;
static const float FALLING_SPEED = 20.f;

static const int GROUND_ENTITY = 0;
static const int PLATFORM_ENTITY = 1;
static const int PLATFORM2_ENTITY = 1;
static const int BODY_ENTITY = 2;
static const int PARTICLE_ENTITY = 3;

using namespace std;
using namespace sf;

hero::hero()
{
}

hero::~hero()
{
    _world->DestroyBody(_body);
}

hero::hero(int wizardType, Vector2i pos, b2World* world, input* input, effects* effectBox, spells* spellBox,
           ladder* ladderBox, sound* soundBox, scoreManager* scoreManager, int life)
{
    //Variable
    _onTheFloor = false;
    _lastOnTheFloor = false;
    _hasJumped = false;
    _hasRoll = false;
    _hasAttacked = false;
    _showDust = true;
    _stunned = false;
    _life = life;
    _maxLife = life;
    _jumpTolerance = 0;
    _canWallJump = 0;
    _rolling = 0;
    _rollingCooldown = 0;
    _hasCounteractedCoolDown = 0;
    _attacking = 0;
    _attackingCooldown = 0;
    _attackingAnimation = 0;
    _hooked = 0;
    _canClimb = 0;
    _frameDust = 0;
    _collisionType = -1;
    _dustFrame = 0;
    _ladderTop = 0;
    _stunned = 0;
    _invincibilityFrame = 0;
    _selfSpellInvincible = 0;
    _effectAnglePosition = 0;
    _wizardType = wizardType;
    _deltaTime = 1;

    //Vectors
    _initPos = pos;
    _speed = Vector2f(0.f, 0.f); //In Box2D unit
    _maxSpeed = Vector2f(RUNNING_SPEED, FALLING_SPEED);
    _position = Vector2f(pos.x, pos.y); //In pixel unit
    _positionClone = Vector2f(pos.x, pos.y); //In pixel unit
    _hookedPoint = Vector2f(0,0);
    _ladderPos = Vector2f(0,0);
    _stunDirection = Vector2f(0,0);

    //Objects
    _input = input;
    _effectBox = effectBox;
    _spellBox = spellBox;
    _ladderBox = ladderBox;
    _soundBox = soundBox;
    _wallSlidingSound = NULL;
    _scoreManager = scoreManager;

    //Box2D
    _world = world;
    _body = Box2DTools::CreateRectangleBox(_world, pos, 20, 35, 0.1, 0, 0, 4);
    _bodyClone = Box2DTools::CreateRectangleBox(_world, pos, 20, 35, 0.1, 0, 0, 4);
    _bodyClone->SetActive(false);

    //Sprites
    LoadWizardTexture(wizardType);
    if (!_lifeTexture.loadFromFile(_files.life))
    {
        cout << "Unable to find life texture. Stopping execution." << endl;
        exit(1);
    }
    else
        ResetLifeSprite();

    //Reverse sprite with initial position
    if(pos.x > 450)
        _animation.ReverseSprite(true);
}

void hero::LoadWizardTexture(int wizardNumber)
{
    //Sprite
    if (_wizardType == 0 && !_texture.loadFromFile(_files.wizardBlueTile))
    {
        cout << "Unable to find hero texture. Stopping execution." << endl;
        exit(1);
    }
    else if (_wizardType == 1 && !_texture.loadFromFile(_files.wizardRedTile))
    {
        cout << "Unable to find hero texture. Stopping execution." << endl;
        exit(1);
    }
    else if (_wizardType == 2 && !_texture.loadFromFile(_files.wizardGreenTile))
    {
        cout << "Unable to find hero texture. Stopping execution." << endl;
        exit(1);
    }
    else if (_wizardType == 3 && !_texture.loadFromFile(_files.wizardPurpleTile))
    {
        cout << "Unable to find hero texture. Stopping execution." << endl;
        exit(1);
    }
    else if (_wizardType == 4 && !_texture.loadFromFile(_files.wizardWhiteTile))
    {
        cout << "Unable to find hero texture. Stopping execution." << endl;
        exit(1);
    }
    else if(_wizardType >= 0 && _wizardType <= 4)
    {
        _sprite.setTexture(_texture);
        _sprite.setOrigin(40, 84);
        _spriteClone.setTexture(_texture);
        _spriteClone.setOrigin(40, 84);
        _spellEffect = NULL;
        _animation = animation(&_sprite, 0.1, 84, 84, 6, 18, 0);
    }
    else
    {
        cout << "Not a correct wizard number" << endl;
        exit(1);
    }
}

void hero::UpdateVariables()
{
    if(_life == 0 && _stunned == 0) //Waiting to respawn hero
    {
        if(_invincibilityFrame > 0)
        {
            _invincibilityFrame --;
            return;
        }
        else //Respawn
        {
            _life = _maxLife;
            _invincibilityFrame = 60;
            _body->SetActive(true); //= Box2DTools::CreateRectangleBox(_world, _initPos, 20, 35, 0.1, 0, 0, 4);
            _body->SetTransform(b2Vec2(_initPos.x/SCALE, _initPos.y/SCALE),0);
            _animation.ReverseSprite(_initPos.x > 450);
        }
    }

    //Body speed + position
    _speed = Vector2f(_body->GetLinearVelocity().x, _body->GetLinearVelocity().y);
    _position = Vector2f(_body->GetPosition().x*SCALE, _body->GetPosition().y*SCALE);
    _positionClone = Vector2f(_bodyClone->GetPosition().x*SCALE, _bodyClone->GetPosition().y*SCALE);

    //Wall jump duration
    if(_canWallJump > 0)
        _canWallJump --;
    else if(_canWallJump < 0)
        _canWallJump++;

    //Testing if body on the floor
    _lastOnTheFloor = _onTheFloor;
    _onTheFloor = false;
    for(double i=_position.x-10.3 ; i<_position.x+10.3 ; i+=0.1)
    {
        b2Body* body;
        if(Box2DTools::TestCollisionPoint(_world, b2Vec2(i/SCALE, (_position.y+18)/SCALE), true,
                                          &_collisionType, &body) && _speed.y >= -0.1)
        {
            _onTheFloor = true;
            _canWallJump = 0;

            //Bounce on other wizards' head
            if(body != _body && body != _bodyClone && isAWizard(body) >= 0)
            {
                _soundBox->PlaySound(8, false);
                _wizards[isAWizard(body)]->CrushWizard();
                _body->SetLinearVelocity(b2Vec2(_speed.x, -15));
                _speed.y = -10;
            }
            break;
        }
    }
    if(!_onTheFloor)
        _collisionType = -1;

    //Jump tolerance - Permit the hero to jump during some frames after leaving the floor
    if(_onTheFloor)
        _jumpTolerance = 6/(_deltaTime != 0 ? _deltaTime :1);
    else if(_jumpTolerance > 0)
        _jumpTolerance --;

    //Dust of moving
    if(_dustFrame > 3)
        _dustFrame = 0;
    _dustFrame += _canWallJump ? fabs(_speed.y) : fabs(_speed.x);

    //Show the falling and jumping dust if there's a collision in the middle of the character
    _showDust = Box2DTools::TestCollisionPoint(_world, b2Vec2(_position.x/SCALE, (_position.y+18)/SCALE), true, &_collisionType);

    //Testing if body can wall jump
    if(!_onTheFloor && _hooked == 0 && _input->isKeyPressed("LEFT") &&
       Box2DTools::TestCollisionPoint(_world, b2Vec2((_position.x-11)/SCALE, (_position.y-14)/SCALE), false, &_collisionType) &&
       Box2DTools::TestCollisionPoint(_world, b2Vec2((_position.x-11)/SCALE, (_position.y+14)/SCALE), false, &_collisionType))
       _canWallJump = 15;
    else if(!_onTheFloor && _hooked == 0 && _input->isKeyPressed("RIGHT") &&
       Box2DTools::TestCollisionPoint(_world, b2Vec2((_position.x+11)/SCALE, (_position.y-14)/SCALE), false, &_collisionType) &&
       Box2DTools::TestCollisionPoint(_world, b2Vec2((_position.x+11)/SCALE, (_position.y+14)/SCALE), false, &_collisionType))
       _canWallJump = -15;

    /*if (_canWallJump > 5)
        _body->SetTransform(b2Vec2((_position.x+3)/SCALE, _position.y/SCALE), 0);
    else if (_canWallJump < -5)
        _body->SetTransform(b2Vec2((_position.x-3)/SCALE, _position.y/SCALE), 0);*/

    //Testing if body can hook to a wall
    if(!_onTheFloor && _speed.y >= -0.1)
    {
        b2Vec2 posLeft = b2Vec2(_body->GetPosition().x-(11/SCALE), _body->GetPosition().y-(14/SCALE));
        b2Vec2 posRight = b2Vec2(_body->GetPosition().x+(11/SCALE), _body->GetPosition().y-(14/SCALE));
        b2Vec2 posGround = b2Vec2(_body->GetPosition().x, _body->GetPosition().y+(26/SCALE));
        if(!Box2DTools::TestCollisionPoint(_world, posGround, true, &_collisionType))
        {
            //Getting hook coordinate if exist
            _hooked = Box2DTools::TestHookPoint(_world, posLeft, &_hookedPoint, _animation.GetReversed());
            if(_hooked == 0)
                _hooked = Box2DTools::TestHookPoint(_world, posRight, &_hookedPoint, _animation.GetReversed());

            //Testing if area above hook point is free
            b2Vec2 hook = b2Vec2(_hookedPoint.x/SCALE, _hookedPoint.y/SCALE);
            bool areaAboveFree = !Box2DTools::TestCollisionPoint(_world, hook, true, &_collisionType);

            //Performing hook
            if(_hooked != 0 && _speed.y >= 0)// && areaAboveFree)
            {
                _canWallJump = 0;
                _body->SetLinearVelocity(b2Vec2(0,0));
                _body->SetTransform(b2Vec2(_hookedPoint.x + ((_hooked > 0 ? 11 : -11)/SCALE), _hookedPoint.y+(14/SCALE)), 0);
                _speed = Vector2f(_body->GetLinearVelocity().x, _body->GetLinearVelocity().y);
                _body->ApplyForce(b2Vec2(0.f,-42*_body->GetMass()), _body->GetWorldCenter(), true);
            }
        }
    }
    else
    {
        _body->ApplyForce(b2Vec2(0.f,0*_body->GetMass()), _body->GetWorldCenter(), true);
        _hooked = 0;
    }

    //Testing if hero can climb to a ladder
    if(_canWallJump == 0 && _speed.y > -10 &&
      (!_onTheFloor || (_onTheFloor && fabs(_speed.x)*SCALE < WALKING_SPEED)))
    {
        if(_canClimb == 10 || (_canClimb != 10 && _input->isKeyPressed("UP")))
        {
            RectangleShape shape(Vector2f(20,35));
            shape.setOrigin(10,18);
            shape.setPosition(_position);
            _canClimb = _ladderBox->TestCollision(&shape, &_ladderPos, &_ladderTop)*10;
        }
        else if(_canClimb > 0)
            _canClimb --;
    }
    else if(_canClimb > 0)
            _canClimb --;

    if(_canClimb == 10 && _position.x != _ladderPos.x)
    {
        double gapX = fabs(_ladderPos.x - _position.x)/5;
        double newPosX = _position.x > _ladderPos.x ? _position.x - gapX : _position.x + gapX;
        _body->SetTransform(b2Vec2(newPosX/SCALE, _position.y/SCALE), 0);
    }

    //Rolling duration
    if(_rolling != 0)
        _rolling += _rolling < 0 ? 1 : -1;
    if(_rollingCooldown != 0)
        _rollingCooldown += _rollingCooldown < 0 ? 1 : -1;
    if(_hasCounteractedCoolDown != 0)
        _hasCounteractedCoolDown += _hasCounteractedCoolDown < 0 ? 1 : -1;

    //Changing hero collision mask after countering duration is over
    if(fabs(_rolling) == 10)
    {
        _world->DestroyBody(_body);
        _body = Box2DTools::CreateRectangleBox(_world, Vector2i(0,0), 20, 35, 0.1, 0, 0, 4);
        _body->SetTransform(b2Vec2(_position.x/SCALE, _position.y/SCALE), 0);
    }

    //Attack duration
    if(_attacking != 0)
        _attacking += _attacking < 0 ? 1 : -1;
    if(_attackingCooldown != 0)
        _attackingCooldown += _attackingCooldown < 0 ? 1 : -1;

    //Direction if stunned
    if(_stunned > 0)
    {
        if((_hooked != 0 ||_canClimb != 0) && fabs(_stunDirection.y) <= 2)
        {
            _stunDirection.x = 0;
            _stunDirection.y = 10;
            _body->ApplyForce(b2Vec2(0.f,42*_body->GetMass()), _body->GetWorldCenter(), true);
        }

        _hooked = 0;
        _canClimb = 0;
        _canWallJump = 0;
        _body->SetLinearVelocity(b2Vec2(_stunDirection.x, _stunDirection.y));
    }

    //Stun duration
    if(_stunned > 0)
        _stunned--;

    //Invincibility duration
    if(_invincibilityFrame > 0)
        _invincibilityFrame --;

    //Limiting running speed
    if(_body->GetLinearVelocity().x > _maxSpeed.x)
        _body->SetLinearVelocity(b2Vec2(_body->GetLinearVelocity().x-1.f*_deltaTime, _body->GetLinearVelocity().y));
    else if(_body->GetLinearVelocity().x < -_maxSpeed.x)
        _body->SetLinearVelocity(b2Vec2(_body->GetLinearVelocity().x+1.f*_deltaTime, _body->GetLinearVelocity().y));

    //Limiting falling speed
    if(_body->GetLinearVelocity().y > _maxSpeed.y && _canClimb != 10)
        _body->SetLinearVelocity(b2Vec2(_body->GetLinearVelocity().x, _maxSpeed.y));

    //Body speed + position (re-update)
    _speed = Vector2f(_body->GetLinearVelocity().x, _body->GetLinearVelocity().y);
    _position = Vector2f(_body->GetPosition().x*SCALE, _body->GetPosition().y*SCALE);

    if(_life == 0 && _stunned == 1) //Killing hero if running out of life
    {
        _position = _sprite.getPosition();
        _effectBox->AddEffect(Vector2i(_position.x, _position.y-25), 0.20, 68, 100, 9, 0, _animation.GetReversed(), 2);
        _soundBox->PlaySound(11, false);
        _body->SetLinearVelocity(b2Vec2(0,0));
        _body->SetActive(false);
        _bodyClone->SetActive(false);
        _stunned = 0;
        _stunDirection = Vector2f(0,0);
    }
}

void hero::PerformEffects()
{
    //Animation of falling - crush sprite + dust
    if(!_lastOnTheFloor && _onTheFloor && _collisionType == GROUND_ENTITY)
    {
        _animation.PerformFallingScale();
        if(_showDust)
        {
            _soundBox->PlaySound(8, false);
            _effectBox->AddDust(Vector2f(_position.x-10, _position.y+20), true, false);
            _effectBox->AddDust(Vector2f(_position.x-10, _position.y+20), true, false);
            _effectBox->AddDust(Vector2f(_position.x-10, _position.y+20), true, false);
            _effectBox->AddDust(Vector2f(_position.x+10, _position.y+20), false, false);
            _effectBox->AddDust(Vector2f(_position.x+10, _position.y+20), false, false);
            _effectBox->AddDust(Vector2f(_position.x+10, _position.y+20), false, false);
        }
    }

    //Dust of jumping
    if(_lastOnTheFloor && !_onTheFloor && _hasJumped && _collisionType == GROUND_ENTITY)
    {
        _animation.PerformJumpingScale();
        _effectBox->AddDust(Vector2f(_position.x-10, _position.y+20), true, false);
        _effectBox->AddDust(Vector2f(_position.x-10, _position.y+20), true, false);
        _effectBox->AddDust(Vector2f(_position.x-10, _position.y+20), true, false);
        _effectBox->AddDust(Vector2f(_position.x+10, _position.y+20), false, false);
        _effectBox->AddDust(Vector2f(_position.x+10, _position.y+20), false, false);
        _effectBox->AddDust(Vector2f(_position.x+10, _position.y+20), false, false);
    }

    //Dust moving
    if(fabs(_speed.x) > 2 && _onTheFloor && _canWallJump == 0 && /*_frameDust == 0 && */fabs(_speed.y) < 0.1 &&
       _collisionType == GROUND_ENTITY && _dustFrame > 3)
    {
        int pos = _speed.x > 0 ? -10 : 10;
        _effectBox->AddDust(Vector2f(_position.x+pos, _position.y+15), _speed.x > 0, true);
    }

    //Dust wall sliding
    if(fabs(_canWallJump) > 0 && !_onTheFloor && _speed.y > 1 && _dustFrame > 3)//_frameDust == 0)
    {
        _frameDust = 4;
        int pos = _canWallJump > 0 ? -10 : 10;
        _effectBox->AddDust(Vector2f(_position.x+pos, _position.y+8), _animation.GetReversed(), true);
        _effectBox->AddDust(Vector2f(_position.x+pos, _position.y-20), _animation.GetReversed(), true);
    }

    //Wall sliding sound
    if(_canWallJump != 0 && _wallSlidingSound == NULL)
        _wallSlidingSound = _soundBox->PlaySound(10, true);
    else if(_canWallJump == 0 && _wallSlidingSound != NULL)
    {
        _wallSlidingSound->stop();
        _wallSlidingSound = NULL;
    }

    //Clone of the sprite, if loop horizontal
    if(_position.x-13 <= 0)
    {
        _spriteClone.setPosition(800+_position.x, _position.y+17);
        _bodyClone->SetActive(true);
        _bodyClone->SetTransform(b2Vec2((800+_position.x)/SCALE, _position.y/SCALE),0);
    }
    else if(_position.x+13 >= 800)
    {
        _spriteClone.setPosition(_position.x-800, _position.y+17);
        _bodyClone->SetActive(true);
        _bodyClone->SetTransform(b2Vec2((_position.x-800)/SCALE, _position.y/SCALE),0);
    }
    else if(_position.y-30 <= 0)
    {
        _spriteClone.setPosition(_position.x, _position.y+617);
        _bodyClone->SetActive(true);
        _bodyClone->SetTransform(b2Vec2(_position.x/SCALE, (_position.y+600)/SCALE),0);
    }
    else if(_position.y+30 >= 600)
    {
        _spriteClone.setPosition(_position.x, _position.y-583);
        _bodyClone->SetActive(true);
        _bodyClone->SetTransform(b2Vec2(_position.x/SCALE, (_position.y-600)/SCALE),0);
    }

    //Disabling collision of clone
    if(_position.x-13 > 0 && _position.x+13 < 800 && _position.y-30 > 0 && _position.y+30 < 600)
    {
        _bodyClone->SetTransform(b2Vec2(0,0),0);
        _bodyClone->SetActive(false);
    }

    //Counter
    if(_frameDust > 0)
        _frameDust--;

    //Self spell invincible duration
    if(_selfSpellInvincible > 0)
        _selfSpellInvincible--;
}

void hero::Move()
{
    //Unlock keys at each frame
    _input->PerformUnlockKeys();

    //Setting acceleration/deceleration
    float deltaAdjust = fabs(_deltaTime - 1) < 0.1 ? 1 : _deltaTime;
    int scalingSpeed = (!_onTheFloor || _rolling != 0 || _canClimb != 0) ? 1.f*deltaAdjust : 1.5*deltaAdjust;
    float scalingBrake = _rolling != 0 || _attacking != 0 ? 0.5*deltaAdjust: 1.f*deltaAdjust;

    //Running
    if(_input->isKeyPressed("RUN") || _rolling != 0)
        _maxSpeed.x = RUNNING_SPEED;
    else if(_onTheFloor || _canClimb > 0)
        _maxSpeed.x = WALKING_SPEED;

    //Rolling
    if(_input->isKeyPressed("ROLL") && _attacking == 0 &&
       _rolling == 0 && _rollingCooldown == 0 &&
       !_hasRoll && _canClimb != 10)
    {
        //Changing body collision mask
        _world->DestroyBody(_body);
        _body = Box2DTools::CreateRectangleBox(_world, Vector2i(0,0), 32, 35, 0.1, 0, 0, 4);
        _body->SetTransform(b2Vec2(_position.x/SCALE, _position.y/SCALE), 0);

        int signe = _animation.GetReversed() ? -1 : 1;
        _rolling = (signe * 22)/(_deltaTime != 0 ? _deltaTime : 1);
        _rollingCooldown = (signe * 50)/(_deltaTime != 0 ? _deltaTime : 0);
        if(_speed.y >= 0)
        {
            _body->SetLinearVelocity(b2Vec2(_speed.x, 0));
            _speed.y = 0;
        }
        _soundBox->PlaySound(7, false);
    }

    //Locking rolling key if pressed once
    if(_input->isKeyPressed("ROLL"))
        _hasRoll = true;
    else
        _hasRoll = false;

    //{Attacking
    if(_input->isKeyPressed("ATTACK") && _rolling == 0 &&
       _attacking == 0 && _attackingCooldown == 0 &&
       !_hasAttacked && _canClimb != 10 && _hooked == 0)
    {
        int signe = _animation.GetReversed() ? -1 : 1;
        _attacking = (signe*22)/_deltaTime;
        _attackingCooldown = (signe*36)/_deltaTime;

        Vector2f position;
        if(_position.x-13 <= 0 && _animation.GetReversed())
            position = Vector2f(800+_position.x, _position.y);
        else if(_position.x+13 >= 800 && !_animation.GetReversed())
            position = Vector2f(_position.x-800, _position.y);
        else if(_position.y-30 <= 0 && _input->isKeyPressed("UP"))
            position = Vector2f(_position.x, _position.y+600);
        else if(_position.y+30 >= 600 && _input->isKeyPressed("DOWN"))
            position = Vector2f(_position.x, _position.y-600);
        else
            position = _position;

        //Spell direction
        Vector2i spellPos;
        Vector2f spellSpeed;
        int spellAngle = 0;
        if(!_input->isKeyPressed("LEFT") && !_input->isKeyPressed("RIGHT"))
        {
            if(_input->isKeyPressed("UP"))
            {
                spellPos = Vector2i(position.x, position.y-45-_speed.y);
                spellSpeed = Vector2f(0, -15);
                spellAngle = 90;
                _spellEffect = _effectBox->AddEffect(spellPos, 0.3, 75, 75, 6, _wizardType*2+1, true, 1);
                _spellEffect->setRotation(90);
                _effectAnglePosition = 3;
                _attackingAnimation = 3;
            }
            else if(_input->isKeyPressed("DOWN") && !_onTheFloor)
            {
                spellPos = Vector2i(position.x, position.y+45+_speed.y);
                spellSpeed = Vector2f(0, 15);
                spellAngle = 90;
                _spellEffect = _effectBox->AddEffect(spellPos, 0.3, 75, 75, 6, _wizardType*2+1, false, 1);
                _spellEffect->setRotation(270);
                _effectAnglePosition = 4;
                _attackingAnimation = 2;
            }
            else
            {
                spellPos = Vector2i(position.x+(signe*(25+fabs(_speed.x))), position.y);
                spellSpeed = Vector2f(fabs(_speed.x) < 10 ? signe*12 : _speed.x + (2*signe), 0);
                _spellEffect = _effectBox->AddEffect(spellPos, 0.3, 75, 75, 6, _wizardType*2+1, _animation.GetReversed(), 1);
                _effectAnglePosition = _animation.GetReversed() ? 1 : 2;
                _attackingAnimation = 1;
            }
        }
        else
        {
            int localSpeed = fabs(_speed.x) < 10 ? signe*12 : _speed.x + (2*signe);
            if(_input->isKeyPressed("UP"))
            {
                spellPos = Vector2i(position.x+(signe*(25+fabs(_speed.x))), position.y-35+_speed.y);
                spellSpeed = Vector2f(localSpeed/1.5, -fabs(localSpeed)/1.5);
                spellAngle = 45;
                _spellEffect = _effectBox->AddEffect(spellPos, 0.3, 75, 75, 6, _wizardType*2+1, _animation.GetReversed(), 1);
                _spellEffect->setRotation(_animation.GetReversed() ? 45 : -45);
                _effectAnglePosition = _animation.GetReversed() ? 5 : 6;
                _attackingAnimation = 4;
            }
            else if(_input->isKeyPressed("DOWN"))
            {
                spellPos = Vector2i(position.x+(signe*(25+fabs(_speed.x))), position.y+35+_speed.y);
                spellSpeed = Vector2f(localSpeed/1.5, fabs(localSpeed)/1.5);
                spellAngle = 45;
                _spellEffect = _effectBox->AddEffect(spellPos, 0.3, 75, 75, 6, _wizardType*2+1, true, 1);
                _spellEffect->setRotation(_animation.GetReversed() ? 315 : 225);
                _effectAnglePosition = _animation.GetReversed() ? 7 : 8;
                _attackingAnimation = 5;
            }
            else
            {
                spellPos = Vector2i(position.x+(signe*(25+fabs(_speed.x))), position.y);
                spellSpeed = Vector2f(localSpeed, 0);
                _spellEffect = _effectBox->AddEffect(spellPos, 0.3, 75, 75, 6, _wizardType*2+1, _animation.GetReversed(), 1);
                _effectAnglePosition = _animation.GetReversed() ? 1 : 2;
                _attackingAnimation = 1;
            }
        }
        //}

        //Creating spell
        _selfSpellInvincible = 5;
        _spellBox->AddSpell(spellPos, 0.3, 35, 25, 6, _wizardType, spellAngle, spellSpeed, GetWizardColor(_wizardType));
        _soundBox->PlaySound(_wizardType+1, false);
        if(_speed.y >= 0)
        {
            _body->SetLinearVelocity(b2Vec2(_speed.x, 0));
            _speed.y = 0;
        }
    }

    //Unhooking
    if((_hooked != 0 && _input->isKeyPressed("DOWN")) ||
       (_hooked < 0 && _input->isKeyPressed("LEFT")) ||
       (_hooked > 0 && _input->isKeyPressed("RIGHT")))
    {
        _hooked = 0;
        _body->ApplyForce(b2Vec2(0.f,42*_body->GetMass()), _body->GetWorldCenter(), true);
        _body->SetLinearVelocity(b2Vec2(_speed.x, 5));
    }

    //Moving
    if(_canClimb > 0 && (_input->isKeyPressed("LEFT") || _input->isKeyPressed("RIGHT")))
    {
        _body->ApplyForce(b2Vec2(0.f,0*_body->GetMass()), _body->GetWorldCenter(), true);
        _canClimb --;
    }
    if(_input->isKeyPressed("LEFT") && _speed.x - scalingSpeed > -_maxSpeed.x && _rolling == 0 && _attacking == 0)
        _body->SetLinearVelocity(b2Vec2(_speed.x - scalingSpeed, _speed.y));
    else if(_input->isKeyPressed("RIGHT") && _speed.x + scalingSpeed < _maxSpeed.x && _rolling == 0 && _attacking == 0)
        _body->SetLinearVelocity(b2Vec2(_speed.x + scalingSpeed, _speed.y));
    else if((!_input->isKeyPressed("LEFT") || _rolling != 0 || _attacking != 0) && _speed.x <= -scalingBrake)
        _body->SetLinearVelocity(b2Vec2(_speed.x+scalingBrake, _speed.y));
    else if((!_input->isKeyPressed("RIGHT") || _rolling != 0 || _attacking != 0) && _speed.x >= scalingBrake)
        _body->SetLinearVelocity(b2Vec2(_speed.x-scalingBrake, _speed.y));
    else if((!_input->isKeyPressed("LEFT") || !_input->isKeyPressed("RIGHT") || _rolling != 0 || _attacking != 0) &&
            fabs(_speed.x) < scalingBrake)
        _body->SetLinearVelocity(b2Vec2(0, _speed.y));

    //{Jumping - Wall Jump
    if(!_hasJumped && _rolling == 0)
    {
        //Jumping - leaving platform
        if(_onTheFloor && (_collisionType == PLATFORM_ENTITY || _collisionType == PLATFORM2_ENTITY) &&
           _input->isKeyPressed("JUMP") && _input->isKeyPressed("DOWN"))
        {
            _onTheFloor = _lastOnTheFloor = false;
            _body->SetTransform(b2Vec2(_body->GetPosition().x, _body->GetPosition().y+0.6), 0);
        }
        else if((_onTheFloor || _canClimb > 0 || _jumpTolerance > 0) && _input->isKeyPressed("JUMP"))
        {
            _body->ApplyForce(b2Vec2(0.f,0*_body->GetMass()), _body->GetWorldCenter(), true);
            _canClimb = 0;
            _body->SetLinearVelocity(b2Vec2(_speed.x, -18-fabs(_speed.x/5)));
            _soundBox->PlaySound(0, false);
        }
        else if(!_onTheFloor && !_input->isKeyPressed("JUMP") && _speed.y < -7)
            _body->SetLinearVelocity(b2Vec2(_speed.x, -7));
        else if(_hooked != 0 && _input->isKeyPressed("JUMP"))
        {
            _body->ApplyForce(b2Vec2(0.f,0*_body->GetMass()), _body->GetWorldCenter(), true);
            _hooked = 0;
            _body->SetLinearVelocity(b2Vec2(_speed.x, -18));
            _soundBox->PlaySound(0, false);
        }

        //Wall jump
        if(_canWallJump != 0 && _input->isKeyPressed("JUMP"))
        {
            if((_input->isKeyPressed("LEFT") && _canWallJump > 0) ||
               (_input->isKeyPressed("RIGHT") && _canWallJump < 0))
                _body->SetLinearVelocity(b2Vec2(_canWallJump > 0 ? 12 : -12, -16));
            else
            {
                _maxSpeed = Vector2f(RUNNING_SPEED, FALLING_SPEED);
                _body->SetLinearVelocity(b2Vec2(_canWallJump > 0 ? 20 : -20, -14));
            }
            _soundBox->PlaySound(0, false);
            _canWallJump = 0;
        }
    }
    //}

    //Climbing on ladders
    if(_canClimb == 10 && !_input->isKeyPressed("LEFT") && !_input->isKeyPressed("RIGHT"))
    {
        if(_input->isKeyPressed("UP") && _position.y-22 > _ladderTop)
        {
            _body->ApplyForce(b2Vec2(0.f,0*_body->GetMass()), _body->GetWorldCenter(), true);
            _body->SetLinearVelocity(b2Vec2(_body->GetLinearVelocity().x, -5));
        }
        else if(_input->isKeyPressed("DOWN"))
        {
            _body->ApplyForce(b2Vec2(0.f,0*_body->GetMass()), _body->GetWorldCenter(), true);
            _body->SetLinearVelocity(b2Vec2(_body->GetLinearVelocity().x, 5));
        }
        else
        {
            _body->ApplyForce(b2Vec2(0.f,-42*_body->GetMass()), _body->GetWorldCenter(), true);
            _body->SetLinearVelocity(b2Vec2(_body->GetLinearVelocity().x, 0));
        }
    }

    //Locking jumping key if pressed once
    if(_input->isKeyPressed("JUMP"))
        _hasJumped = true;
    else
        _hasJumped = false;

    if(fabs(_canWallJump) > 0)
        _maxSpeed.y = 5.f;
    else
        _maxSpeed.y = FALLING_SPEED;
}

void hero::AnimeSprite()
{
    //Animation speed
    if(_attacking != 0)
        _animation.SetFrameDelay(0.45);
    else if((_animation.GetAnimation() == 7 && _maxSpeed.x > WALKING_SPEED+1) || _rolling != 0 || _hooked != 0)
        _animation.SetFrameDelay(0.25);
    else if(_canClimb == 10)
        _animation.SetFrameDelay(0.20);
    else
        _animation.SetFrameDelay(0.15);

    //Reverse sprite
    if(_canWallJump == 0 && _rolling == 0 && _attacking == 0 && _hooked == 0 && _canClimb != 10)
    {
        if(_input->isKeyPressed("LEFT"))
            _animation.ReverseSprite(true);
        else if(_input->isKeyPressed("RIGHT"))
            _animation.ReverseSprite(false);
    }

    //Changing opacity if character invincible
    if(_stunned == 0 && _invincibilityFrame > 0)
        _sprite.setColor(Color(255,255,255,(_invincibilityFrame%3!=0 ? 200 : 100)));
    else
        _sprite.setColor(Color(255,255,255));

    //{All animation
    if(_stunned != 0)
    {
        if(_stunDirection.x > 0.5)
            _animation.ReverseSprite(true);
        else if(_stunDirection.x < -0.5)
            _animation.ReverseSprite(false);
        _animation.SetAnimation(17,0);
    }
    else if(_canClimb == 10)
    {
        _animation.ReverseSprite(false);
        if(_speed.y != 0)
            _animation.SetAnimation(9,0);
        else
            _animation.SetAnimation(10,0);
    }
    else if(_hooked != 0)
    {
        _animation.SetAnimation(8, 4);
        _animation.ReverseSprite(_hooked > 0);
    }
    else if(_attacking != 0)
    {
        if(_spellEffect != NULL)
            _spellEffect->setPosition(GetEffectAnglePosition(_effectAnglePosition));
        if(_canWallJump != 0)
            _animation.SetAnimation(12, 5);
        else if(_attackingAnimation == 1)
            _animation.SetAnimation(11, 5);
        else if(_attackingAnimation == 2)
            _animation.SetAnimation(13, 5);
        else if(_attackingAnimation == 3)
            _animation.SetAnimation(14, 5);
        else if(_attackingAnimation == 4)
            _animation.SetAnimation(15, 5);
        else
            _animation.SetAnimation(16, 5);
        _animation.ReverseSprite(_attacking < 0);
    }
    else if(_rolling != 0)
    {
        _animation.SetAnimation(6, 0);
        _animation.ReverseSprite(_rolling < 0);
    }
    else if(_canWallJump != 0)
    {
        _animation.ReverseSprite(_canWallJump < 0);
        _animation.SetAnimation(5, 0);
    }
    else if(!_onTheFloor && _speed.y < 0)
        _animation.SetAnimation(2, 2);
    else if(!_onTheFloor && _speed.y >= 0)
        _animation.SetAnimation(3, 3);
    else if(_onTheFloor && _input->isKeyPressed("LEFT") && _speed.x > 0.1)
        _animation.SetAnimation(4, 0);
    else if(_onTheFloor && _input->isKeyPressed("RIGHT") && _speed.x < -0.1)
        _animation.SetAnimation(4, 0);
    else if(fabs(_speed.x) > 0.5)
        _animation.SetAnimation(fabs(_speed.x) > WALKING_SPEED+1 ? 7 : 1, 0);
    else
        _animation.SetAnimation(0, 0);
    //}

    //Perform animation + position + drawing
    _animation.PerformAnimation(_deltaTime);
    _sprite.setPosition(_position.x, _position.y+17);

    //Perform sprite rotation
    if(_onTheFloor && _animation.GetAnimation() != 4)
        _sprite.setRotation(_speed.x/2);
    else
        _sprite.setRotation(0);
}

b2Body* hero::GetBodyA()
{
    return _body;
}

b2Body* hero::GetBodyB()
{
    return _bodyClone->IsActive() ? _bodyClone : _body;
}

Vector2f hero::GetPosition()
{
    return Vector2f(_body->GetPosition().x*SCALE, _body->GetPosition().y*SCALE);
}

int hero::isCounteracting()
{
    return fabs(_rolling) > 10 && _hasCounteractedCoolDown == 0 ? _rolling/fabs(_rolling) : 0;
}

bool hero::isSelfSpellInvincible()
{
    return _selfSpellInvincible > 0;
}

void hero::hasCounteracted()
{
    _hasCounteractedCoolDown = 10;
}

void hero::SetStunned(Vector2f stunDirection, int stunDuration, int numWizardOfSpell)
{
    if(_stunned == 0 && _invincibilityFrame == 0 && _life > 0)
    {
        //Stun
        _life -= _life > 0 ? 1 : 0;
        _stunned = stunDuration;
        _stunDirection = stunDirection;
        _invincibilityFrame = stunDuration*5;

        //Score
        if(_life == 0)
        {
            if(numWizardOfSpell != _wizardType)
            {
                _scoreManager->AddPoints(-1, _wizardType);
                _scoreManager->AddPoints(1, numWizardOfSpell);
            }
            else
                _scoreManager->AddPoints(-2, _wizardType);
        }
    }
}

Vector2f hero::GetEffectAnglePosition(int i)
{
    Vector2f pos = Vector2f(_body->GetPosition().x*SCALE, _body->GetPosition().y*SCALE);
    switch(i)
    {
        case 1: //LEFT
            return Vector2f(pos.x-20, pos.y);
        case 2: //RIGHT
            return Vector2f(pos.x+20, pos.y);
        case 3: //UP
            return Vector2f(pos.x, pos.y-30);
        case 4: //DOWN
            return Vector2f(pos.x, pos.y+10);
        case 5: //LEFT-UP
            return Vector2f(pos.x-25, pos.y-30);
        case 6: //RIGHT-UP
            return Vector2f(pos.x+25, pos.y-30);
        case 7: //LEFT-DOWN
            return Vector2f(pos.x-25, pos.y+25);
        case 8: //RIGHT-DOWN
            return Vector2f(pos.x+25, pos.y+25);
        default:
            return Vector2f(0, 0);
    }
}

int hero::GetWizardType()
{
    return _wizardType;
}

Color hero::GetWizardColor(int i)
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

void hero::CrushWizard()
{
    _animation.PerformFallingScale();
}

int hero::isAWizard(b2Body* body)
{
    for(int i=0 ; i<_wizards.size() ; i++)
    {
        if(_wizards[i]->GetBodyA() == body || _wizards[i]->GetBodyB() == body)
            return i;
    }
    return -1;
}

int hero::getLife()
{
    return _life;
}

void hero::Step()
{
    UpdateVariables();
    if(_life == 0 && _stunned == 0) { return; } //Hero is dead
    if(_stunned == 0 && _life > 0) { Move(); }
    PerformEffects();
    AnimeSprite();
}

void hero::ResetHero(int numColor, int life)
{
    _wizardType = numColor;
    LoadWizardTexture(_wizardType);
    _hasJumped = true;
    _life = life;
    ResetLifeSprite();
    _maxLife = life;
    _body->SetActive(true);
    _body->SetTransform(b2Vec2(_initPos.x/SCALE, _initPos.y/SCALE),0);
    _bodyClone->SetActive(false);
    _animation.ReverseSprite(_initPos.x > 450);
}

void hero::ResetLifeSprite()
{
    //Delete
    for(int i=0 ; i<_lifeSprite.size() ; i++)
    {
        delete _lifeSprite[i];
        delete _lifeSpriteClone[i];
    }
    _lifeSprite.clear();
    _lifeSpriteClone.clear();

    //Recreate
    for(int i=0 ; i<_life ; i++)
    {
        _lifeSprite.push_back(new Sprite());
        _lifeSprite[_lifeSprite.size()-1]->setTexture(_lifeTexture);
        _lifeSprite[_lifeSprite.size()-1]->setTextureRect(IntRect(0,0,14,12));
        _lifeSprite[_lifeSprite.size()-1]->setOrigin(7,6);
        _lifeSpriteClone.push_back(new Sprite());
        _lifeSpriteClone[_lifeSprite.size()-1]->setTexture(_lifeTexture);
        _lifeSpriteClone[_lifeSprite.size()-1]->setTextureRect(IntRect(0,0,14,12));
        _lifeSpriteClone[_lifeSprite.size()-1]->setOrigin(7,6);
    }
}

void hero::DisableBody()
{
    _body->SetActive(false);
    _bodyClone->SetActive(false);
}

void hero::DrawLife(RenderWindow* window)
{
    if(_lifeSprite.size() <= 1)
        return;

    for(int i=0 ; i<_lifeSprite.size() ; i++)
    {
        if(_invincibilityFrame > 0)
        {
            _lifeSprite[i]->setColor(Color(255,255,255));
            _lifeSpriteClone[i]->setColor(Color(255,255,255));
        }
        else
        {
            _lifeSprite[i]->setColor(Color(255,255,255,200));
            _lifeSpriteClone[i]->setColor(Color(255,255,255,200));
        }

        _lifeSprite[i]->setPosition((_position.x+i*15)-((_lifeSprite.size()*14)/2)+4, _position.y+25);
        _lifeSpriteClone[i]->setPosition((_spriteClone.getPosition().x+i*15)-((_lifeSprite.size()*14)/2)+4, _spriteClone.getPosition().y+8);
        _lifeSprite[i]->setTextureRect(IntRect(i < _life ? 0 : 14,0,14,12));
        _lifeSpriteClone[i]->setTextureRect(IntRect(i < _life ? 0 : 14,0,14,12));
        window->draw(*_lifeSprite[i]);
        if(_position.x-13 <= 0 || _position.x+13 >= 800 || _position.y-30 <= 0 || _position.y+30 >= 600)
            window->draw(*_lifeSpriteClone[i]);
    }
}

void hero::Display(RenderWindow* window, float deltaTime)
{
    _deltaTime = deltaTime;
    if(_life == 0 && _stunned == 0) { return; } //Hero is dead
    DrawLife(window);
    window->draw(_sprite);
    if(_position.x-13 <= 0 || _position.x+13 >= 800 || _position.y-30 <= 0 || _position.y+30 >= 600)
    {
        _spriteClone.setTextureRect(_sprite.getTextureRect());
        _spriteClone.setScale(_sprite.getScale());
        _spriteClone.setColor(_sprite.getColor());
        window->draw(_spriteClone);
    }
}
