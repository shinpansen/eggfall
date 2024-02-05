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
#include "../tools/animation.hpp"
#include "../tools/files.hpp"
#include "../effects/effects.hpp"
#include "../effects/spells.hpp"
#include "../effects/sound.hpp"
#include "../objects/ladder.hpp"
#include "../UI/scoreManager.hpp"

using namespace std;
using namespace sf;

class spells;
class scoreManager;
class hero
{
public:
    hero();
    ~hero();
    hero(int wizardType, Vector2i pos, b2World* world, input* input, effects* effectBox, spells* spellBox,
         ladder* ladderBox, sound* soundBox, scoreManager* scoreManager, int life);
    void LoadWizardTexture(int wizardNumber);
    void UpdateVariables();
    void Move();
    void PerformEffects();
    void AnimeSprite();
    b2Body* GetBodyA();
    b2Body* GetBodyB();
    Vector2f GetPosition();
    int isCounteracting();
    bool isSelfSpellInvincible();
    void hasCounteracted();
    void SetStunned(Vector2f stunDirection, int stunDuration, int numWizardOfSpell);
    Vector2f GetEffectAnglePosition(int i);
    int GetWizardType();
    Color GetWizardColor(int i);
    void CrushWizard();
    int isAWizard(b2Body* body);
    int getLife();
    void Step();
    void ResetHero(int numColor, int life);
    void ResetLifeSprite();
    void DisableBody();
    void DrawLife(RenderWindow* window);
    void Display(RenderWindow* window, float deltaTime);
    vector<hero*> _wizards;
    bool _hasJumped;
private:
    //Variables
    bool _onTheFloor;
    bool _lastOnTheFloor;
    bool _hasRoll;
    bool _hasAttacked;
    bool _showDust;
    int _life;
    int _jumpTolerance; //Permit the hero to jump during some frames after leaving the floor
    int _canWallJump;
    int _rolling;
    int _rollingCooldown;
    int _hasCounteractedCoolDown;
    int _attacking;
    int _attackingCooldown;
    int _attackingAnimation;
    int _hooked;
    int _canClimb;
    int _frameDust;
    int _stunned;
    int _invincibilityFrame;
    int _userData;
    int _effectAnglePosition;
    int _wizardType;
    int _collisionType;
    int _dustFrame;
    int _selfSpellInvincible;
    int _maxLife;
    float _ladderTop;
    float _deltaTime;

    //Vectors
    Vector2i _initPos;
    Vector2f _speed;
    Vector2f _maxSpeed;
    Vector2f _position;
    Vector2f _positionClone;
    Vector2f _hookedPoint;
    Vector2f _ladderPos;
    Vector2f _stunDirection;

    //Objects
    input* _input;
    animation _animation;
    effects* _effectBox;
    spells* _spellBox;
    ladder* _ladderBox;
    files _files;
    sound* _soundBox;
    Sound* _wallSlidingSound;
    scoreManager* _scoreManager;

    //Box2D
    b2World* _world;
    b2Body* _body;
    b2Body* _bodyClone;

    //Sprite
    Sprite _sprite;
    Sprite _spriteClone;
    Sprite* _spellEffect;
    vector<Sprite*> _lifeSprite;
    vector<Sprite*> _lifeSpriteClone;
    Texture _texture;
    Texture _lifeTexture;
};
#endif
