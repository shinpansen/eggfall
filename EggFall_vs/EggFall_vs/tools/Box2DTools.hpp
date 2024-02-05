#ifndef Box2DTools_HPP
#define Box2DTools_HPP

#include <iostream>
#include <math.h>
#include <vector>
#include <typeinfo>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <Box2D/Box2D.h>
#include "utils.hpp"

using namespace std;
using namespace sf;

enum SHAPE
{
	RECTANGLE,
	CIRCLE,
	POLYGON
};

enum MASK
{
	MASK_ALL,
	MASK_STATIC
};

enum USER_DATA
{
    UD_VOID = 0,
    UD_HOOK = 1,
    UD_DASH = 2,
    UD_CLIMB = 3
};

class Box2DTools
{
public:
    Box2DTools();
    static b2Body* CreateRectangleBox(b2World* world, Vector2f pos, float width, float height, float friction, float density, float angle, short category, bool bridge = false);
    static b2Body* CreateCircleBox(b2World* world, Vector2f pos, int radius, float friction, float density, short category, bool bridge = false);
	static b2Body* CreateBody(b2World* world, Vector2f pos, Vector2f shapeSize, float friction, float density, float angle, short category, SHAPE shape, bool bridge = false);
    static bool TestCollisionPoint(b2World* world, b2Vec2 point, bool all, short* collisionType, b2Body** body = NULL);
    static int TestHookPoint(b2World* world, b2Vec2 point, Vector2f* collisionPos, bool reversed);
	static bool TestLadderPoint(b2World* world, b2Vec2 point);
    static void ManageBodyLoop(b2World* world);
	static void ChangeMaskBits(b2Body* body, MASK maskType);
	static Vector2f CalculateAdjustedContactPos(b2Contact* contact, Vector2f directionVector);
    static void DrawCollisionMask(b2World* world, RenderWindow* window);
    static Vector2f GetAABBSize(b2Fixture* fixture);
	static Vector2f GetAABBSize(b2Body* body, bool upscaleToSFMLUnits);

    //Physics bodies entity
	static const short GROUND_ENTITY;
	static const short PLATFORM_ENTITY;
	static const short BODY_ENTITY;
	static const short PARTICLE_ENTITY;
	static const short LADDER_ENTITY;
};

class contactListener : public b2ContactListener
{
public:
	contactListener(b2World *world);
	virtual ~contactListener(void);
	virtual void PreSolve(b2Contact* contact, const b2Manifold* oldManifold);
};
#endif
