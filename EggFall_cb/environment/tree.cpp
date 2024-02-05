#include "tree.hpp"

static const float SCALE = 30.f;
using namespace std;
using namespace sf;

tree::tree()
{
}

tree::tree(Vector2f pos,  Vector2i heightMinMax, Texture* texture, IntRect rect, Color color, b2World* world, effects* effectBox)
{
    /* TRICKY : _heightMinMax
     * x = TOP
     * y = bottom */

    //Variables
    _position = pos;
    _heightMinMax = heightMinMax;
    _rect = rect;
    _angle = 0;
    _angleToReach = 0;
    _colorID = 0;
    _kindOfTree = 0;

    //Objects
    _texture = texture;
    _sprite.setTexture(*_texture);
    _sprite.setTextureRect(_rect);
    _sprite.setOrigin(58,138);
    _sprite.setPosition(_position);
    _color = color;
    _world = world;
    _effectBox = effectBox;
	_leafDrawOption = new drawOption(Vector2f(0, 0), Vector2f(6, 3), _color);
	_woodDrawOption = new drawOption(Vector2f(0, 0), Vector2f(5, 5), Color(111, 47, 0));
}

tree::tree(Vector2f pos, Texture* texture, int colorID, int kindOfTree, b2World* world, effects* effectBox)
{
    //Variables
    _position = pos;
    _heightMinMax = getHeightMinMax(kindOfTree);
    _rect = IntRect(kindOfTree*115, colorID*150, 115, 150);
    _angle = 0;
    _angleToReach = 0;
    _colorID = colorID;
    _kindOfTree = kindOfTree;

    //Objects
    _texture = texture;
    _sprite.setTexture(*_texture);
    _sprite.setTextureRect(_rect);
    _sprite.setOrigin(58,138);
    _sprite.setPosition(_position);
    _color = getColor(colorID);
    _world = world;
    _effectBox = effectBox;
	_leafDrawOption = new drawOption(Vector2f(0, 0), Vector2f(6, 3), _color);
	_woodDrawOption = new drawOption(Vector2f(0, 0), Vector2f(5, 5), Color(111, 47, 0));
}

void tree::CalcAngle()
{
    for (b2Body* body = _world->GetBodyList(); body != 0; body = body->GetNext())
    {
        if(body->GetType() == b2_dynamicBody && (int)body->GetUserData() != UD_DASH &&
		   fabs(body->GetLinearVelocity().x) > 5 &&
           fabs(body->GetPosition().x*SCALE - _position.x) < 5 &&
           body->GetPosition().y*SCALE <= _position.y &&
           body->GetPosition().y*SCALE >= _position.y-_heightMinMax.x)
        {
            if(_angleToReach == 0) //Calculating initial angle
            {
                int x = body->GetPosition().x*SCALE;
                int y = body->GetPosition().y*SCALE;

				//Adding particles representing leaves and wood falling
				if (y <= _position.y - _heightMinMax.y && y >= _position.y - _heightMinMax.x)
				{
					// Leaves
					int random = utils::RandomNumber(2, 4);
					_leafDrawOption->pos = Vector2f(x, y - 10);
					for (int i = 0; i < random; i++)
						_effectBox->AddRectangleShape(*_leafDrawOption, fadeOption(0, 200), physics(_world, 0.5));

					// Wood
					random = utils::RandomNumber(1, 2);
					_woodDrawOption->pos = Vector2f(x, y - 10);
					for (int i = 0; i < random; i++)
						_effectBox->AddRectangleShape(*_woodDrawOption, fadeOption(0, 200), physics(_world, 0.9));
				}
            }
            _angleToReach = body->GetLinearVelocity().x/1.5f;
            return;
        }
    }
}

void tree::performTeeter(float deltaTime)
{
    _sprite.setRotation(_angle);
    if(fabs(_angle - _angleToReach) < 1 && fabs(_angleToReach) >= 1)
        _angleToReach = -_angleToReach/2;
    else if(fabs(_angleToReach) >= 1)
        _angle += (float)((_angleToReach - _angle)/4)*deltaTime;
    else
        _angleToReach = 0;
}

Vector2i tree::getHeightMinMax(int kindOfTree)
{
    switch(kindOfTree)
    {
        case 0:
            return Vector2i(85,5);
        case 1:
            return Vector2i(130,5);
        case 2:
            return Vector2i(110,10);
        case 3:
            return Vector2i(135,50);
        case 4:
            return Vector2i(130,5);
        case 5:
            return Vector2i(50,20);
        case 6:
            return Vector2i(75,20);
        case 7:
            return Vector2i(110,70);
        default:
            return Vector2i(85,5);
    }
}

Color tree::getColor(int colorID)
{
    switch(colorID)
    {
        case 0:
            return Color(107,215,28);
        case 1:
            return Color(83,128,0);
        case 2:
            return Color(49,80,58);
        case 3:
            return Color(0,128,38);
        case 4:
            return Color(224,203,100);
        case 5:
            return Color(216,131,25);
        case 6:
            return Color(170,100,0);
        case 7:
            return Color(201,201,201);
        case 8:
            return Color(255,106,143);
        case 9:
            return Color(222,59,39);
        case 10:
            return Color(128,39,32);
        case 11:
            return Color(160,101,219);
        case 12:
            return Color(143,157,229);
        default:
            return Color(107,215,28);
    }
}

Vector2f tree::GetPosition()
{
    return _position;
}

void tree::SetPosition(Vector2f pos)
{
    _position = pos;
    _sprite.setPosition(pos);
}

void tree::SetKind(int colorID, int kindOfTree)
{
    _colorID = colorID;
    _kindOfTree = kindOfTree;
    _heightMinMax = getHeightMinMax(kindOfTree);
    _rect = IntRect(kindOfTree*115, colorID*150, 115, 150);
    _sprite.setTextureRect(_rect);
    _color = getColor(colorID);
}

int tree::GetColorID()
{
    return _colorID;
}

int tree::GetKindOfTree()
{
    return _kindOfTree;
}

void tree::Step(float deltaTime)
{
    CalcAngle();
    performTeeter(deltaTime);
}

void tree::DrawTree(RenderWindow* window)
{
    window->draw(_sprite);
}
