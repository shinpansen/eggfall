#ifndef effects_hpp
#define effects_hpp

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <math.h>
#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <Box2D/Box2D.h>
#include "animation.hpp"
#include "effectObj.hpp"
#include "../tools/Box2DTools.hpp"
#include "../tools/files.hpp"

using namespace std;
using namespace sf;

enum SHAKE_TYPE
{
	SHAKE_DEFAULT,
	SHAKE_HORIZONTAL,
	SHAKE_VERTICAL
};

enum TEXTURE_ID
{
	T_SPELLS,
	T_SPELLS_EXPLOSION,
	T_HIT,
	T_SHIELD,
	T_GRIND,
	T_DEAD,
	T_DUST
};

class effects
{
public:
    effects();
	void AddSprite(TEXTURE_ID textureId, Vector2i animRowCol, drawOption drawOption, fadeOption fadeOption, physics physics, bool background = false);
	void AddCustomSprite(Texture* texture, IntRect rect, drawOption drawOption, fadeOption fadeOption, physics physics, bool background = false);
	Sprite* AddAnimation(TEXTURE_ID textureId, int numAnimation, float frameDelay, bool reversed, drawOption drawOption, fadeOption fadeOption, physics physics, int loop = -1, bool background = false);
	void AddRectangleShape(drawOption drawOption, fadeOption fadeOption, physics physics, bool background = false);
	void AddCircleShape(drawOption drawOption, fadeOption fadeOption, physics physics, bool background = false);
	void AddText(drawOption drawOption, fadeOption fadeOption, physics physics, bool background = false);
	void AddDust(Vector2f pos, bool clockwise, bool isStatic, float speedReduction = 0.3, bool background = false);
	void ShakeScreen(int amplitute, float speed, SHAKE_TYPE shakeType = SHAKE_DEFAULT);
	void FlashScreen(float duration);
	Texture* GetTexture(TEXTURE_ID textureId);
	IntRect GetTextureRect(TEXTURE_ID textureId, Vector2i animRowCol);
	View GetDefaultView(bool fullscreen);
	void DestroyEverything();
    void RenderEffects(b2World* world, bool fullscreen, float deltaTime);
	void Display(RenderWindow* window, bool fullscreen, float deltaTime);
	void DisplayFront(RenderWindow* window, bool fullscreen, float deltaTime);
	void DisplayBack(RenderWindow* window, bool fullscreen, float deltaTime);
private:
	//Variables
    float _deltaTime;
	float _shakeSpeed;
	float _flashScreen;
	int _shakeFrame;
	Vector2f _shakePositions[5];

	//Lists
	vector<effectObj*> _effectObjectsFront;
	vector<effectObj*> _effectObjectsBack;

	//Graphics
	RectangleShape _flashShape;
    Texture _spellsTexture;
	Texture _spellsExplosionTexture;
	Texture _hitTexture;
	Texture _shieldTexture;
	Texture _grindEffectTexture;
	Texture _dyingTexture;
	Texture _dustTexture;
	Font _font;
};
#endif
