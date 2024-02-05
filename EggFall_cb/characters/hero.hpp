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
#include "../UI/scoreManager.hpp"

using namespace std;
using namespace sf;

enum CHARA_EFFECT
{
	DUST_LANDING,
	DUST_WALLJUMP,
	DUST_CROUCHING,
	DASH_TRAIL,
	DASH_STAR
};

class projectiles;
class scoreManager;
class hero
{
public:
    hero();
    ~hero();
    hero(int colorID, int life, Vector2f pos, b2World* world, input* input, effects* effectBox,
		 projectiles* projectileBox, sound* soundBox, scoreManager* scoreManager);
	virtual void LoadHeroTexture();
    void Update();
    void Move();
	virtual bool PerformAttack();
    void PerformEffects();
    void AnimSprite();
	virtual void AnimAttack();
    b2Body* GetBodyA();
    b2Body* GetBodyB();
	Vector2f GetPositionA();
	Vector2f GetPositionB();
    int isCounteracting();
	void SetSelfProjectileInvincible(b2Body* projectileBody);
	bool IsSelfProjectileInvincible(b2Contact* contact);
	bool IsDashing();
	bool IsStunned();
    void HasCounteracted();
	void SetStunned(Vector2f stunDirection, int stunDuration, b2Body* projectileOrigin);
	void SetHasJumped(bool jumped);
    int GetColorID();
    Color GetHeroColor(int i);
    void CrushHero();
    int IsAPlayer(b2Body* body);
    int GetLife();
    void ResetHero(int numColor, int life);
	void ResetBody(int width, int height);
    void ResetLifeSprite();
    void DisableBody();
	void DrawEffect(CHARA_EFFECT effectType, int pos = -1);
    void DrawLife(RenderWindow* window);
	void Step(float deltaTime);
    void Display(RenderWindow* window, float deltaTime);
    vector<hero*> _players;
protected:
    //Variables
    bool _onTheFloor;
	bool _headOnTheCeiling;
    bool _lastOnTheFloor;
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
	float _counteract;
	float _counteractCooldown;
	float _hasCounteractedCooldown;
	float _dash;
	float _dashCooldown;
	float _dashEffectFrame;
	float _canClimb;
	float _stunned;
	float _dustFrame;
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

    //2D vectors
	Vector2f _initPos;
    Vector2f _speed;
    Vector2f _maxSpeed;
    Vector2f _position;
    Vector2f _positionClone;
	Vector2f _dashDirection;
    Vector2f _hookedPoint;
    Vector2f _ladderPos;
    Vector2f _stunDirection;
	Vector2f _spellSpeed;
	Vector2f _spellPos;

    //Objects
    input* _input;
	animation _animation;
    effects* _effectBox;
    projectiles* _projectileBox;
    sound* _soundBox;
    Sound* _wallSlidingSound;
    scoreManager* _scoreManager;

    //Box2D
    b2World* _world;
    b2Body* _body;
    b2Body* _bodyClone;

    //Sprite
    Sprite _sprite;
	Sprite _spriteFlash;
	Sprite _aimArrow;
    Sprite* _spellEffect;
    vector<Sprite*> _lifeSprite;
	Texture _texture[3];
	Texture _textureFlash[3];
	Texture _lifeTexture;
	Texture _aimArrowTexture;
};
#endif
