#include "Box2DTools.hpp"

static const float SCALE = 30.f;
const short Box2DTools::GROUND_ENTITY = 0x0001;
const short Box2DTools::PLATFORM_ENTITY = 0x0002;
const short Box2DTools::BODY_ENTITY = 0x0008;
const short Box2DTools::PARTICLE_ENTITY = 0x0016;
const short Box2DTools::LADDER_ENTITY = 0x0032;

using namespace std;
using namespace sf;

Box2DTools::Box2DTools()
{
}

b2Body* Box2DTools::CreateRectangleBox(b2World* world, Vector2f pos, float width, float height, float friction, float density, float angle, short category, bool bridge)
{
	b2Body* body = CreateBody(world, pos, Vector2f(width, height), friction, density, angle, category, SHAPE::RECTANGLE, bridge);
	return body;
}

b2Body* Box2DTools::CreateCircleBox(b2World* world, Vector2f pos, int radius, float friction, float density, short category, bool bridge)
{
	b2Body* body = CreateBody(world, pos, Vector2f(radius, radius), friction, density, 0, category, SHAPE::CIRCLE, bridge);
	return body;
}

b2Body * Box2DTools::CreateBody(b2World* world, Vector2f pos, Vector2f shapeSize, float friction, float density, float angle, short category, SHAPE shape, bool bridge)
{
	//Testing world
	if (world == NULL) { return NULL; }

	//Body
	b2BodyDef bodyDef;
	bodyDef.position = b2Vec2(pos.x / SCALE, pos.y / SCALE);
	bodyDef.type = category == BODY_ENTITY || category == PARTICLE_ENTITY || bridge ? b2_dynamicBody : b2_staticBody;
	bodyDef.angle = utils::DegreeToRadian(angle);
	b2Body* body = world->CreateBody(&bodyDef);

	//Fixture
	b2FixtureDef fixtureDef;
	fixtureDef.density = density;
	fixtureDef.friction = friction;
	fixtureDef.filter.categoryBits = bridge ? PLATFORM_ENTITY : category;
	if (category == PARTICLE_ENTITY)
		fixtureDef.filter.maskBits = GROUND_ENTITY | PLATFORM_ENTITY;
	else if (category == LADDER_ENTITY)
		fixtureDef.filter.maskBits = LADDER_ENTITY;
	else if (bridge)
		fixtureDef.filter.maskBits = BODY_ENTITY;
	else
		fixtureDef.filter.maskBits = GROUND_ENTITY | PLATFORM_ENTITY | BODY_ENTITY;

	//Shape
	b2PolygonShape polygonShape;
	shapeSize.x = shapeSize.x < 1.f ? 1.f : shapeSize.x;
	shapeSize.y = shapeSize.y < 1.f ? 1.f : shapeSize.y;
	polygonShape.SetAsBox((shapeSize.x / 2.f) / SCALE, (shapeSize.y / 2.f) / SCALE);
	b2CircleShape circleShape;
	circleShape.m_radius = ((shapeSize.x / 2.f) / SCALE);
	if (shape == SHAPE::RECTANGLE)
		fixtureDef.shape = &polygonShape;
	else
		fixtureDef.shape = &circleShape;

	//Adding fixture
	body->CreateFixture(&fixtureDef);

	//Random initial velocity if particle
	if (category == PARTICLE_ENTITY)
	{
		body->SetLinearVelocity(b2Vec2((rand() % 8) - 4, -2));
		body->SetTransform(body->GetPosition(), rand() % 10);
	}
	return body;
}

bool Box2DTools::TestCollisionPoint(b2World* world, b2Vec2 point, bool all, short* collisionType, b2Body** body)
{
	//Testing world
	if (world == NULL) { return false; }

	//Iterating through bodies
    for (b2Body* BodyIterator = world->GetBodyList(); BodyIterator != 0; BodyIterator = BodyIterator->GetNext())
    {
        if(!BodyIterator->IsActive()) { continue; }
        for (b2Fixture* fixture = BodyIterator->GetFixtureList(); fixture; fixture = fixture->GetNext())
        {
            b2Filter filter = fixture->GetFilterData();
            double height = GetAABBSize(fixture).y;
            double edge = fixture->GetBody()->GetPosition().y - (height/2);
			*collisionType = fixture->GetFilterData().categoryBits;

            if(body != NULL)
                *body = fixture->GetBody();

            if(!all && filter.categoryBits == GROUND_ENTITY && fixture->TestPoint(point))
                return true;
            else if(all && (filter.categoryBits == GROUND_ENTITY || filter.categoryBits == BODY_ENTITY) &&
                    fixture->TestPoint(point))
                return true;
            else if(all && (filter.categoryBits == PLATFORM_ENTITY) &&
                    fixture->TestPoint(point))
                return true;
        }
    }
    return false;
}

int Box2DTools::TestHookPoint(b2World* world, b2Vec2 point, Vector2f* collisionPos, bool reversed)
{
	//Testing world
	if (world == NULL) { return 0; }

	//Iterating through bodies
    for (b2Body* BodyIterator = world->GetBodyList(); BodyIterator != 0; BodyIterator = BodyIterator->GetNext())
    {
		//Can only hook to static bodies
		if (BodyIterator->GetType() != b2_staticBody || (int)BodyIterator->GetUserData() != UD_HOOK)
			continue;

		//Can't hook to perfectly stacked bodies (if testing a body with another stacked above)
		short colTest;
		Vector2f groundSize = GetAABBSize(BodyIterator, false);
		Vector2f edgeTopLeft = Vector2f(BodyIterator->GetPosition().x - (groundSize.x / 2.f), BodyIterator->GetPosition().y - (groundSize.y / 2.f));
		Vector2f edgeTopRight = Vector2f(BodyIterator->GetPosition().x + (groundSize.x / 2.f), BodyIterator->GetPosition().y - (groundSize.y / 2.f));
		/*if ((!reversed && TestCollisionPoint(world, b2Vec2(edgeTopLeft.x + 0.02f, edgeTopLeft.y - 0.05f), true, &colTest)) ||
			(reversed && TestCollisionPoint(world, b2Vec2(edgeTopRight.x - 0.02f, edgeTopRight.y - 0.05f), true, &colTest)))
			continue;*/

		//Right hook test
		if (!reversed && fabs(point.x - edgeTopLeft.x) < 0.3 && fabs(point.y - edgeTopLeft.y) < 0.4 && point.y <= edgeTopLeft.y)
		{
			*collisionPos = edgeTopLeft;
			return -1;
		}
		//Left hook test
		else if (reversed && fabs(point.x - edgeTopRight.x) < 0.3 && fabs(point.y - edgeTopRight.y) < 0.4 && point.y <= edgeTopRight.y)
		{
			*collisionPos = edgeTopRight;
			return 1;
		}
    }
    return 0;
}

void Box2DTools::ManageBodyLoop(b2World* world)
{
	//Testing world
	if (world == NULL) { return; }

    b2Filter filter;
    for (b2Body* BodyIterator = world->GetBodyList(); BodyIterator != 0; BodyIterator = BodyIterator->GetNext())
    {
        filter = BodyIterator->GetFixtureList()[0].GetFilterData();
        if(filter.categoryBits == BODY_ENTITY)
        {
            if(BodyIterator->GetPosition().x*SCALE > utils::VIEW_WIDTH)
                BodyIterator->SetTransform(b2Vec2(0,BodyIterator->GetPosition().y), BodyIterator->GetAngle());
            else if(BodyIterator->GetPosition().x*SCALE < 0)
                BodyIterator->SetTransform(b2Vec2(utils::VIEW_WIDTH/SCALE,BodyIterator->GetPosition().y), BodyIterator->GetAngle());
            else if(BodyIterator->GetPosition().y*SCALE > utils::VIEW_HEIGHT)
                BodyIterator->SetTransform(b2Vec2(BodyIterator->GetPosition().x, 0/SCALE), BodyIterator->GetAngle());
            else if(BodyIterator->GetPosition().y*SCALE < 0)
                BodyIterator->SetTransform(b2Vec2(BodyIterator->GetPosition().x, utils::VIEW_HEIGHT/SCALE), BodyIterator->GetAngle());
        }
    }
}

void Box2DTools::ChangeMaskBits(b2Body* body, MASK maskType)
{
	if (body == NULL) { return; }
	for (b2Fixture* fixture = body->GetFixtureList(); fixture; fixture = fixture->GetNext())
	{
		b2Filter* filter = new b2Filter();
		filter->categoryBits = fixture->GetFilterData().categoryBits;
		filter->maskBits = maskType == MASK_ALL ? GROUND_ENTITY|PLATFORM_ENTITY|BODY_ENTITY : GROUND_ENTITY;
		fixture->SetFilterData(*filter);
	}
}

Vector2f Box2DTools::CalculateAdjustedContactPos(b2Contact * contact, Vector2f directionVector)
{
	//Calculating contact location
	b2WorldManifold worldManifold;
	contact->GetWorldManifold(&worldManifold);
	Vector2f contactPos = Vector2f(worldManifold.points[0].x*SCALE, worldManifold.points[0].y*SCALE);

	//Adjusting contact location (searching for edge)
	b2Body * groundBody = contact->GetFixtureA()->GetBody();
	if (contact->GetFixtureB()->GetBody()->GetType() == b2_staticBody)
		groundBody = contact->GetFixtureB()->GetBody();
	Vector2f groundSize = Box2DTools::GetAABBSize(groundBody, true);
	FloatRect groundBounds = FloatRect(groundBody->GetPosition().x*SCALE - groundSize.x / 2.f,
		groundBody->GetPosition().y*SCALE - groundSize.y / 2.f, groundSize.x, groundSize.y);
	while (groundBounds.contains(contactPos))
	{
		contactPos.x += -directionVector.x / 100.f;
		contactPos.y += -directionVector.y / 100.f;
	}
	return contactPos;
}

void Box2DTools::DrawCollisionMask(b2World* world, RenderWindow* window)
{
	//Testing world
	if (world == NULL) { return; }

    b2Transform defaultTransform(b2Vec2(0,0), b2Rot(0));
    for (b2Body* body = world->GetBodyList(); body != 0; body = body->GetNext())
    {
		if (body == NULL || !body->IsActive()) { continue; }
        //if(body->GetType() != b2_dynamicBody/* && body->GetFixtureList()->GetFilterData().categoryBits != Box2DTools::PLATFORM_ENTITY*/) { continue; }
		if(body->GetFixtureList()->GetFilterData().categoryBits != Box2DTools::GROUND_ENTITY) { continue; }

        bool isACircle = false;
        b2AABB aabb;
        b2CircleShape circleShapeTest;
        for (b2Fixture* fixture = body->GetFixtureList(); fixture; fixture = fixture->GetNext())
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
        shape.setPosition(SCALE * body->GetPosition().x, SCALE * body->GetPosition().y);
		shape.setRotation(utils::RadianToDegree(body->GetAngle()));

        //Circle box
        CircleShape circleShape(width/2);
        circleShape.setFillColor(Color(255,255,255,50));
        circleShape.setOutlineThickness(2);
        circleShape.setOrigin(width/2, width/2);
        circleShape.setPosition(SCALE * body->GetPosition().x, SCALE * body->GetPosition().y);

        //Color
        if (body->GetFixtureList()[0].GetFilterData().categoryBits == BODY_ENTITY)
            shape.setOutlineColor(Color(255,150,150));
        else if(body->GetFixtureList()[0].GetFilterData().categoryBits == GROUND_ENTITY)
            shape.setOutlineColor(Color(150,255,150));
        else
            shape.setOutlineColor(Color(150,150,255));

        if(isACircle)
            window->draw(circleShape);
        else
            window->draw(shape);
    }
}


Vector2f Box2DTools::GetAABBSize(b2Fixture* fixture)
{
    b2AABB aabb = fixture->GetAABB(0);
    fixture->GetShape()->ComputeAABB(&aabb, fixture->GetBody()->GetTransform(), 0);
    float width = fabs(aabb.upperBound.x - aabb.lowerBound.x);
	float height = fabs(aabb.upperBound.y - aabb.lowerBound.y);
	return Vector2f(width, height);
}

Vector2f Box2DTools::GetAABBSize(b2Body* body, bool upscaleToSFMLUnits)
{
	b2AABB aabb;
	b2Transform defaultTransform(b2Vec2(0, 0), b2Rot(0));
	for (b2Fixture* fixture = body->GetFixtureList(); fixture; fixture = fixture->GetNext())
	{
		aabb = fixture->GetAABB(0);
		fixture->GetShape()->ComputeAABB(&aabb, defaultTransform, 0);
	}

	if(upscaleToSFMLUnits)
		return Vector2f((int)((aabb.upperBound.x*SCALE) - (aabb.lowerBound.x*SCALE)), (int)((aabb.upperBound.y*SCALE) - (aabb.lowerBound.y*SCALE)));
	else
		return Vector2f((aabb.upperBound.x) - (aabb.lowerBound.x), (aabb.upperBound.y) - (aabb.lowerBound.y));
}

contactListener::contactListener(b2World * world)
{
	world->SetContactListener(this);
}

contactListener::~contactListener(void)
{
}

void contactListener::PreSolve(b2Contact * contact, const b2Manifold * oldManifold)
{
	if (!contact->IsTouching())
		return;

	//Disable collision during dash
	if (contact->GetFixtureA()->GetFilterData().categoryBits == Box2DTools::BODY_ENTITY &&
		contact->GetFixtureB()->GetFilterData().categoryBits == Box2DTools::BODY_ENTITY &&
	   ((int)contact->GetFixtureA()->GetBody()->GetUserData() == UD_DASH ||
		(int)contact->GetFixtureB()->GetBody()->GetUserData() == UD_DASH))
	{
		contact->SetEnabled(false);
	}
	//Disable collision with platforms if climbimg or dashing - Projectiles passing through platforms if going up
	else if ((contact->GetFixtureA()->GetFilterData().categoryBits == Box2DTools::BODY_ENTITY &&
		      contact->GetFixtureB()->GetFilterData().categoryBits == Box2DTools::PLATFORM_ENTITY &&
			 ((int)contact->GetFixtureA()->GetBody()->GetUserData() == UD_CLIMB ||
			  (int)contact->GetFixtureA()->GetBody()->GetUserData() == UD_DASH)) ||
			 (contact->GetFixtureB()->GetFilterData().categoryBits == Box2DTools::BODY_ENTITY &&
			  contact->GetFixtureA()->GetFilterData().categoryBits == Box2DTools::PLATFORM_ENTITY &&
			 ((int)contact->GetFixtureB()->GetBody()->GetUserData() == UD_CLIMB ||
		      (int)contact->GetFixtureB()->GetBody()->GetUserData() == UD_DASH)))
	{
		contact->SetEnabled(false);
	}
	//Collision with platform
	else if ((contact->GetFixtureA()->GetFilterData().categoryBits == Box2DTools::BODY_ENTITY &&
			  contact->GetFixtureB()->GetFilterData().categoryBits == Box2DTools::PLATFORM_ENTITY) ||
			 (contact->GetFixtureB()->GetFilterData().categoryBits == Box2DTools::BODY_ENTITY &&
			  contact->GetFixtureA()->GetFilterData().categoryBits == Box2DTools::PLATFORM_ENTITY))
	{
		//Identifying bodies
		b2Body* dynamicBody = contact->GetFixtureA()->GetFilterData().categoryBits == Box2DTools::BODY_ENTITY ?
			contact->GetFixtureA()->GetBody() : contact->GetFixtureB()->GetBody();
		b2Body* platformBody = contact->GetFixtureA()->GetFilterData().categoryBits == Box2DTools::PLATFORM_ENTITY ?
			contact->GetFixtureA()->GetBody() : contact->GetFixtureB()->GetBody();

		//Test
		if(platformBody->GetAngle() == 0.f && dynamicBody->GetLinearVelocity().y < -0.1)
			contact->SetEnabled(false);
		else
		{
			Vector2f bodySize = Box2DTools::GetAABBSize(dynamicBody, true);
			Vector2f platformSize = Box2DTools::GetAABBSize(platformBody, true);
			RectangleShape bodyShape = RectangleShape(Vector2f(bodySize.x, 3.f));
			bodyShape.setOrigin(bodySize.x / 2.f, 1.5f);
			bodyShape.setPosition(dynamicBody->GetPosition().x * SCALE, dynamicBody->GetPosition().y * SCALE + bodySize.y / 2.f);
			RectangleShape platformShape = RectangleShape(Vector2f(platformSize.x, platformSize.y));
			platformShape.setOrigin(platformSize.x / 2.f, platformSize.y / 2.f);
			platformShape.setPosition(platformBody->GetPosition().x * SCALE, platformBody->GetPosition().y * SCALE);
			platformShape.setRotation(utils::RadianToDegree(platformBody->GetAngle()));
			if(!bodyShape.getGlobalBounds().intersects(platformShape.getGlobalBounds()))
				contact->SetEnabled(false);
		}
	}
	//Avoid dynamic bodies being stuck between two perfectly aligned static bodies while moving
	else if ((contact->GetFixtureA()->GetFilterData().categoryBits == Box2DTools::BODY_ENTITY ||
		 contact->GetFixtureB()->GetFilterData().categoryBits == Box2DTools::BODY_ENTITY) &&
	    (contact->GetFixtureA()->GetFilterData().categoryBits == Box2DTools::GROUND_ENTITY ||
		 contact->GetFixtureB()->GetFilterData().categoryBits == Box2DTools::GROUND_ENTITY))
	{
		//Identifying bodies
		b2Body* heroBody = contact->GetFixtureA()->GetFilterData().categoryBits == Box2DTools::BODY_ENTITY ?
			contact->GetFixtureA()->GetBody() : contact->GetFixtureB()->GetBody();
		b2Body* groundBody = contact->GetFixtureA()->GetFilterData().categoryBits == Box2DTools::GROUND_ENTITY ?
			contact->GetFixtureA()->GetBody() : contact->GetFixtureB()->GetBody();

		//Size and position
		Vector2f heroSize = Box2DTools::GetAABBSize(heroBody, true);
		Vector2f heroPos = Vector2f(heroBody->GetPosition().x*SCALE, heroBody->GetPosition().y*SCALE);
		Vector2f groundSize = Box2DTools::GetAABBSize(groundBody, true);
		Vector2f groundPos = Vector2f(groundBody->GetPosition().x*SCALE, groundBody->GetPosition().y*SCALE);
		float tolerance = 1.5f;

		//Horizontal moving test
		if ((fabs((groundPos.y - groundSize.y / 2.f) - (heroPos.y + heroSize.y / 2.f)) < tolerance &&
			 fabs((groundPos.x - groundSize.x / 2.f) - (heroPos.x + heroSize.x / 2.f)) < tolerance &&
			 heroBody->GetLinearVelocity().x > 0.1) ||
			(fabs((groundPos.y - groundSize.y / 2.f) - (heroPos.y + heroSize.y / 2.f)) < tolerance &&
			 fabs((groundPos.x + groundSize.x / 2.f) - (heroPos.x - heroSize.x / 2.f)) < tolerance &&
			 heroBody->GetLinearVelocity().x < -0.1))
		{
			contact->SetEnabled(false);
		}
		//Vertical moving test (walljump)
		else if ((fabs((groundPos.y - groundSize.y / 2.f) - (heroPos.y + heroSize.y / 2.f)) < tolerance &&
			 fabs((groundPos.x - groundSize.x / 2.f) - (heroPos.x + heroSize.x / 2.f)) < tolerance &&
			 heroBody->GetLinearVelocity().y > 0.1) ||
			(fabs((groundPos.y - groundSize.y / 2.f) - (heroPos.y + heroSize.y / 2.f)) < tolerance &&
			 fabs((groundPos.x + groundSize.x / 2.f) - (heroPos.x - heroSize.x / 2.f)) < tolerance &&
			 heroBody->GetLinearVelocity().y > 0.1) ||
			(fabs((groundPos.y + groundSize.y / 2.f) - (heroPos.y - heroSize.y / 2.f)) < tolerance &&
			 fabs((groundPos.x - groundSize.x / 2.f) - (heroPos.x + heroSize.x / 2.f)) < tolerance &&
			 heroBody->GetLinearVelocity().y < -0.1) ||
			(fabs((groundPos.y + groundSize.y / 2.f) - (heroPos.y - heroSize.y / 2.f)) < tolerance &&
			 fabs((groundPos.x + groundSize.x / 2.f) - (heroPos.x - heroSize.x / 2.f)) < tolerance &&
			 heroBody->GetLinearVelocity().y < -0.1))
		{
			contact->SetEnabled(false);
		}
	}
}
