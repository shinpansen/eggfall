#include "effectObj.hpp"

static const float SCALE = 30.f;

using namespace std;
using namespace sf;

//DRAW OPTION
drawOption::drawOption(Vector2f pos, Vector2f shapeSize, Vector2f origin, Color fillColor, float rotation, int opacity, Color outlineColor, int outlineThickness)
{
	//Shape constructor custom origin
	this->pos = pos;
	this->shapeSize = shapeSize;
	this->origin = origin;
	this->rotation = rotation;
	this->opacity = opacity;
	this->fillColor = fillColor;
	this->outlineColor = outlineColor;
	this->outlineThickness = outlineThickness;
	this->text = "";
	this->characterSize = 0;
}

drawOption::drawOption(Vector2f pos, Vector2f shapeSize, Color fillColor, float rotation, int opacity, Color outlineColor, int outlineThickness)
{
	//Shape constructor centered origin
	this->pos = pos;
	this->shapeSize = shapeSize;
	this->origin = Vector2f(shapeSize.x / 2.f, shapeSize.y / 2.f);
	this->rotation = rotation;
	this->opacity = opacity;
	this->fillColor = fillColor;
	this->outlineColor = outlineColor;
	this->outlineThickness = outlineThickness;
	this->text = "";
	this->characterSize = 0;
}

drawOption::drawOption(Vector2f pos, Vector2f origin, float rotation, int opacity)
{
	//Sprite constructor
	this->pos = pos;
	this->shapeSize = Vector2f(0, 0);
	this->origin = origin;
	this->rotation = rotation;
	this->opacity = opacity;
	this->fillColor = Color(0,0,0);
	this->outlineColor = Color(0, 0, 0);
	this->outlineThickness = 0;
	this->text = "";
	this->characterSize = 0;
}

drawOption::drawOption(string text, int characterSize, Vector2f pos, Vector2f origin, Color fillColor, float rotation, int opacity, Color outlineColor, int outlineThickness)
{
	//Text constructor
	this->pos = pos;
	this->shapeSize = Vector2f(0, 0);
	this->origin = origin;
	this->rotation = rotation;
	this->opacity = opacity;
	this->fillColor = fillColor;
	this->outlineColor = outlineColor;
	this->outlineThickness = outlineThickness;
	this->text = text;
	this->characterSize = characterSize;
}

//FADE OPTION
fadeOption::fadeOption()
{
	this->directionStep = Vector2f(0, 0);
	this->opacityStep = 0.f;
	this->rotationStep = 0.f;
	this->duration = 120.f;
	this->speedReduction = 1;
	this->scaleStep = Vector2f(0, 0);
	this->opacityDelay = 0.f;
	this->stopAtEndAnimation = true;
}

fadeOption::fadeOption(Vector2f directionStep, float opacityStep, float rotationStep, float durationStep, float speedReduction, Vector2f scaleStep, float opacityDelay, bool stopAtEndAnimation)
{
	// Full fade option
	this->directionStep = directionStep;
	this->opacityStep = opacityStep >= 0.f ? opacityStep : 0.f;
	this->rotationStep = rotationStep;
	this->duration = durationStep > 0.f || directionStep != Vector2f(0,0) || opacityStep > 0 ? durationStep : 120.f;
	this->speedReduction = speedReduction < 0 || speedReduction > 1 ? 0.5 : speedReduction;
	this->scaleStep = scaleStep;
	this->opacityDelay = opacityDelay;
	this->stopAtEndAnimation = stopAtEndAnimation;
}

fadeOption::fadeOption(Vector2f directionStep, float durationStep, float speedReduction)
{
	// Fade based on direction
	this->directionStep = directionStep;
	this->opacityStep = 0.f;
	this->rotationStep = 0.f;
	this->duration = durationStep > 0.f || directionStep != Vector2f(0, 0) ? durationStep : 120.f;
	this->speedReduction = speedReduction < 0 || speedReduction > 1 ? 0.5 : speedReduction;
	this->scaleStep = Vector2f(0, 0);
	this->opacityDelay = 0.f;
	this->stopAtEndAnimation = true;
}

fadeOption::fadeOption(float opacityStep)
{
	// Fade based on opacity
	this->directionStep = Vector2f(0, 0);
	this->opacityStep = opacityStep >= 0.f ? opacityStep : 0.f;
	this->rotationStep = 0.f;
	this->duration = opacityStep > 0.f ? -1.f : 120.f;
	this->speedReduction = 1;
	this->scaleStep = Vector2f(0, 0);
	this->opacityDelay = 0.f;
	this->stopAtEndAnimation = true;
}

fadeOption::fadeOption(float rotationStep, float durationStep)
{
	// Fade based on duration (while rotating)
	this->directionStep = Vector2f(0, 0);
	this->opacityStep = 0.f;
	this->rotationStep = rotationStep;
	this->duration = durationStep > 0.f ? durationStep : 120.f;
	this->speedReduction = 1;
	this->scaleStep = Vector2f(0, 0);
	this->opacityDelay = 0.f;
	this->stopAtEndAnimation = true;
}

fadeOption::fadeOption(Vector2f scaleStep, float durationStep)
{
	// Fade based on duration (while scaling)
	this->directionStep = Vector2f(0, 0);
	this->opacityStep = 0.f;
	this->rotationStep = 0.f;
	this->duration = durationStep > 0.f ? durationStep : 120.f;
	this->speedReduction = 1;
	this->scaleStep = scaleStep;
	this->opacityDelay = 0.f;
	this->stopAtEndAnimation = true;
}

//PHYSICS
physics::physics(bool enabled, b2World* world, Vector2f impulse, float friction, float density, float gravityScale)
{
	this->enabled = enabled;
	this->world = world;
	this->impulse = impulse;
	this->friction = friction;
	this->density = density;
	this->gravityScale = gravityScale;
}

physics::physics(b2World * world, float gravityScale)
{
	this->enabled = true;
	this->world = world;
	this->impulse = Vector2f(0,0);
	this->friction = 0.5;
	this->density = 0.05;
	this->gravityScale = gravityScale;
}

//EFFECT OBJ
effectObj::~effectObj()
{
	if (_sprite != NULL)
		delete _sprite;
	if (_animation != NULL)
		delete _animation;
	if (_rect != NULL)
		delete _rect;
	if (_circle != NULL)
		delete _circle;
	if (_text != NULL)
		delete _text;
	if (_world != NULL && _body != NULL)
		_world->DestroyBody(_body);
}

effectObj::effectObj()
{
	Init();
}

void effectObj::Init(fadeOption fadeOption, float opacity)
{
	_fadeOption = fadeOption;
	_opacity = fabs(opacity);
	_finished = false;
	_effectType = EFFECT_NONE;
	_sprite = NULL;
	_animation = NULL;
	_text = NULL;
	_rect = NULL;
	_circle = NULL;
	_world = NULL;
	_body = NULL;
}

effectObj::effectObj(Texture * texture, IntRect textureRect, float rotation, Vector2f pos, Vector2f origin, float opacity, fadeOption fadeOption)
{
	//Testing texture
	if (texture == NULL)
		return;

	//Creating sprite object
	Init(fadeOption, opacity > 0.f ? opacity : 0.f);
	_effectType = EFFECT_SPRITE;

	//Sprite
	_sprite = new Sprite();
	_sprite->setTexture(*texture);
	_sprite->setTextureRect(textureRect);
	_sprite->setOrigin(origin);
	_sprite->setPosition(pos);
	_sprite->setRotation(rotation);
	_sprite->setColor(Color(255, 255, 255, _opacity));
}

effectObj::effectObj(Texture * texture, Vector2f tileSize, float frameDelay, int nbFrames, int numAnimation, bool reversed, 
				     float rotation, Vector2f pos, Vector2f origin, float opacity, fadeOption fadeOption, int loop)
{
	//Testing texture
	if (texture == NULL)
		return;

	//Creating sprite object with animation
	Init(fadeOption, opacity > 0.f ? opacity : 0.f);
	_effectType = EFFECT_SPRITE;

	//Sprite
	_sprite = new Sprite();
	_sprite->setTexture(*texture);
	_sprite->setOrigin(origin);
	_sprite->setPosition(pos);
	_sprite->setRotation(rotation);
	_sprite->setColor(Color(255, 255, 255, _opacity));

	//Animation
	_animation = new animation(_sprite, frameDelay, tileSize.x, tileSize.y, nbFrames, numAnimation + 1, -1);
	_animation->SetAnimation(numAnimation, loop >= 0 && loop <= nbFrames-1 ? loop : nbFrames-1);
	_animation->ReverseSprite(reversed);
}

effectObj::effectObj(float width, float height, Color fillColor, Color outlineColor, int outlineThickness,
					 float rotation, Vector2f pos, Vector2f origin, float opacity, fadeOption fadeOption)
{
	//Creating rectangle shape object
	Init(fadeOption, opacity > 0.f ? opacity : 0.f);
	_effectType = EFFECT_RECT;

	_rect = new RectangleShape(Vector2f(fabs(width), fabs(height)));
	_rect->setFillColor(Color(fillColor.r, fillColor.g, fillColor.b, fillColor.a == 0 ? fillColor.a : _opacity));
	_rect->setOutlineColor(Color(outlineColor.r, outlineColor.g, outlineColor.b, outlineColor.a == 0 ? outlineColor.a : _opacity));
	_rect->setOutlineThickness(fabs(outlineThickness));
	_rect->setOrigin(origin);
	_rect->setPosition(pos);
	_rect->setRotation(rotation);
}

effectObj::effectObj(float radius, Color fillColor, Color outlineColor, int outlineThickness,
					 Vector2f pos, Vector2f origin, float opacity, fadeOption fadeOption)
{
	//Creating circle shape object
	Init(fadeOption, opacity > 0.f ? opacity : 0.f);
	_effectType = EFFECT_CIRCLE;

	_circle = new CircleShape(radius);
	_circle->setFillColor(Color(fillColor.r, fillColor.g, fillColor.b, fillColor.a == 0 ? fillColor.a : _opacity));
	_circle->setOutlineColor(Color(outlineColor.r, outlineColor.g, outlineColor.b, outlineColor.a == 0 ? outlineColor.a : _opacity));
	_circle->setOutlineThickness(fabs(outlineThickness));
	_circle->setOrigin(origin);
	_circle->setPosition(pos);
}

effectObj::effectObj(string text, Font* font, int characterSize, Color fillColor, Color outlineColor, int outlineThickness,
					 float rotation, Vector2f pos, ALIGN textAlign, float opacity, fadeOption fadeOption)
{
	//Creating text object
	Init(fadeOption, opacity > 0.f ? opacity : 0.f);
	_effectType = EFFECT_TEXT;
	
	//Colors
	_text = new Text(text, *font, characterSize);
	_text->setFillColor(Color(fillColor.r, fillColor.g, fillColor.b, fillColor.a == 0 ? fillColor.a : _opacity));
	_text->setOutlineColor(Color(outlineColor.r, outlineColor.g, outlineColor.b, outlineColor.a == 0 ? outlineColor.a : _opacity));
	_text->setOutlineThickness(fabs(outlineThickness));

	//Text alignment
	if (textAlign == ALIGN_LEFT)
		_text->setOrigin(0.f, _text->getLocalBounds().height / 2.f);
	else if (textAlign == ALIGN_RIGHT)
		_text->setOrigin(_text->getLocalBounds().width, _text->getLocalBounds().height / 2.f);
	else if (textAlign == ALIGN_MIDDLE)
		_text->setOrigin(_text->getLocalBounds().width / 2.f, _text->getLocalBounds().height / 2.f);

	//Position + rotation
	_text->setPosition(pos);
	_text->setRotation(rotation);
}

void effectObj::InitPhysics(Vector2f impulse, float friction, float density, float gravityScale)
{
	if (_world == NULL) {
		cout << "World can't be NULL !" << endl;
		return;
	}
	switch (_effectType)
	{
		case EFFECT_SPRITE:
			// Changing origin to gravity center
			_sprite->setOrigin(_sprite->getLocalBounds().width / 2.f, _sprite->getLocalBounds().height / 2.f);

			//Creating Box2D body
			_body = Box2DTools::CreateRectangleBox(_world, _sprite->getPosition(), _sprite->getLocalBounds().width,
				_sprite->getLocalBounds().height, friction, density, _sprite->getRotation(), Box2DTools::PARTICLE_ENTITY);
			break;
		case EFFECT_RECT:
			// Changing origin to gravity center
			_rect->setOrigin(_rect->getLocalBounds().width / 2.f, _rect->getLocalBounds().height / 2.f);

			//Creating Box2D body
			_body = Box2DTools::CreateRectangleBox(_world, _rect->getPosition(), _rect->getLocalBounds().width,
				_rect->getLocalBounds().height, friction, density, _rect->getRotation(), Box2DTools::PARTICLE_ENTITY);
			break;
		case EFFECT_CIRCLE:
			// Changing origin to gravity center
			_circle->setOrigin(_circle->getLocalBounds().width / 2.f, _circle->getLocalBounds().height / 2.f);

			//Creating Box2D body
			_body = Box2DTools::CreateCircleBox(_world, _circle->getPosition(), _circle->getLocalBounds().width,
				friction, density, Box2DTools::PARTICLE_ENTITY);
			break;
		case EFFECT_TEXT:
			// Changing origin to gravity center
			_text->setOrigin(_text->getLocalBounds().width / 2.f, _text->getLocalBounds().height);

			//Creating Box2D body
			_body = Box2DTools::CreateRectangleBox(_world, _text->getPosition(), _text->getLocalBounds().width,
				_text->getLocalBounds().height, friction, density, _text->getRotation(), Box2DTools::PARTICLE_ENTITY);
			break;
	}

	// Changing gravity scale + Adding first body impulse
	if (_body != NULL)
	{
		_body->SetGravityScale(gravityScale);
		if(impulse != Vector2f(0,0))
			_body->SetLinearVelocity(b2Vec2(impulse.x / SCALE, impulse.y / SCALE));
	}
}

void effectObj::AddBox2DPhysics(physics physics)
{
	_world = physics.world;
	InitPhysics(physics.impulse, physics.friction, physics.density, physics.gravityScale);
}

void effectObj::AddBox2DPhysics(b2World * world, Vector2f impulse, float friction, float density, float gravityScale)
{
	_world = world;
	InitPhysics(impulse, friction, density, gravityScale);
}

void effectObj::Step(float deltaTime)
{
	try
	{
		if (_finished) { return; }

		// Duration incrementation
		if (_fadeOption.duration > 0.f)
			_fadeOption.duration = utils::StepCooldown(_fadeOption.duration, 1.f, deltaTime);

		// Moving / coloring / rotating effect object
		bool outsideScreen = false, endAnimation = false;
		Vector2f posObj = Vector2f(0,0);
		FloatRect boundObj = FloatRect(0,0,1,1);
		switch (_effectType)
		{
			case EFFECT_SPRITE:
				if (_sprite != NULL)
				{
					// Moving and rotating regarding physics (or not)
					if (_body == NULL)
					{
						_sprite->move(Vector2f(_fadeOption.directionStep.x*deltaTime, _fadeOption.directionStep.y*deltaTime));
						_sprite->setRotation(utils::StepRotation(_sprite->getRotation(), _fadeOption.rotationStep * deltaTime));
					}
					else
					{
						_sprite->setPosition(_body->GetPosition().x*SCALE, _body->GetPosition().y*SCALE);
						_sprite->setRotation(utils::RadianToDegree(_body->GetAngle()));
					}

					// Adding opacity effect
					if (_fadeOption.opacityDelay == 0.f || (_fadeOption.opacityDelay > 0.f && _fadeOption.duration <= _fadeOption.opacityDelay))
					{
						_opacity = utils::StepCooldown(_opacity, _fadeOption.opacityStep, deltaTime);
						_sprite->setColor(Color(255, 255, 255, _sprite->getColor().a == 0 ? 0 : _opacity));
					}

					// Changing scale
					if (fabs(_fadeOption.scaleStep.x) > 0.f || fabs(_fadeOption.scaleStep.y) > 0.f)
						_sprite->setScale(_sprite->getScale().x + _fadeOption.scaleStep.x*deltaTime,
										  _sprite->getScale().y + _fadeOption.scaleStep.y*deltaTime);

					// Reajusting scale if < 0
					if (_sprite->getScale().x < 0.f)
						_sprite->setScale(0, _sprite->getScale().y);
					if (_sprite->getScale().y < 0.f)
						_sprite->setScale(_sprite->getScale().x, 0);

					//Animation
					if (_animation != NULL)
					{
						_animation->PerformAnimation(deltaTime);
						endAnimation = _animation->isLastFrame();
					}

					// Getting pos and bound (outside view test)
					posObj = _sprite->getPosition();
					boundObj = _sprite->getLocalBounds();
				}
				break;
			case EFFECT_RECT:
				if (_rect != NULL)
				{
					// Moving and rotating regarding physics (or not)
					if (_body == NULL)
					{
						_rect->move(Vector2f(_fadeOption.directionStep.x*deltaTime, _fadeOption.directionStep.y*deltaTime));
						_rect->setRotation(utils::StepRotation(_rect->getRotation(), _fadeOption.rotationStep * deltaTime));
					}
					else
					{
						_rect->setPosition(_body->GetPosition().x*SCALE, _body->GetPosition().y*SCALE);
						_rect->setRotation(utils::RadianToDegree(_body->GetAngle()));
					}

					// Adding opacity effect
					if (_fadeOption.opacityDelay == 0.f || (_fadeOption.opacityDelay > 0.f && _fadeOption.duration <= _fadeOption.opacityDelay))
					{
						_opacity = utils::StepCooldown(_opacity, _fadeOption.opacityStep, deltaTime);
						_rect->setFillColor(Color(_rect->getFillColor().r, _rect->getFillColor().g,
							_rect->getFillColor().b, _rect->getFillColor().a == 0 ? 0 : _opacity));
						_rect->setOutlineColor(Color(_rect->getOutlineColor().r, _rect->getOutlineColor().g,
							_rect->getOutlineColor().b, _rect->getOutlineColor().a == 0 ? 0 : _opacity));
					}

					// Changing scale
					if (fabs(_fadeOption.scaleStep.x) > 0.f || fabs(_fadeOption.scaleStep.y) > 0.f)
						_rect->setScale(_rect->getScale().x + _fadeOption.scaleStep.x*deltaTime,
										_rect->getScale().y + _fadeOption.scaleStep.y*deltaTime);

					// Reajusting scale if < 0
					if (_rect->getScale().x < 0.f)
						_rect->setScale(0, _rect->getScale().y);
					if (_rect->getScale().y < 0.f)
						_rect->setScale(_rect->getScale().x, 0);
					
					// Getting pos and bound (outside view test)
					posObj = _rect->getPosition();
					boundObj = _rect->getLocalBounds();
				}
				break;
			case EFFECT_CIRCLE:
				if (_circle != NULL)
				{
					// Moving and rotating regarding physics (or not)
					if (_body == NULL)
						_circle->move(Vector2f(_fadeOption.directionStep.x*deltaTime, _fadeOption.directionStep.y*deltaTime));
					else
						_circle->setPosition(_body->GetPosition().x*SCALE, _body->GetPosition().y*SCALE);

					// Adding opacity effect
					if (_fadeOption.opacityDelay == 0.f || (_fadeOption.opacityDelay > 0.f && _fadeOption.duration <= _fadeOption.opacityDelay))
					{
						_opacity = utils::StepCooldown(_opacity, _fadeOption.opacityStep, deltaTime);
						_circle->setFillColor(Color(_circle->getFillColor().r, _circle->getFillColor().g,
							_circle->getFillColor().b, _circle->getFillColor().a == 0 ? 0 : _opacity));
						_circle->setOutlineColor(Color(_circle->getOutlineColor().r, _circle->getOutlineColor().g,
							_circle->getOutlineColor().b, _circle->getOutlineColor().a == 0 ? 0 : _opacity));
					}

					// Changing scale
					if (fabs(_fadeOption.scaleStep.x) > 0.f || fabs(_fadeOption.scaleStep.y) > 0.f)
						_circle->setScale(_circle->getScale().x + _fadeOption.scaleStep.x*deltaTime,
										  _circle->getScale().y + _fadeOption.scaleStep.y*deltaTime);

					// Reajusting scale if < 0
					if (_circle->getScale().x < 0.f)
						_circle->setScale(0, _circle->getScale().y);
					if (_circle->getScale().y < 0.f)
						_circle->setScale(_circle->getScale().x, 0);
					
					// Getting pos and bound (outside view test)
					posObj = _circle->getPosition();
					boundObj = _circle->getLocalBounds();
				}
				break;
			case EFFECT_TEXT:
				if (_text != NULL)
				{
					// Moving and rotating regarding physics (or not)
					if (_body == NULL)
					{
						_text->move(Vector2f(_fadeOption.directionStep.x*deltaTime, _fadeOption.directionStep.y*deltaTime));
						_text->setRotation(utils::StepRotation(_text->getRotation(), _fadeOption.rotationStep * deltaTime));
					}
					else
					{
						_text->setPosition(_body->GetPosition().x*SCALE, _body->GetPosition().y*SCALE);
						_text->setRotation(utils::RadianToDegree(_body->GetAngle()));
					}

					// Adding opacity effect
					if (_fadeOption.opacityDelay == 0.f || (_fadeOption.opacityDelay > 0.f && _fadeOption.duration <= _fadeOption.opacityDelay))
					{
						_opacity = utils::StepCooldown(_opacity, _fadeOption.opacityStep, deltaTime);
						_text->setFillColor(Color(_text->getFillColor().r, _text->getFillColor().g,
							_text->getFillColor().b, _text->getFillColor().a == 0 ? 0 : _opacity));
						_text->setOutlineColor(Color(_text->getOutlineColor().r, _text->getOutlineColor().g,
							_text->getOutlineColor().b, _text->getOutlineColor().a == 0 ? 0 : _opacity));
					}

					// Changing scale
					if (fabs(_fadeOption.scaleStep.x) > 0.f || fabs(_fadeOption.scaleStep.y) > 0.f)
						_text->setScale(_text->getScale().x + _fadeOption.scaleStep.x*deltaTime,
										_text->getScale().y + _fadeOption.scaleStep.y*deltaTime);

					// Reajusting scale if < 0
					if (_text->getScale().x < 0.f)
						_text->setScale(0, _text->getScale().y);
					if (_text->getScale().y < 0.f)
						_text->setScale(_text->getScale().x, 0);
					
					// Getting pos and bound (outside view test)
					posObj = _text->getPosition();
					boundObj = _text->getLocalBounds();
				}
				break;
		}

		// Speed reduction
		if(_fadeOption.speedReduction > 0.f && _fadeOption.speedReduction < 1.f)
			_fadeOption.directionStep = Vector2f(
				_fadeOption.directionStep.x - ((_fadeOption.directionStep.x*_fadeOption.speedReduction)*deltaTime),
				_fadeOption.directionStep.y - ((_fadeOption.directionStep.y*_fadeOption.speedReduction)*deltaTime));

		// Testing if outside screen
		if (_fadeOption.directionStep != Vector2f(0, 0) && (
			posObj.x + boundObj.width < 0.f  || posObj.x - boundObj.width > utils::VIEW_WIDTH ||
			posObj.y + boundObj.height < 0.f || posObj.y - boundObj.height > utils::VIEW_HEIGHT))
			outsideScreen = true;

		// Destroying finished effect
		if (outsideScreen || (endAnimation && _fadeOption.stopAtEndAnimation) || _opacity <= 0.f || _fadeOption.duration == 0.f)
			_finished = true;
	}
	catch (exception e)
	{
		cout << e.what() << endl;
	}
}

bool effectObj::IsFinished()
{
	return _finished;
}

Sprite * effectObj::GetSprite()
{
	return _sprite;
}

void effectObj::Display(RenderWindow* window)
{
	if (_finished) { return; }

	switch (_effectType)
	{
		case EFFECT_SPRITE:
			if (_sprite != NULL)
				window->draw(*_sprite);
			break;
		case EFFECT_RECT:
			if (_rect != NULL)
				window->draw(*_rect);
			break;
		case EFFECT_CIRCLE:
			if (_circle != NULL)
				window->draw(*_circle);
			break;
		case EFFECT_TEXT:
			if (_text != NULL)
				window->draw(*_text);
			break;
	}
}
