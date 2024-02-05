#include "effects.hpp"

static const float SCALE = 30.f;

using namespace std;
using namespace sf;

effects::effects()
{
	//Variables
	_shakeFrame = 0;
	_shakeSpeed = 1.f;
	_shakePositions[0] = Vector2f(0.f, 0.f);

	//Graphics
	_flashShape.setSize(Vector2f(utils::VIEW_WIDTH, utils::VIEW_HEIGHT));
	if (!_spellsTexture.loadFromFile(files::spellTile))
		cout << "Unable to find spells texture '" << files::spellExplosionTile << "'" << endl;
    if (!_spellsExplosionTexture.loadFromFile(files::spellExplosionTile))
        cout << "Unable to find spells explosions texture '" << files::spellExplosionTile << "'" << endl;
	if (!_hitTexture.loadFromFile(files::hitEffectTile))
		cout << "Unable to find hit effect texture '" << files::hitEffectTile << "'" << endl;
	if (!_shieldTexture.loadFromFile(files::shieldEffectTile))
		cout << "Unable to find shield effect texture '" << files::shieldEffectTile << "'" << endl;
	if (!_grindEffectTexture.loadFromFile(files::grindEffectTile))
		cout << "Unable to find grind effect texture '" << files::grindEffectTile << "'" << endl;
    if (!_dyingTexture.loadFromFile(files::wizardDying))
        cout << "Unable to find wizard dying texture '" << files::wizardDying << "'" << endl;
	if (!_dustTexture.loadFromFile(files::dustEffectTile))
		cout << "Unable to find dust effect texture '" << files::dustEffectTile << "'" << endl;
	_font.loadFromFile(files::font2);
}

void effects::AddSprite(TEXTURE_ID textureId, Vector2i animRowCol, drawOption drawOption, fadeOption fadeOption, physics physics, bool background)
{
	// Adding new sprite effect
	effectObj * sfx = new effectObj(
		GetTexture(textureId),
		GetTextureRect(textureId, animRowCol),
		drawOption.rotation,
		drawOption.pos,
		drawOption.origin,
		drawOption.opacity,
		fadeOption);
	
	//Push
	if (background)
		_effectObjectsBack.push_back(sfx);
	else
		_effectObjectsFront.push_back(sfx);

	// Adding physics if enabled
	if (physics.enabled)
		sfx->AddBox2DPhysics(physics);
}

void effects::AddCustomSprite(Texture * texture, IntRect rect, drawOption drawOption, fadeOption fadeOption, physics physics, bool background)
{
	// Adding new sprite effect
	effectObj * sfx = new effectObj(
		texture,
		rect,
		drawOption.rotation,
		drawOption.pos,
		drawOption.origin,
		drawOption.opacity,
		fadeOption);
	
	//Push
	if (background)
		_effectObjectsBack.push_back(sfx);
	else
		_effectObjectsFront.push_back(sfx);

	// Adding physics if enabled
	if (physics.enabled)
		sfx->AddBox2DPhysics(physics);
}

Sprite* effects::AddAnimation(TEXTURE_ID textureId, int numAnimation, float frameDelay, bool reversed, drawOption drawOption, fadeOption fadeOption, physics physics, int loop, bool background)
{
	//Getting default IntRect + calculating number of frames
	IntRect rect = GetTextureRect(textureId, Vector2i(0, 0));
	int nbFrames = GetTexture(textureId)->getSize().x / rect.width;

	// Adding new sprite effect
	effectObj * sfx = new effectObj(
		GetTexture(textureId),
		Vector2f(rect.width, rect.height),
		frameDelay,
		nbFrames,
		numAnimation,
		reversed,
		drawOption.rotation,
		drawOption.pos,
		Vector2f(rect.width / 2.f, rect.height / 2.f),
		drawOption.opacity,
		fadeOption,
		loop);

	//Push
	if(background)
		_effectObjectsBack.push_back(sfx);
	else
		_effectObjectsFront.push_back(sfx);

	// Adding physics if enabled
	if (physics.enabled)
		sfx->AddBox2DPhysics(physics);

	//Return sprite pointer
	return sfx->GetSprite();
}

void effects::AddRectangleShape(drawOption drawOption, fadeOption fadeOption, physics physics, bool background)
{
	// Adding new rectangle shape effect
	effectObj * sfx = new effectObj(
		drawOption.shapeSize.x,
		drawOption.shapeSize.y,
		drawOption.fillColor,
		drawOption.outlineColor,
		drawOption.outlineThickness,
		drawOption.rotation,
		drawOption.pos,
		drawOption.origin,
		drawOption.opacity,
		fadeOption);
	
	//Push
	if (background)
		_effectObjectsBack.push_back(sfx);
	else
		_effectObjectsFront.push_back(sfx);

	// Adding physics if enabled
	if (physics.enabled)
		sfx->AddBox2DPhysics(physics);
}

void effects::AddCircleShape(drawOption drawOption, fadeOption fadeOption, physics physics, bool background)
{
	// Adding new circle shape effect
	effectObj * sfx = new effectObj(
		drawOption.shapeSize.x,
		drawOption.fillColor,
		drawOption.outlineColor,
		drawOption.outlineThickness,
		drawOption.pos,
		drawOption.origin,
		drawOption.opacity,
		fadeOption);
	
	//Push
	if (background)
		_effectObjectsBack.push_back(sfx);
	else
		_effectObjectsFront.push_back(sfx);

	// Adding physics if enabled
	if (physics.enabled)
		sfx->AddBox2DPhysics(physics);
}

void effects::AddText(drawOption drawOption, fadeOption fadeOption, physics physics, bool background)
{
	// Adding new text effect
	effectObj * sfx = new effectObj(
		drawOption.text,
		&_font,
		drawOption.characterSize,
		drawOption.fillColor,
		drawOption.outlineColor,
		drawOption.outlineThickness,
		drawOption.rotation,
		drawOption.pos,
		ALIGN_MIDDLE,
		drawOption.opacity,
		fadeOption);
	
	//Push
	if (background)
		_effectObjectsBack.push_back(sfx);
	else
		_effectObjectsFront.push_back(sfx);

	// Adding physics if enabled
	if (physics.enabled)
		sfx->AddBox2DPhysics(physics);
}

void effects::AddDust(Vector2f pos, bool clockwise, bool isStatic, float speedReduction, bool background)
{
	try
	{
		//Random size and rotation speed
		int width = rand() % 3 + 1;
		int rotationSpeed = rand() % 15 + 5;
		int posX = rand() % 6 - 3;
		int posY = rand() % 6 - 3;
		float speedX = clockwise ? -fabs((float)posX*2.f) : fabs((float)posX*2.f);

		//Adding dust
		AddCircleShape(drawOption(
			Vector2f(pos.x + posX, pos.y + posY),
			Vector2f(width, width),
			Vector2f(width / 2, width / 2),
			Color::White),
			//Color(235, 235, 200, 255)),
			fadeOption(
				isStatic ? Vector2f(0, 0) : Vector2f(speedX, -fabs((float)posY)),
				5.f,
				clockwise ? rotationSpeed : -rotationSpeed,
				666,
				speedReduction),
			physics(false),
			background);
	}
	catch (exception e)
	{
		cout << "An error occurred while adding dust : " << e.what() << endl;
	}
}

void effects::ShakeScreen(int amplitute, float speed, SHAKE_TYPE shakeType)
{
	_shakeFrame = 4;
	_shakeSpeed = speed < 1 ? 1 : speed;

	//Random offsets
	int signe = utils::RandomNumber(1, 0);
	signe = signe == 0 ? -1 : signe;
	int xOffSet = amplitute;// signe*utils::RandomNumber(amplitute, 5);
	int yOffSet = amplitute;//signe*utils::RandomNumber(amplitute, 5);

	//Shake type
	if (shakeType == SHAKE_HORIZONTAL)
		yOffSet = 0.f;
	else if (shakeType == SHAKE_VERTICAL)
		xOffSet = 0.f;

	//Update shake corners to reach
	_shakePositions[1] = Vector2f(xOffSet, yOffSet);
	_shakePositions[2] = Vector2f(-xOffSet, -yOffSet);
	_shakePositions[3] = Vector2f(-xOffSet, yOffSet);
	_shakePositions[4] = Vector2f(xOffSet, -yOffSet);
}

void effects::FlashScreen(float duration)
{
	_flashScreen = duration < 5.f ? 5.f : duration;
}

Texture * effects::GetTexture(TEXTURE_ID textureId)
{
	if (textureId == T_SPELLS)
		return &_spellsTexture;
	else if (textureId == T_SPELLS_EXPLOSION)
		return &_spellsExplosionTexture;
	else if (textureId == T_HIT)
		return &_hitTexture;
	else if (textureId == T_SHIELD)
		return &_shieldTexture;
	else if (textureId == T_GRIND)
		return &_grindEffectTexture;
	else if (textureId == T_DEAD)
		return &_dyingTexture;
	else if (textureId == T_DUST)
		return &_dustTexture;
	else
		return NULL;
}

IntRect effects::GetTextureRect(TEXTURE_ID textureId, Vector2i animRowCol)
{
	switch (textureId)
	{
		case T_SPELLS:
			if (animRowCol.x < 0 || animRowCol.y < 0 ||
				animRowCol.x > 5 || animRowCol.y > 10)
				return IntRect(0, 0, _spellsTexture.getSize().x, _spellsTexture.getSize().y);
			else
				return IntRect(animRowCol.x * 48, animRowCol.y * 48, 48, 48);
		case T_SPELLS_EXPLOSION:
			if (animRowCol.x < 0 || animRowCol.y < 0 ||
				animRowCol.x > 5 || animRowCol.y > 9)
				return IntRect(0, 0, _spellsExplosionTexture.getSize().x, _spellsExplosionTexture.getSize().y);
			else
				return IntRect(animRowCol.x * 75, animRowCol.y * 75, 75, 75);
		case T_HIT:
			if (animRowCol.x < 0 || animRowCol.y < 0 ||
				animRowCol.x > 5 || animRowCol.y > 1)
				return IntRect(0, 0, _hitTexture.getSize().x, _hitTexture.getSize().y);
			else
				return IntRect(animRowCol.x * 96, animRowCol.y * 96, 96, 96);
		case T_SHIELD:
			if (animRowCol.x < 0 || animRowCol.y < 0 ||
				animRowCol.x > 11 || animRowCol.y > 1)
				return IntRect(0, 0, _shieldTexture.getSize().x, _shieldTexture.getSize().y);
			else
				return IntRect(animRowCol.x * 48, animRowCol.y * 48, 48, 48);
		case T_GRIND:
			if (animRowCol.x < 0 || animRowCol.y < 0 ||
				animRowCol.x > 5 || animRowCol.y > 0)
				return IntRect(0, 0, _grindEffectTexture.getSize().x, _grindEffectTexture.getSize().y);
			else
				return IntRect(animRowCol.x * 150, animRowCol.y * 150, 150, 150);
		case T_DEAD:
			if (animRowCol.x < 0 || animRowCol.y < 0 ||
				animRowCol.x > 8 || animRowCol.y > 0)
				return IntRect(0, 0, _dyingTexture.getSize().x, _dyingTexture.getSize().y);
			else
				return IntRect(animRowCol.x * 68, animRowCol.y * 100, 68, 100);
		case T_DUST:
			if (animRowCol.x < 0 || animRowCol.y < 0 ||
				animRowCol.x > 5 || animRowCol.y > 3)
				return IntRect(0, 0, _dustTexture.getSize().x, _dustTexture.getSize().y);
			else
				return IntRect(animRowCol.x * 32, animRowCol.y * 32, 32, 32);
	}
}

View effects::GetDefaultView(bool fullscreen)
{
	Vector2i screenSize;
	if (fullscreen)
		screenSize = Vector2i(VideoMode::getDesktopMode().width, VideoMode::getDesktopMode().height);
	else
		screenSize = Vector2i(utils::VIEW_WIDTH, utils::VIEW_HEIGHT);

	FloatRect rect;
	if (utils::VIEW_HEIGHT*screenSize.x / screenSize.y >= utils::VIEW_WIDTH)
		rect = FloatRect(0, 0, utils::VIEW_HEIGHT*screenSize.x / screenSize.y, utils::VIEW_HEIGHT);
	else
		rect = FloatRect(0, 0, utils::VIEW_WIDTH, utils::VIEW_WIDTH * screenSize.y / screenSize.x);
	View view(rect);
	return view;
}

void effects::DestroyEverything()
{
	//FRONT
	for (int i = 0; i < _effectObjectsFront.size(); i++)
	{
		if (_effectObjectsFront[i] != NULL)
		{
			delete _effectObjectsFront[i];
			_effectObjectsFront[i] = NULL;
		}
	}
	_effectObjectsFront.clear();

	//BACK
	for (int i = 0; i < _effectObjectsBack.size(); i++)
	{
		if (_effectObjectsBack[i] != NULL)
		{
			delete _effectObjectsBack[i];
			_effectObjectsBack[i] = NULL;
		}
	}
	_effectObjectsBack.clear();
}

void effects::RenderEffects(b2World* world, bool fullscreen, float deltaTime)
{
    try
    {
        _deltaTime = deltaTime;

		// Display + Step + delete effect objects
		// FRONT
		vector<effectObj*> effectObjectsFrontClean; // Clean vector without NULL objects
		for (int i = 0; i < _effectObjectsFront.size(); i++)
		{
			if (_effectObjectsFront[i] != NULL && _effectObjectsFront[i]->IsFinished())
			{
				delete _effectObjectsFront[i];
				_effectObjectsFront[i] = NULL;
			}
			else if (_effectObjectsFront[i] != NULL)
			{
				_effectObjectsFront[i]->Step(deltaTime);
				effectObjectsFrontClean.push_back(_effectObjectsFront[i]);
			}
		}
		_effectObjectsFront = effectObjectsFrontClean;

		// BACK
		vector<effectObj*> effectObjectsBackClean;
		for (int i = 0; i < _effectObjectsBack.size(); i++)
		{
			if (_effectObjectsBack[i] != NULL && _effectObjectsBack[i]->IsFinished())
			{
				delete _effectObjectsBack[i];
				_effectObjectsBack[i] = NULL;
			}
			else if (_effectObjectsBack[i] != NULL)
			{
				_effectObjectsBack[i]->Step(deltaTime);
				effectObjectsBackClean.push_back(_effectObjectsBack[i]);
			}
		}
		_effectObjectsBack = effectObjectsBackClean;

		//Flash screen step
		_flashScreen = utils::StepCooldown(_flashScreen, 1.f, deltaTime);
    }
    catch(exception e)
    {
        cout << e.what() << endl;
    }
}

void effects::Display(RenderWindow * window, bool fullscreen, float deltaTime)
{
	DisplayBack(window, fullscreen, deltaTime);
	DisplayFront(window, fullscreen, deltaTime);
}

void effects::DisplayFront(RenderWindow* window, bool fullscreen, float deltaTime)
{
	//Shake screen effect
    View view = GetDefaultView(fullscreen);
    Vector2f viewDefault = Vector2f(utils::VIEW_WIDTH / 2.f, utils::VIEW_HEIGHT / 2.f);
    Vector2f viewCenter = window->getView().getCenter();
    if (viewDefault != viewCenter || _shakeFrame > 0)
    {
        Vector2f centerToReach = Vector2f(viewDefault.x + _shakePositions[_shakeFrame].x, viewDefault.y + _shakePositions[_shakeFrame].y);
        viewCenter.x = utils::StepCooldown(viewCenter.x, _shakeSpeed, deltaTime, centerToReach.x);
        viewCenter.y = utils::StepCooldown(viewCenter.y, _shakeSpeed, deltaTime, centerToReach.y);
        if (viewCenter == centerToReach && _shakeFrame > 0)
            _shakeFrame--;
        view.setCenter(viewCenter);
        window->setView(view);
    }

	// Drawing effect objects
	for (int i = 0; i < _effectObjectsFront.size(); i++)
		if (_effectObjectsFront[i] != NULL)
			_effectObjectsFront[i]->Display(window);

	//Flash screen
	if (_flashScreen > 0.f)
	{
		_flashShape.setFillColor(Color(255, 255, 255, _flashScreen > 4.f ? 255 : 200 / (5.f - _flashScreen)));
		window->draw(_flashShape);
	}
}

void effects::DisplayBack(RenderWindow* window, bool fullscreen, float deltaTime)
{
	// Drawing effect objects
	for (int i = 0; i < _effectObjectsBack.size(); i++)
		if (_effectObjectsBack[i] != NULL)
			_effectObjectsBack[i]->Display(window);
}

