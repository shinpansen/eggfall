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

	//Textures - font
    if (!_spellsTexture.loadFromFile(files::spellExplosionTile))
        cout << "Unable to find spells texture '" << files::spellExplosionTile << "'" << endl;
    if (!_dyingTexture.loadFromFile(files::wizardDying))
        cout << "Unable to find wizard dying texture '" << files::wizardDying << "'" << endl;
	_font.loadFromFile(files::font2);
}

void effects::AddSprite(TEXTURE_ID textureId, Vector2i animRowCol, drawOption drawOption, fadeOption fadeOption, physics physics)
{
	// Adding new sprite effect
	_effectObjects.push_back(new effectObj(
		GetTexture(textureId),
		GetTextureRect(textureId, animRowCol),
		drawOption.rotation,
		drawOption.pos,
		drawOption.origin,
		drawOption.opacity,
		fadeOption));

	// Adding physics if enabled
	if (physics.enabled)
		_effectObjects[_effectObjects.size() - 1]->AddBox2DPhysics(physics);
}

void effects::AddCustomSprite(Texture * texture, IntRect rect, drawOption drawOption, fadeOption fadeOption, physics physics)
{
	// Adding new sprite effect
	_effectObjects.push_back(new effectObj(
		texture,
		rect,
		drawOption.rotation,
		drawOption.pos,
		drawOption.origin,
		drawOption.opacity,
		fadeOption));

	// Adding physics if enabled
	if (physics.enabled)
		_effectObjects[_effectObjects.size() - 1]->AddBox2DPhysics(physics);
}

Sprite* effects::AddAnimation(TEXTURE_ID textureId, int numAnimation, float frameDelay, bool reversed, drawOption drawOption, fadeOption fadeOption, physics physics)
{
	//Getting default IntRect + calculating number of frames
	IntRect rect = GetTextureRect(textureId, Vector2i(0, 0));
	int nbFrames = GetTexture(textureId)->getSize().x / rect.width;

	// Adding new sprite effect
	_effectObjects.push_back(new effectObj(
		GetTexture(textureId),
		Vector2f(rect.width, rect.height),
		frameDelay,
		nbFrames,
		numAnimation,
		reversed,
		drawOption.rotation,
		drawOption.pos,
		Vector2f(rect.width/2.f, rect.height/2.f),
		drawOption.opacity,
		fadeOption));

	// Adding physics if enabled
	if (physics.enabled)
		_effectObjects[_effectObjects.size() - 1]->AddBox2DPhysics(physics);

	//Return sprite pointer
	return _effectObjects[_effectObjects.size() - 1]->GetSprite();
}

void effects::AddRectangleShape(drawOption drawOption, fadeOption fadeOption, physics physics)
{
	// Adding new rectangle shape effect
	_effectObjects.push_back(new effectObj(
		drawOption.shapeSize.x,
		drawOption.shapeSize.y,
		drawOption.fillColor,
		drawOption.outlineColor,
		drawOption.outlineThickness,
		drawOption.rotation,
		drawOption.pos,
		drawOption.origin,
		drawOption.opacity,
		fadeOption));

	// Adding physics if enabled
	if (physics.enabled)
		_effectObjects[_effectObjects.size() - 1]->AddBox2DPhysics(physics);
}

void effects::AddCircleShape(drawOption drawOption, fadeOption fadeOption, physics physics)
{
	// Adding new circle shape effect
	_effectObjects.push_back(new effectObj(
		drawOption.shapeSize.x,
		drawOption.fillColor,
		drawOption.outlineColor,
		drawOption.outlineThickness,
		drawOption.pos,
		drawOption.origin,
		drawOption.opacity,
		fadeOption));

	// Adding physics if enabled
	if (physics.enabled)
		_effectObjects[_effectObjects.size() - 1]->AddBox2DPhysics(physics);
}

void effects::AddText(drawOption drawOption, fadeOption fadeOption, physics physics)
{
	// Adding new text effect
	_effectObjects.push_back(new effectObj(
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
		fadeOption));

	// Adding physics if enabled
	if (physics.enabled)
		_effectObjects[_effectObjects.size() - 1]->AddBox2DPhysics(physics);
}

void effects::AddDust(Vector2f pos, bool clockwise, bool isStatic, float speedReduction)
{
	try
	{
		//Random size and rotation speed
		int width = rand() % 4 + 2;
		int rotationSpeed = rand() % 15 + 5;
		int posX = rand() % 6 - 3;
		int posY = rand() % 6 - 3;
		float speedX = clockwise ? -fabs((float)posX*2.f) : fabs((float)posX*2.f);

		//Adding dust
		AddRectangleShape(drawOption(
			Vector2f(pos.x + posX, pos.y + posY),
			Vector2f(width, width),
			Vector2f(width / 2, width / 2),
			Color(235, 235, 200, 255)),
			fadeOption(
				isStatic ? Vector2f(0, 0) : Vector2f(speedX, -fabs((float)posY)),
				5.f,
				clockwise ? rotationSpeed : -rotationSpeed,
				666,
				speedReduction),
			physics(false));
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

Texture * effects::GetTexture(TEXTURE_ID textureId)
{
	if (textureId == T_SPELLS)
		return &_spellsTexture;
	else if (textureId == T_DEAD)
		return &_dyingTexture;
	else
		return NULL;
}

IntRect effects::GetTextureRect(TEXTURE_ID textureId, Vector2i animRowCol)
{
	switch (textureId)
	{
		case T_SPELLS:
			if (animRowCol.x < 0 || animRowCol.y < 0 ||
				animRowCol.x > 5 || animRowCol.y > 9)
				return IntRect(0, 0, _spellsTexture.getSize().x, _spellsTexture.getSize().y);
			else
				return IntRect(animRowCol.x * 75, animRowCol.y * 75, 75, 75);
		case T_DEAD:
			if (animRowCol.x < 0 || animRowCol.y < 0 ||
				animRowCol.x > 8 || animRowCol.y > 0)
				return IntRect(0, 0, _dyingTexture.getSize().x, _dyingTexture.getSize().y);
			else
				return IntRect(animRowCol.x * 68, animRowCol.y * 100, 68, 100);
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

void effects::RenderEffects(b2World* world, bool fullscreen, float deltaTime)
{
    try
    {
        _deltaTime = deltaTime;

		// Display + Step + delete effect objects
		vector<effectObj*> effectObjectsClean; // Clean vector without NULL objects
		for (int i = 0; i < _effectObjects.size(); i++)
		{
			if (_effectObjects[i] != NULL && _effectObjects[i]->IsFinished())
			{
				delete _effectObjects[i];
				_effectObjects[i] = NULL;
			}
			else if (_effectObjects[i] != NULL)
			{
				_effectObjects[i]->Step(deltaTime);
				//_effectObjects[i]->Display(window);
				effectObjectsClean.push_back(_effectObjects[i]);
			}
		}
		_effectObjects = effectObjectsClean;

		//Shake screen effect
		/*View view = GetDefaultView(fullscreen);
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
		}*/
    }
    catch(exception e)
    {
        cout << e.what() << endl;
    }
}

void effects::Display(RenderWindow* window, bool fullscreen, float deltaTime)
{
	//Center view (cancel screen shake)
	/*View view = GetDefaultView(fullscreen);
	Vector2f viewDefault = Vector2f(utils::VIEW_WIDTH / 2.f, utils::VIEW_HEIGHT / 2.f);
	view.setCenter(viewDefault);
	window->setView(view);*/

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
	for (int i = 0; i < _effectObjects.size(); i++)
		if (_effectObjects[i] != NULL)
			_effectObjects[i]->Display(window);
}

