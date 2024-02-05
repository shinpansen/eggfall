#ifndef hero_hpp
#define hero_hpp

#include <math.h>
#include <string.h>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <Box2D/Box2D.h>
#include "../tools/input.hpp"
#include "../tools/Box2DTools.hpp"
#include "../tools/files.hpp"
#include "../tools/utils.hpp"
#include "../effects/animation.hpp"
#include "../effects/effects.hpp"
#include "../effects/sound.hpp"
#include "../projectiles/projectiles.hpp"
#include "../UI/display/scoreManager.hpp"
#include "../UI/display/playerHUD.hpp"

using namespace std;
using namespace sf;

enum CHARA_EFFECT
{
	DUST_MOVING,
	DUST_JUMPING,
	DUST_LANDING,
	DUST_WALLJUMP,
	DUST_WALLJUMP_LEAVE,
	DUST_CROUCHING,
	DASH_TRAIL,
	DASH_STAR
};

class playerHUD;
class projectiles;
class projectileObj;
class scoreManager;
class hero
{
public:
    hero();
    ~hero();
    hero(int colorID, int life, Vector2f pos, b2World* world, input* input, effects* effectBox,
		 projectiles* projectileBox, sound* soundBox, scoreManager* scoreManager, Texture * heroTexture,
	     Texture * heroTextureFlash, Texture * lifeStaminaTexture, Texture * aimArrowTexture);
    void Update();
    void Move();
	virtual bool PerformAttack();
	void PerformCounteract(bool success = true);
	virtual void PerformCounteractChildEffect();
    void PerformEffects();
    void AnimSprite();
	virtual void AnimAttack();
	virtual void AnimateChild();
    b2Body* GetBodyA();
    b2Body* GetBodyB();
	Vector2f GetPositionA(bool addSpeed = true);
	Vector2f GetPositionB(bool addSpeed = true);
    int isCounteracting();
	void SetSelfProjectileInvincible(b2Body* projectileBody);
	bool IsSelfProjectileInvincible(b2Contact* contact);
	int GetHooked();
	Vector2f GetSpeed();
	int GetSigneSpeed();
	bool IsDashing();
	bool IsStunned();
	bool IsAiming();
	bool IsCounteractingProjectile();
	bool OnPlatform();
	bool OnLadder();
	bool ClimbingLadder();
    void HasCounteracted(projectileObj * projectile);
	void SetStunned(Vector2f stunDirection, Vector2f contactPos, int stunDuration, b2Body* projectileOrigin);
	void SetHasJumped(bool jumped);
	Vector2f GetInitPos();
	Vector2f GetScale();
	input * GetInput();
	Texture * GetHeroTexture();
	Texture * GetHeroTextureFlash();
    int GetColorID();
    Color GetHeroColor(int i);
    void CrushHero();
    int IsAPlayer(b2Body* body);
	int GetLife();
	float GetStamina();
	bool IsInvincible();
	bool IsOnTheFloor();
	bool IsCrouching();
	void ResetHero(int colorID, int life);
	void ResetBody(int width, int height);
	void ResetLifeSprites();
    void DisableBody();
	void DrawEffect(CHARA_EFFECT effectType, int pos = -1);
	void Step(float deltaTime);
	void DrawHUD(RenderWindow* window);
    void Display(RenderWindow* window, float deltaTime);
    vector<hero*> _players;
protected:
    //Variables
	bool _onTheFloor;
	bool _lastOnTheFloor;
	bool _onBridge;
	bool _headOnTheCeiling;
	bool _hasJumped;
	bool _hasCounteract;
	bool _hasDash;
    bool _hasAttacked;
	bool _isAiming;
	bool _cancelJumpFallScale;
	bool _dashStep;
	bool _crouch;
	bool _cloneVisible;
	bool _crushAttackAnimation;
	bool _stuck;
	bool _drawDust;
	bool _lastReversed;
	int _colorID;
    int _life;
	int _maxLife;
    int _hooked;
	int _attackingAnimation;
    int _effectAnglePosition;
	short _collisionType;
	float _canWallJump;
	float _jumpTolerance; //Permit the hero to jump during some frames after leaving the floor
	float _attacking;
	float _attackingCooldown;
	float _attackingCounteract;
	float _counteract;
	float _lastCounteract;
	float _lastCounteractSigne;
	float _counteractCooldown;
	float _hasCounteractedCooldown;
	float _isCounteractingProjectile;
	float _dash;
	float _dashCooldown;
	float _dashEffectFrame;
	float _canClimb;
	float _stamina;
	float _staminaCooldown;
	float _stunned;
	float _lastSpeedX;
	float _dustFrame;
	float _dustDuration;
	float _dustCooldown;
	float _invincibilityFrame;
	float _ladderHeight;
	float _flashFrame;
	float _hasJumpedCooldown;
	float _spellAngle;
	float _selfProjectileInvincible;
	float _lastFallingSpeed;
	float _updateFallingSpeedFrame;
	float _aimingSpeedCooldown;
	float _platformFall;
    float _deltaTime;
	b2Body* _lastProjectile;
	projectileObj* _counteractedProjectile;

    //2D vectors
	Vector2f _initPos;
    Vector2f _speed;
    Vector2f _maxSpeed;
    Vector2f _position;
    Vector2f _positionClone;
	Vector2f _dashDirection;
	Vector2f _lastContactPos;
    Vector2f _hookedPoint;
    Vector2f _ladderPos;
    Vector2f _stunDirection;
	Vector2f _projectileSpeed;
	Vector2f _projectilePos;
	Vector2f _hitRelativePos;

    //Objects
	animation * _animation;
	playerHUD * _playerHUD;
    input * _input;
    effects * _effectBox;
    projectiles * _projectileBox;
    sound * _soundBox;
	scoreManager * _scoreManager;
    Sound * _wallSlidingSound;

    //Box2D
    b2World * _world;
    b2Body * _body;
    b2Body * _bodyClone;

    //Sprite
    Sprite * _heroSprite;
	Sprite * _heroSpriteFlash;
	Sprite * _aimArrow;
	Sprite * _projectileEffect;
	Sprite * _hitEffect;
	Sprite * _shieldEffect;
	Sprite * _grindEffect;
    vector<Sprite*> _lifeSprite;
	Texture * _heroTexture;
	Texture * _heroTextureFlash;
	Texture * _lifeStaminaTexture;
	Texture * _aimArrowTexture;
};
#endif
