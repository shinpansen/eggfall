#ifndef Box2DTools_HPP
#define Box2DTools_HPP

#include <iostream>
#include <math.h>
#include <vector>
#include <typeinfo>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <Box2D/Box2D.h>

using namespace std;
using namespace sf;

class Box2DTools
{
public:
    Box2DTools();
    static b2Body* CreateRectangleBox(b2World* world, Vector2i pos, int width, int height, float friction, float density, float angle, int category);
    static b2Body* CreateCircleBox(b2World* world, Vector2i pos, int radius, float friction, float density, int category);
    static bool TestCollisionPoint(b2World* world, b2Vec2 point, bool all, int* collisionType, b2Body** body = NULL);
    static int TestHookPoint(b2World* world, b2Vec2 point, Vector2f* collisionPos, bool reversed);
    static void ManageOneWayPlatforms(b2World* world);
    static void ManageBodyLoop(b2World* world);
    static void DrawCollisionMask(b2World* world, RenderWindow* window);
    static uint16 Entity(int i);
    static Vector2f GetAABBSize(b2Fixture* fixture);
    static int getCollisionTypeNum(uint16 categoryBit);
};
#endif
