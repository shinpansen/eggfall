#include "Box2DTools.hpp"

static const float SCALE = 30.f;
static const short GROUND_ENTITY = 0x0001;
static const short PLATFORM_ENTITY = 0x0002;
static const short PLATFORM2_ENTITY = 0x0004;
static const short BODY_ENTITY = 0x0008;
static const short PARTICLE_ENTITY = 0x0016;

using namespace std;
using namespace sf;

/* TRICKY - category
 * 1 : b2_staticBody "GROUND"
 * 2 : b2_staticBody "ONE WAY PLATFORM"
 * 3 : b2_staticBody "ONE WAY PLATFORM 2" Used if hero is in collision with two 1way platforms
 * 4 : b2_dynamicBody "BODY"
 * 5 : b2_dynamicBody "PARTICLE BODY" */

Box2DTools::Box2DTools()
{
}

b2Body* Box2DTools::CreateRectangleBox(b2World* world, Vector2i pos, int width, int height, float friction, float density, float angle, int category)
{
    b2BodyDef bodyDef;
    bodyDef.position = b2Vec2(pos.x/SCALE, pos.y/SCALE);
    bodyDef.type = (category >= 4) ? b2_dynamicBody : b2_staticBody;
    bodyDef.angle = angle*b2_pi/180;
    b2Body* body = world->CreateBody(&bodyDef);

    b2PolygonShape shape;
    shape.SetAsBox((width/2)/SCALE, (height/2)/SCALE);
    b2FixtureDef fixtureDef;
    fixtureDef.density = density;
    fixtureDef.friction = friction;
    fixtureDef.shape = &shape;
    fixtureDef.filter.categoryBits = Entity(category);
    fixtureDef.filter.maskBits = category != 5 ? GROUND_ENTITY|PLATFORM_ENTITY|PLATFORM2_ENTITY|BODY_ENTITY : GROUND_ENTITY;
    body->CreateFixture(&fixtureDef);
    if(category == 5)
    {
        body->SetLinearVelocity(b2Vec2((rand()%8)-4,-2));
        body->SetTransform(body->GetPosition(), rand()%10);
    }

    return body;
}

b2Body* Box2DTools::CreateCircleBox(b2World* world, Vector2i pos, int radius, float friction, float density, int category)
{
    b2BodyDef bodyDef;
    bodyDef.position = b2Vec2(pos.x/SCALE, pos.y/SCALE);
    bodyDef.type = (category >= 4) ? b2_dynamicBody : b2_staticBody;
    b2Body* body = world->CreateBody(&bodyDef);

    b2CircleShape shape;
    shape.m_radius = ((radius/2)/SCALE);
    b2FixtureDef fixtureDef;
    fixtureDef.density = density;
    fixtureDef.friction = friction;
    fixtureDef.shape = &shape;
    fixtureDef.filter.categoryBits = Entity(category);
    fixtureDef.filter.maskBits = category != 5 ? GROUND_ENTITY|PLATFORM_ENTITY|PLATFORM2_ENTITY|BODY_ENTITY : GROUND_ENTITY;
    body->CreateFixture(&fixtureDef);
    if(category == 5)
    {
        body->SetLinearVelocity(b2Vec2((rand()%8)-4,-2));
        body->SetTransform(body->GetPosition(), rand()%10);
    }

    return body;
}

bool Box2DTools::TestCollisionPoint(b2World* world, b2Vec2 point, bool all, int* collisionType, b2Body** body)
{
    for (b2Body* BodyIterator = world->GetBodyList(); BodyIterator != 0; BodyIterator = BodyIterator->GetNext())
    {
        if(!BodyIterator->IsActive()) { continue; }
        for (b2Fixture* fixture = BodyIterator->GetFixtureList(); fixture; fixture = fixture->GetNext())
        {
            b2Filter filter = fixture->GetFilterData();
            double height = GetAABBSize(fixture).y;
            double edge = fixture->GetBody()->GetPosition().y - (height/2);
            *collisionType = getCollisionTypeNum(fixture->GetFilterData().categoryBits);

            if(body != NULL)
                *body = fixture->GetBody();

            if(!all && filter.categoryBits == GROUND_ENTITY && fixture->TestPoint(point))
                return true;
            else if(all && (filter.categoryBits == GROUND_ENTITY || filter.categoryBits == BODY_ENTITY) &&
                    fixture->TestPoint(point))
                return true;
            else if(all && (filter.categoryBits == PLATFORM_ENTITY || filter.categoryBits == PLATFORM2_ENTITY) &&
                    fixture->TestPoint(point) && fabs(point.y - edge) < 0.05)
                return true;
        }
    }
    return false;
}

int Box2DTools::TestHookPoint(b2World* world, b2Vec2 point, Vector2f* collisionPos, bool reversed)
{
    for (b2Body* BodyIterator = world->GetBodyList(); BodyIterator != 0; BodyIterator = BodyIterator->GetNext())
    {
        for (b2Fixture* fixture = BodyIterator->GetFixtureList(); fixture; fixture = fixture->GetNext())
        {
            b2Filter filter = fixture->GetFilterData();
            if(filter.categoryBits == GROUND_ENTITY)
            {
                Vector2f fixtureSize = GetAABBSize(fixture);
                b2Vec2 pos = fixture->GetBody()->GetPosition();
                Vector2f edgeTopLeft = Vector2f(pos.x-(fixtureSize.x/2), pos.y-(fixtureSize.y/2));
                Vector2f edgeTopRight = Vector2f(pos.x+(fixtureSize.x/2), pos.y-(fixtureSize.y/2));
                if(fabs(point.x - edgeTopLeft.x) < 0.2 && fabs(point.y - edgeTopLeft.y) < 0.2 &&
                   point.y <= edgeTopLeft.y && !reversed)
                {
                    *collisionPos = edgeTopLeft;
                    return -1;
                }

                else if(fabs(point.x - edgeTopRight.x) < 0.2 && fabs(point.y - edgeTopRight.y) < 0.2 &&
                        point.y <= edgeTopRight.y && reversed)
                {
                    *collisionPos = edgeTopRight;
                    return 1;
                }
            }
        }
    }
    return 0;
}

void Box2DTools::ManageOneWayPlatforms(b2World* world)
{
    //Reset filters
    b2Filter filter;
    for (b2Body* BodyIterator = world->GetBodyList(); BodyIterator != 0; BodyIterator = BodyIterator->GetNext())
    {
        filter = BodyIterator->GetFixtureList()[0].GetFilterData();
        if(filter.categoryBits != PARTICLE_ENTITY)
        {
            filter.maskBits = GROUND_ENTITY|PLATFORM_ENTITY|PLATFORM2_ENTITY|BODY_ENTITY;
            BodyIterator->GetFixtureList()[0].SetFilterData(filter);
        }
    }

    //Manage one way platforms filters
    for (b2Contact* contact = world->GetContactList() ; contact ; contact = contact->GetNext())
    {
        //Get fixtures
        bool OneWayContact = false;
        b2Fixture* fixtureA = contact->GetFixtureA();
        b2Fixture* fixtureB = contact->GetFixtureB();

        //fixtureA = body, fixtureB = platform
        if(fixtureA->GetFilterData().categoryBits == BODY_ENTITY &&
          (fixtureB->GetFilterData().categoryBits == PLATFORM_ENTITY ||
           fixtureB->GetFilterData().categoryBits == PLATFORM2_ENTITY))
            OneWayContact = true;
        else if((fixtureA->GetFilterData().categoryBits == PLATFORM_ENTITY ||
                 fixtureA->GetFilterData().categoryBits == PLATFORM2_ENTITY) &&
                 fixtureB->GetFilterData().categoryBits == BODY_ENTITY)
        {
            fixtureA = contact->GetFixtureB();
            fixtureB = contact->GetFixtureA();
            OneWayContact = true;
        }

        //Getting categoryBits of fixtureB
        short category = fixtureB->GetFilterData().categoryBits;
        if(category == PLATFORM_ENTITY)
            category = PLATFORM2_ENTITY;
        else if(category == PLATFORM2_ENTITY)
            category = PLATFORM_ENTITY;

        //height of bounding box
        Vector2f sizeA = GetAABBSize(fixtureA);
        Vector2f sizeB = GetAABBSize(fixtureB);

        //One way contact testing
        if(OneWayContact)
        {
            if(fixtureA->GetBody()->GetLinearVelocity().y < -0.5)
            {
                filter = fixtureA->GetFilterData();
                filter.maskBits = GROUND_ENTITY|BODY_ENTITY|category;
                fixtureA->SetFilterData(filter);
            }
            else if(fixtureB->GetBody()->GetPosition().y - (sizeB.y/2) + 0.3 <
               fixtureA->GetBody()->GetPosition().y + (sizeA.y/2) ||
               fixtureA->GetBody()->GetPosition().x + (sizeA.x/2) <=
               fixtureB->GetBody()->GetPosition().x - (sizeB.x/2) ||
               fixtureA->GetBody()->GetPosition().x - (sizeA.x/2) >=
               fixtureB->GetBody()->GetPosition().x + (sizeB.x/2))
            {
                filter = fixtureA->GetFilterData();
                filter.maskBits = GROUND_ENTITY|BODY_ENTITY|category;
                fixtureA->SetFilterData(filter);
            }
            else if(fixtureB->GetBody()->GetPosition().y - (sizeB.y/2) + 0.3 >=
                    fixtureA->GetBody()->GetPosition().y + (sizeA.y/2) &&
                    fixtureB->GetBody()->GetPosition().y - (sizeB.y/2) + 0.05 <
                    fixtureA->GetBody()->GetPosition().y + (sizeA.y/2))
            {
                b2Vec2 pos = fixtureA->GetBody()->GetPosition();
                double gap = fabs((fixtureB->GetBody()->GetPosition().y - (sizeB.y/2)) -
                               (fixtureA->GetBody()->GetPosition().y + (sizeA.y/2))) + 0.06;
                fixtureA->GetBody()->SetTransform(b2Vec2(pos.x, pos.y-gap), fixtureA->GetBody()->GetAngle());
            }
        }
    }
}

void Box2DTools::ManageBodyLoop(b2World* world)
{
    b2Filter filter;
    for (b2Body* BodyIterator = world->GetBodyList(); BodyIterator != 0; BodyIterator = BodyIterator->GetNext())
    {
        filter = BodyIterator->GetFixtureList()[0].GetFilterData();
        if(filter.categoryBits == BODY_ENTITY)
        {
            if(BodyIterator->GetPosition().x*SCALE > 800)
                BodyIterator->SetTransform(b2Vec2(0/SCALE,BodyIterator->GetPosition().y), BodyIterator->GetAngle());
            else if(BodyIterator->GetPosition().x*SCALE < 0)
                BodyIterator->SetTransform(b2Vec2(800/SCALE,BodyIterator->GetPosition().y), BodyIterator->GetAngle());
            else if(BodyIterator->GetPosition().y*SCALE > 600)
                BodyIterator->SetTransform(b2Vec2(BodyIterator->GetPosition().x, 0/SCALE), BodyIterator->GetAngle());
            else if(BodyIterator->GetPosition().y*SCALE < 0)
                BodyIterator->SetTransform(b2Vec2(BodyIterator->GetPosition().x, 600/SCALE), BodyIterator->GetAngle());
        }
    }
}

void Box2DTools::DrawCollisionMask(b2World* world, RenderWindow* window)
{
    b2Transform defaultTransform(b2Vec2(0,0), b2Rot(0));
    for (b2Body* BodyIterator = world->GetBodyList(); BodyIterator != 0; BodyIterator = BodyIterator->GetNext())
    {
        //if(BodyIterator->GetType() == b2_dynamicBody) { continue; }

        bool isACircle = false;
        b2AABB aabb;
        b2CircleShape circleShapeTest;
        for (b2Fixture* fixture = BodyIterator->GetFixtureList(); fixture; fixture = fixture->GetNext())
        {
            aabb = fixture->GetAABB(0);
            fixture->GetShape()->ComputeAABB(&aabb, defaultTransform, 0);
            isACircle = fixture->GetShape()->GetType() == circleShapeTest.GetType();
        }

        int width = (aabb.upperBound.x*SCALE) - (aabb.lowerBound.x*SCALE) - 3; //-3 : Remove border size
        int height = (aabb.upperBound.y*SCALE) - (aabb.lowerBound.y*SCALE) - 3;

        //Rectangle box
        RectangleShape shape(Vector2f(width, height));
        shape.setFillColor(Color(255,255,255,50));
        shape.setOutlineThickness(2);
        shape.setOrigin(width/2, height/2);
        shape.setPosition(SCALE * BodyIterator->GetPosition().x, SCALE * BodyIterator->GetPosition().y);
        shape.setRotation(BodyIterator->GetAngle() * 180/b2_pi);

        //Circle box
        CircleShape circleShape(width/2);
        circleShape.setFillColor(Color(255,255,255,50));
        circleShape.setOutlineThickness(2);
        circleShape.setOrigin(width/2, width/2);
        circleShape.setPosition(SCALE * BodyIterator->GetPosition().x, SCALE * BodyIterator->GetPosition().y);

        //Color
        if (BodyIterator->GetFixtureList()[0].GetFilterData().categoryBits == BODY_ENTITY)
            shape.setOutlineColor(Color(255,150,150));
        else if(BodyIterator->GetFixtureList()[0].GetFilterData().categoryBits == GROUND_ENTITY)
            shape.setOutlineColor(Color(150,255,150));
        else
            shape.setOutlineColor(Color(150,150,255));

        if(isACircle)
            window->draw(circleShape);
        else
            window->draw(shape);
    }
}

uint16 Box2DTools::Entity(int i)
{
    switch(i)
    {
        case 1:
            return GROUND_ENTITY;
        case 2:
            return PLATFORM_ENTITY;
        case 3:
            return PLATFORM2_ENTITY;
        case 4:
            return BODY_ENTITY;
        case 5:
            return PARTICLE_ENTITY;
        default:
            return GROUND_ENTITY;
    }
}

Vector2f Box2DTools::GetAABBSize(b2Fixture* fixture)
{
    b2AABB aabb = fixture->GetAABB(0);
    fixture->GetShape()->ComputeAABB(&aabb, fixture->GetBody()->GetTransform(), 0);
    double width = fabs(aabb.upperBound.x - aabb.lowerBound.x);
    double height = fabs(aabb.upperBound.y - aabb.lowerBound.y);
    return Vector2f(width, height);
}

int Box2DTools::getCollisionTypeNum(uint16 categoryBit)
{
    if(categoryBit == GROUND_ENTITY)
        return 0;
    else if(categoryBit == PLATFORM_ENTITY || categoryBit == PLATFORM2_ENTITY)
        return 1;
    else if(categoryBit == BODY_ENTITY)
        return 2;
    else
        return 3;
}
