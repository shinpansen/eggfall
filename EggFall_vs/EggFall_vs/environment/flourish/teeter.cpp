#include "teeter.hpp"

static const float SCALE = 30.f;

teeterNode::teeterNode()
{
}

teeterNode::teeterNode(IntRect textureRect, Vector2f origin, Vector2f relativePos, bool invertRotation, 
	bool displayInFront, float speedReduction, float angleRatio, float angleReduction)
{
	this->textureRect = textureRect;
	this->origin = origin;
	this->relativePos = relativePos;
	this->invertRotation = invertRotation;
	this->displayInFront = displayInFront;
	this->speedReduction = speedReduction != 0.f ? speedReduction : 1.f;
	this->angleReduction = angleReduction != 0.f && angleReduction > 1.f ? angleReduction : 2.f;
	this->angleRatio = fabs(angleRatio);
	this->angle = 0.f;
	this->angleToReach = 0.f;
}

teeter::~teeter()
{
	//Delete root sprite
	if (_rootSprite != NULL)
		delete _rootSprite;

	//Delete nodes
	for (int i = 0; i < _nodesSprites.size(); i++)
		if (_nodesSprites[i] != NULL)
			delete _nodesSprites[i];
}

teeter::teeter()
{
}

teeter::teeter(b2World * world, effects * effectBox, Texture * teeterTexture, Vector2f pos, int teeterHeight, Vector2f rectColorID, teeterNode root)
{
    //Variables
    _pos = pos;
	_rectColorID = rectColorID;
	_teeterHeight = teeterHeight;
	_contactPos = Vector2i();

    //Objects
	_world = world;
	_effectBox = effectBox;
	_root = root;

	//Graphics
    _teeterTexture = teeterTexture;

	//Root
	_rootSprite = new Sprite();
	_rootSprite->setTexture(*_teeterTexture);
	IntRect ir = root.textureRect;
	_rootSprite->setTextureRect(IntRect(ir.left + _rectColorID.x, ir.top + _rectColorID.y, ir.width, ir.height));
	_rootSprite->setOrigin(root.origin);
	_rootSprite->setPosition(pos);
}

void teeter::LoadNodes(vector<teeterNode> nodes)
{
	//Delete existing sprites
	for (int i = 0; i < _nodesSprites.size(); i++)
		if (_nodesSprites[i] != NULL)
			delete _nodesSprites[i];

	//Adding nodes
	_nodes = nodes;
	for (int i = 0; i < _nodes.size(); i++)
	{
		_nodesSprites.push_back(new Sprite());
		_nodesSprites[i]->setTexture(*_teeterTexture);
		IntRect ir = _nodes[i].textureRect;
		_nodesSprites[i]->setTextureRect(IntRect(ir.left + _rectColorID.x, ir.top + _rectColorID.y, ir.width, ir.height));
		_nodesSprites[i]->setOrigin(_nodes[i].origin);
		_nodesSprites[i]->setPosition(_pos + _nodes[i].relativePos);
	}
}

/*teeter::teeter(Vector2f pos, Texture* texture, int colorID, int kindOfTree, b2World* world, effects* effectBox)
{
    //Variables
    _position = pos;
    _heightMinMax = getHeightMinMax(kindOfTree);
    _rect = IntRect(kindOfTree*115, colorID*150, 115, 150);
	_angle = 0.f;
	_angleLeaf = 0.f;
	_angleToReach = 0.f;
	_angleToReachLeaf = 0.f;
    _colorID = colorID;
    _kindOfTree = kindOfTree;

    //Objects
    _texture = texture;
    _sprite.setTexture(*_texture);
    _sprite.setTextureRect(_rect);
    _sprite.setOrigin(57.5, 144);
    _sprite.setPosition(_position);
    _color = getColor(colorID);
    _world = world;
    _effectBox = effectBox;
	_leafDrawOption = new drawOption(Vector2f(0, 0), Vector2f(6, 3), _color);
	_woodDrawOption = new drawOption(Vector2f(0, 0), Vector2f(5, 5), Color(111, 47, 0));

	_leavesSprites.push_back(new Sprite());
	_leavesSprites[0]->setTexture(*_texture);
	_leavesSprites[0]->setTextureRect(IntRect(115, 0, 115, 150));
	_leavesSprites[0]->setOrigin(46, 116);
	_leavesSprites[0]->setPosition(_position.x - 11.5, _position.y - 28);
}*/

void teeter::CalcAngle()
{
	_contactPos = Vector2i();
    for (b2Body* body = _world->GetBodyList(); body != 0; body = body->GetNext())
    {
        if(body->GetType() == b2_dynamicBody && (int)body->GetUserData() != UD_DASH &&
		   body->GetFixtureList()->GetFilterData().categoryBits != Box2DTools::PARTICLE_ENTITY && 
		   fabs(body->GetLinearVelocity().x) > 5 &&
           fabs(body->GetPosition().x*SCALE - _pos.x) < 5 &&
           ((body->GetPosition().y*SCALE >= _pos.y - _teeterHeight &&
             body->GetPosition().y*SCALE <= _pos.y + 10) ||
			(_pos.y - _teeterHeight < 0 &&
			 body->GetPosition().y*SCALE >= utils::VIEW_HEIGHT - fabs(_pos.y - _teeterHeight) &&
			 body->GetPosition().y*SCALE <= utils::VIEW_HEIGHT + 10)))
        {
			//Root angle to reach (max = 30°)
			_root.angleToReach = body->GetLinearVelocity().x * _root.angleRatio;
			_root.angleToReach = fabs(_root.angleToReach) <= 30 ? _root.angleToReach : fabs(_root.angleToReach) / _root.angleToReach * 30.f;
			_contactPos = Vector2i(body->GetPosition().x * SCALE, body->GetPosition().y * SCALE);

			//Nodes angles to reach
			for (int i = 0; i < _nodes.size(); i++)
			{
				int signe = _nodes[i].invertRotation ? 1 : -1;
				if (_root.angleToReach > 0)
					_nodes[i].angleToReach = _root.angleToReach - fabs(body->GetLinearVelocity().x) * signe * _nodes[i].angleRatio;
				else
					_nodes[i].angleToReach = _root.angleToReach + fabs(body->GetLinearVelocity().x) * signe * _nodes[i].angleRatio;
			}
			break;
        }
    }
}

Color teeter::GetColor(int colorID)
{
	return _rootSprite != NULL ? _rootSprite->getColor() : Color();
}

void teeter::PerformTeeter(float deltaTime)
{
	//Stepping root rotation
	if (fabs(_root.angle - _root.angleToReach) < 1.f && fabs(_root.angleToReach) >= 1.f)
		_root.angleToReach = -_root.angleToReach / _root.angleReduction;
	else if (fabs(_root.angleToReach) >= 1.f)
		_root.angle += ((_root.angleToReach - _root.angle) / _root.speedReduction)*deltaTime;
	else
		_root.angleToReach = 0.f;

	//Rotating root sprite
	if (_rootSprite != NULL)
		_rootSprite->setRotation(_root.angle);

	//Zero ajust
	if (_root.angleToReach == 0.f && fabs(_root.angle) < 1.f)
		_root.angle = 0.f;

	//Nodes rotation
	for (int i = 0; i < _nodes.size(); i++)
	{
		//Vector rotation 
		Vector2f vect = Vector2f(0, _nodes[i].relativePos.y);
		Vector2f vectRotate = utils::RotateVector(vect, _root.angle);

		//Stepping node rotation
		if (fabs(_nodes[i].angle - _nodes[i].angleToReach) < 1 && fabs(_nodes[i].angleToReach) >= 1)
			_nodes[i].angleToReach = -_nodes[i].angleToReach / _nodes[i].angleReduction;
		else if (fabs(_nodes[i].angleToReach) >= 1)
			_nodes[i].angle += (float)((_nodes[i].angleToReach - _nodes[i].angle) / _nodes[i].speedReduction)*deltaTime;
		else
			_nodes[i].angleToReach = 0;

		//Rotating node
		Vector2f initPos = _pos + _nodes[i].relativePos;
		_nodesSprites[i]->setPosition(initPos.x + (vectRotate.x - vect.x), initPos.y + (vectRotate.y - vect.y));
		if(_nodes[i].angleRatio != 0.f)
			_nodesSprites[i]->setRotation(_nodes[i].angle);
		else 
			_nodesSprites[i]->setRotation(_rootSprite->getRotation());

		//Zero ajust
		if (_nodes[i].angleToReach == 0.f && fabs(_nodes[i].angle) < 1.f)
			_nodes[i].angle = 0.f;
	}
}

Vector2f teeter::GetPosition()
{
    return _pos;
}

void teeter::SetPosition(Vector2f pos)
{
    _pos = pos;
    
	//TODO - move sprites
}

int teeter::GetColorID()
{
    return _colorID;
}

void teeter::Step(float deltaTime)
{
    CalcAngle();
    PerformTeeter(deltaTime);
	StepChild(deltaTime); //Hidden method
}

void teeter::StepChild(float deltatime) {};

void teeter::Display(RenderWindow* window)
{
	//Back Nodes
	for (int i = 0; i < _nodesSprites.size(); i++)
		if (_nodesSprites[i] != NULL && _nodes.size() > i && !_nodes[i].displayInFront)
			utils::DrawLoopSprite(window, _nodesSprites[i]);

	//Root
	if(_rootSprite != NULL)
		utils::DrawLoopSprite(window, _rootSprite);

	//In front Nodes
	for (int i = 0; i < _nodesSprites.size(); i++)
		if (_nodesSprites[i] != NULL && _nodes.size() > i && _nodes[i].displayInFront)
			utils::DrawLoopSprite(window, _nodesSprites[i]);

	//Display hidden methods
	DisplayChild(window);
}

void teeter::DisplayChild(RenderWindow* window) {}
