#include "teeterLamp.hpp"

teeterLamp::~teeterLamp()
{
	for (int i = 0; i < _lightsShapes.size(); i++)
		if (_lightsShapes[i] != NULL)
			delete _lightsShapes[i];
}

teeterLamp::teeterLamp()
{
}

void teeterLamp::LoadLights()
{
	//Lights
	for (int i = 0; i < _nodesSprites.size(); i++)
	{
		//Circle shape
		float radius = _lightRadius;// max(_nodesSprites[i]->getLocalBounds().width, _nodesSprites[i]->getLocalBounds().height);
		_lightsShapes.push_back(new CircleShape(radius / 2.f));
		_lightsShapes[i]->setOrigin(_nodesSprites[i]->getOrigin());
		_lightsShapes[i]->setPosition(Vector2f(_nodesSprites[i]->getPosition().x, _nodesSprites[i]->getPosition().y));
		_lightsShapes[i]->setFillColor(Color(255, 255, 215, 40));
		_lightsShapes[i]->setOutlineColor(Color(255, 255, 215, 40));
		_lightsShapes[i]->setOutlineThickness(radius);

		//Border animation
		pair<float, float> lightBorder;
		lightBorder.first = radius;
		lightBorder.second = radius * 1.1f;
		_lightsShapesBorders.push_back(lightBorder);
	}

	//Changing texture if is not lit
	if (!_isLit)
	{
		if (_rootSprite != NULL)
			_rootSprite->setTexture(*_lampOffTexture);
		for (int i = 0; i < _nodesSprites.size(); i++)
			if (_nodesSprites[i] != NULL)
				_nodesSprites[i]->setTexture(*_lampOffTexture);
	}
}

void teeterLamp::StepChild(float deltaTime)
{
	//Moving and rotating lights
	for (int i = 0; i < _lightsShapes.size(); i++)
	{
		if (_lightsShapes[i] != NULL && i < _nodesSprites.size() && _nodesSprites[i] != NULL)
		{
			_lightsShapes[i]->setPosition(Vector2f(_nodesSprites[i]->getPosition().x  + _lightsRelativePos.x, 
				_nodesSprites[i]->getPosition().y + _lightsRelativePos.y));
			_lightsShapes[i]->setRotation(_nodesSprites[i]->getRotation());
		}

		//Animating lights
		if (_isLit)
		{
			float bInit = _lightsShapesBorders[i].first;
			float bReach = _lightsShapesBorders[i].second;
			float bGet = _lightsShapes[i]->getOutlineThickness();
			if ((bReach > bInit && bGet >= bReach) || (bReach == bInit && bGet <= bReach))
				_lightsShapesBorders[i].second = bReach > bInit ? bInit : bInit * 1.1f;
			else if (bGet < bReach)
				_lightsShapes[i]->setOutlineThickness(bGet + 0.02f * deltaTime);
			else if (bGet > bReach)
				_lightsShapes[i]->setOutlineThickness(bGet - 0.02f*deltaTime);
		}
	}
}

void teeterLamp::DisplayChild(RenderWindow * window)
{
	//If lamp is not lit, nothing to do
	if (!_isLit)
		return;

	//Changing texture if lit or not
	if (_rootSprite != NULL)
		_rootSprite->setTexture(_root.angle <= 0 ? *_teeterTexture : *_lampOffTexture);
	for (int i = 0; i < _nodesSprites.size(); i++)
		if (_nodesSprites[i] != NULL)
			_nodesSprites[i]->setTexture(_root.angle <= 0 ? *_teeterTexture : *_lampOffTexture);

	//Light
	for (int i = 0; i < _lightsShapes.size(); i++)
		if (_lightsShapes[i] != NULL && _root.angle <= 0)
			utils::DrawLoopShape(window, _lightsShapes[i]);
}