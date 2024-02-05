#include "bridge.hpp"

static const float SCALE = 30.f;

bridge::~bridge()
{
	//Destroying Box2D joints
	for (int i = 0; i < _revoluteJoints.size(); i++)
		if (_revoluteJoints[i] != NULL)
			_world->DestroyJoint(_revoluteJoints[i]);

	//Destroying Box2D bodies
	if (_leftBody != NULL)
		_world->DestroyBody(_leftBody);
	if (_rightBody != NULL)
		_world->DestroyBody(_rightBody);
	for (int i = 0; i < _bridgeBodies.size(); i++)
		if (_bridgeBodies[i] != NULL)
			_world->DestroyBody(_bridgeBodies[i]);

	//Destroying SFML sprites
	for (int i = 0; i < _bridgeSprites.size(); i++)
		if (_bridgeSprites[i] != NULL)
			delete _bridgeSprites[i];
	if (_leftBridge != NULL)
		delete _leftBridge;
	if (_rightBridge != NULL)
		delete _rightBridge;
}

bridge::bridge(b2World * world, Texture * texture, int colorID, Vector2f posLeft, Vector2f posRight)
{
	//Variables - Objects
	_world = world;
	_ladderPlatformTexture = texture;
	_colorID = colorID >= 0 && colorID <= 1 ? colorID : 0;
	_posLeft = posLeft.x <= posRight.x ? posLeft : posRight;
	_posRight = posRight.x > posLeft.x ? posRight : posLeft;
	if (fabs(_posLeft.x - _posRight.x) < 16)
		_posRight.x = _posLeft.x + 16.f;

	//Left and right tree trunks
	_leftBridge = new Sprite();
	_leftBridge->setTexture(*_ladderPlatformTexture);
	_leftBridge->setTextureRect(IntRect(64 + 128 * _colorID, 96, 48, 64));
	_leftBridge->setOrigin(44, 52);
	_leftBridge->setPosition(_posLeft);
	_leftBody = Box2DTools::CreateRectangleBox(_world, Vector2f(_posLeft.x - 8, _posLeft.y + 8), 16, 16, 0, 0, 0, Box2DTools::PLATFORM_ENTITY);
	_rightBridge = new Sprite();
	_rightBridge->setTexture(*_ladderPlatformTexture);
	_rightBridge->setTextureRect(IntRect(144 + 128 * _colorID, 96, 48, 64));
	_rightBridge->setOrigin(4, 52);
	_rightBridge->setPosition(_posRight);
	_rightBody = Box2DTools::CreateRectangleBox(_world, Vector2f(_posRight.x + 8, _posRight.y + 8), 16, 16, 0, 0, 0, Box2DTools::PLATFORM_ENTITY);

	//Creating bridge blocks
	int nbBlocks = utils::DistanceBetween(_posLeft, _posRight) / 16;
	float yDistance = _posRight.y - _posLeft.y;
	for (int i = 0; i < nbBlocks; i++)
	{
		//Block sprite randomize
		int leftRect, topRect;
		if (i == 0 || i == nbBlocks - 1)
		{
			leftRect = 112;
			topRect = 96;
		}
		else if (utils::RandomNumber(2, 0) != 0)
		{
			leftRect = 128;
			topRect = 96;
		}
		else
		{
			leftRect = 112 + 16 * utils::RandomNumber(1, 0);
			topRect = 112 + 16 * utils::RandomNumber(1, 0);
		}

		//Add Sprite
		_bridgeSprites.push_back(new Sprite());
		_bridgeSprites[i]->setTexture(*_ladderPlatformTexture);
		_bridgeSprites[i]->setTextureRect(IntRect(leftRect + 128 * _colorID, topRect, 16, 16));
		_bridgeSprites[i]->setOrigin(8, 8);

		//Box2D bodies
		Vector2f blockPos = Vector2f(_posLeft.x + 8 + i * 16, _posLeft.y + (yDistance / nbBlocks * i));
		_bridgeBodies.push_back(Box2DTools::CreateRectangleBox(_world, blockPos, 20, 8, 0.01, 5, 0, Box2DTools::PLATFORM_ENTITY, true));

		//Box2D joints
		b2RevoluteJointDef  jointDef;
		jointDef.bodyA = i == 0 ? _leftBody : _bridgeBodies[i-1];
		jointDef.bodyB = _bridgeBodies[i];
		jointDef.collideConnected = false;
		jointDef.localAnchorA.Set(8.f / SCALE, (i == 0 ? -8.f / SCALE : -8.f / SCALE));
		jointDef.localAnchorB.Set(-8.f / SCALE, -8.f / SCALE);
		_revoluteJoints.push_back(_world->CreateJoint(&jointDef));

		//Last joint
		if (i == nbBlocks - 1)
		{
			b2RevoluteJointDef jointDef;
			jointDef.bodyA = _bridgeBodies[i];
			jointDef.bodyB = _rightBody;
			jointDef.collideConnected = false;
			jointDef.localAnchorA.Set(8.f / SCALE, -8.f / SCALE);
			jointDef.localAnchorB.Set(-8.f / SCALE, -8.f / SCALE);
			_revoluteJoints.push_back(_world->CreateJoint(&jointDef));
		}
	}
}

FloatRect bridge::GetGlobalBounds()
{
	if (_bridgeSprites.size() == 1)
		return FloatRect(_bridgeSprites[0]->getGlobalBounds());
	else
	{
		FloatRect boundA = _bridgeSprites[0]->getGlobalBounds();
		FloatRect boundB = _bridgeSprites[_bridgeSprites.size() / 2]->getGlobalBounds();
		FloatRect boundC = _bridgeSprites[_bridgeSprites.size() - 1]->getGlobalBounds();
		return FloatRect(boundA.left - 8, boundA.top - 8, boundC.left + boundC.width + 16 - boundA.left, boundB.top + boundB.height + 16 - boundA.top - 8);
	}
}

string bridge::GetType()
{
	if (_colorID == 0)
		return "wood_a";
	else
		return "wood_b";
}

bool bridge::IntersectRect(FloatRect rect)
{
	for (int i = 0; i < _bridgeSprites.size(); i++)
		if (_bridgeSprites[i]->getGlobalBounds().intersects(rect))
			return true;
	return false;
}

int bridge::GetColorID()
{
	return _colorID;
}

Vector2f bridge::GetLeftPos()
{
	return _leftBridge->getPosition();
}

Vector2f bridge::GetRightPos()
{
	return _rightBridge->getPosition();
}

Color bridge::GetColor()
{
	return _leftBridge->getColor();
}

void bridge::SetColor(Color color)
{
	_leftBridge->setColor(color);
	_rightBridge->setColor(color);
	for (int i = 0; i < _bridgeSprites.size(); i++)
		_bridgeSprites[i]->setColor(color);
}

vector<Sprite*>* bridge::GetBridgeSprites()
{
	return &_bridgeSprites;
}

pair<Sprite*, Sprite*> bridge::GetLeftRightBridge()
{
	pair<Sprite*, Sprite*> pair;
	pair.first = _leftBridge;
	pair.second = _rightBridge;
	return pair;
}

void bridge::BreakBridgeBlocks(Vector2f pos)
{
	//Searching for blocks to alter
	for (int i = 0; i < _bridgeSprites.size(); i++)
		if (_bridgeSprites[i] != NULL && utils::DistanceBetween(_bridgeSprites[i]->getPosition(), pos) < 30 &&
			_bridgeSprites[i]->getTextureRect() == IntRect(128 + 128 * _colorID, 96, 16, 16))
			_bridgeSprites[i]->setTextureRect(IntRect(112 + 16*utils::RandomNumber(1,0) + 128 * _colorID, 112, 16, 16));
}

void bridge::Step(float deltaTime)
{
}

void bridge::Display(RenderWindow * window)
{
	//Drawing left and right tree trunks
	window->draw(*_leftBridge);
	window->draw(*_rightBridge);

	//Drawing bridge blocks
	for (int i = 0; i < _bridgeSprites.size(); i++)
	{
		if (_bridgeSprites[i] != NULL && _bridgeBodies[i] != NULL)
		{
			_bridgeSprites[i]->setPosition(_bridgeBodies[i]->GetPosition().x*SCALE, _bridgeBodies[i]->GetPosition().y*SCALE-2);
			_bridgeSprites[i]->setRotation(utils::RadianToDegree(_bridgeBodies[i]->GetAngle()));
			window->draw(*_bridgeSprites[i]);
		}
	}

	//TEST
	/*RectangleShape rect;
	FloatRect bound = GetGlobalBounds();
	rect.setSize(Vector2f(bound.width, bound.height));
	rect.setPosition(Vector2f(bound.left, bound.top));
	rect.setFillColor(Color(255, 0, 0, 100));
	window->draw(rect);*/
}
