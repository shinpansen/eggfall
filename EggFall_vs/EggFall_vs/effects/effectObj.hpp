#ifndef effectObj_hpp
#define effectObj_hpp

#include <iostream>
#include <cstdlib>
#include <cmath>
#include <math.h>
#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <Box2D\Box2D.h>
#include "animation.hpp"
#include "../tools/Box2DTools.hpp"
#include "../tools/utils.hpp"
#include "../UI/menu/menuList.hpp"

using namespace std;
using namespace sf;

enum EFFECT_TYPE
{
	EFFECT_NONE,
	EFFECT_SPRITE,
	EFFECT_RECT,
	EFFECT_CIRCLE,
	EFFECT_TEXT
};

class drawOption
{
public:
	drawOption(Vector2f pos, Vector2f shapeSize, Vector2f origin, Color fillColor, float rotation = 0, int opacity = 255, Color outlineColor = Color(0, 0, 0), int outlineThickness = 0);
	drawOption(Vector2f pos, Vector2f shapeSize, Color fillColor, float rotation = 0, int opacity = 255, Color outlineColor = Color(0, 0, 0), int outlineThickness = 0);
	drawOption(Vector2f pos, Vector2f origin = Vector2f(0,0), float rotation = 0, int opacity = 255);
	drawOption(string text, int characterSize, Vector2f pos, Vector2f origin, Color fillColor, float rotation = 0, int opacity = 255, Color outlineColor = Color(0, 0, 0), int outlineThickness = 0);
	Vector2f pos;
	Vector2f shapeSize;
	Vector2f origin;
	Color fillColor;
	Color outlineColor;
	int outlineThickness;
	int opacity;
	float rotation;
	string text;
	int characterSize;
};

class fadeOption
{
public:
	fadeOption();
	fadeOption(Vector2f directionStep, float opacityStep, float rotationStep, float durationStep, float speedReduction = 0, Vector2f scaleStep = Vector2f(0,0), float opacityDelay = 0.f, bool stopAtEndAnimation = true);
	fadeOption(Vector2f directionStep, float durationStep, float speedReduction);
	fadeOption(float opacityStep);
	fadeOption(float rotationStep, float durationStep);
	fadeOption(Vector2f scaleStep, float durationStep);

	Vector2f directionStep;
	float opacityStep;
	float rotationStep;
	float duration;
	float speedReduction;
	float opacityDelay;
	Vector2f scaleStep;
	bool stopAtEndAnimation;
};

class physics
{
public:
	physics(bool enabled, b2World* world = NULL, Vector2f impulse = Vector2f(0, 0), float friction = 0.5, float density = 0.05, float gravityScale = 1);
	physics(b2World* world = NULL, float gravityScale = 1);
	bool enabled;
	b2World* world;
	Vector2f impulse;
	float friction;
	float density;
	float gravityScale;
};

class effectObj
{
public:
	~effectObj();
	effectObj();
	effectObj(Texture* texture, IntRect textureRect, float rotation, Vector2f pos, Vector2f origin, float opacity, fadeOption fadeOption);
	effectObj(Texture* texture, Vector2f tileSize, float frameDelay, int nbFrames, int numAnimation, bool reversed, float rotation, Vector2f pos, Vector2f origin, float opacity, fadeOption fadeOption, int loop = -1);
	effectObj(float width, float height, Color fillColor, Color outlineColor, int outlineThickness, float rotation, Vector2f pos, Vector2f origin, float opacity, fadeOption fadeOption);
	effectObj(float radius, Color fillColor, Color outlineColor, int outlineThickness, Vector2f pos, Vector2f origin, float opacity, fadeOption fadeOption);
	effectObj(string text, Font* font, int characterSize, Color fillColor, Color outlineColor, int outlineThickness, float rotation, Vector2f pos, ALIGN textAlign, float opacity, fadeOption fadeOption);
	void AddBox2DPhysics(physics physics);
	void AddBox2DPhysics(b2World* world, Vector2f impulse, float friction, float density, float gravityScale);
	void Step(float deltaTime);
	bool IsFinished();
	Sprite* GetSprite();
	void Display(RenderWindow * window);
private:
	//Methods
	void Init(fadeOption fadeOption = NULL, float opacity = 0.f);
	void InitPhysics(Vector2f impulse, float friction, float density, float gravityScale);

	//Variables - Objects
	fadeOption _fadeOption;
	float _opacity;
	bool _finished;
	EFFECT_TYPE _effectType;

	//SFML Graphics objetcs
	Sprite* _sprite;
	animation* _animation;
	RectangleShape* _rect;
	CircleShape* _circle;
	Text* _text;
	
	//Box2D
	b2World* _world;
	b2Body* _body;
};
#endif