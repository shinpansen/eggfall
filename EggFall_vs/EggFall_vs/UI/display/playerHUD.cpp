#include "playerHUD.hpp"

using namespace std;
using namespace sf;

playerHUD::~playerHUD()
{
	//Destroy life sprites
	for (int i = 0; i < _lifeSprites.size(); i++)
		if (_lifeSprites[i] != NULL)
			delete _lifeSprites[i];

	//Destroy stamina objects
	if (_staminaBar != NULL)
		delete _staminaBar;
	if (_staminaShapeFront != NULL)
		delete _staminaShapeFront;
	if (_staminaShapeFront2 != NULL)
		delete _staminaShapeFront2;
	if (_staminaShapeBack != NULL)
		delete _staminaShapeBack;
}

playerHUD::playerHUD(Texture * lifeStaminaTexture, int maxLife)
{
	//Variables
	_maxLife = maxLife;
	_life = maxLife;
	_stamina = 100.f;
	_staminaEmptyFrames = 0.f;

	//Graphics
	_lifeStaminaTexture = lifeStaminaTexture;

	//Sprites
	for (int i = 0; i < _maxLife; i++)
	{
		_lifeSprites.push_back(new Sprite());
		_lifeSprites[i]->setTexture(*_lifeStaminaTexture);
		_lifeSprites[i]->setTextureRect(IntRect(0, 0, 14, 14));
		_lifeSprites[i]->setOrigin(7, 7);
	}
	_staminaBar = new Sprite();
	_staminaBar->setTexture(*_lifeStaminaTexture);
	_staminaBar->setTextureRect(IntRect(0, 14, 46, 10));
	_staminaBar->setOrigin(24, 5);

	//Stamina shapes
	_staminaShapeFront = new RectangleShape(Vector2f(42, 6));
	_staminaShapeFront2 = new RectangleShape(Vector2f(42, 6));
	_staminaShapeBack = new RectangleShape(Vector2f(42, 6));
	_staminaShapeFront->setFillColor(Color(181, 230, 29));
	_staminaShapeFront2->setFillColor(Color(234, 26, 83));
	_staminaShapeFront2->setSize(Vector2f(42, 6));
	_staminaShapeBack->setFillColor(Color(42, 42, 42));
}

bool playerHUD::SetTransparent(hero * player)
{
	return player->IsInvincible() || player->IsAiming();
}

void playerHUD::Step(hero * player, float deltaTime)
{
	//Update variables values
	_life = player->GetLife();
	_stamina = player->GetStamina();
	if (_stamina == 0.f)
		_staminaEmptyFrames += deltaTime;
	else
		_staminaEmptyFrames = 0.f;

	//Sprites positioning
	float lifeY = player->GetPositionA(false).y - (10.f * player->GetScale().y) * 2.f - (player->IsCrouching() ? 17.5f : 25.f);
	lifeY -= player->ClimbingLadder() ? 10.f : 0.f;
	for (int i = 0; i < _lifeSprites.size(); i++)
	{
		//Life color
		_lifeSprites[i]->setColor(Color(255, 255, 255, SetTransparent(player) ? 120 : 255));

		//Location
		float lifeX = (player->GetPositionA(false).x + i * 14.f) - ((_lifeSprites.size() * 14.f) / 2.f) + 6.f;
		_lifeSprites[i]->setPosition(lifeX + (player->IsOnTheFloor() * (player->GetSpeed().x / 2.f)), lifeY);
		_lifeSprites[i]->setTextureRect(IntRect(i < _life ? 0 : 14, 0, 14, 14));
	}

	//Stamina positioning + shape resizing
	float staminaX = player->GetPositionA(false).x + (player->IsOnTheFloor() * (player->GetSpeed().x / 2.f));
	_staminaBar->setPosition(staminaX, lifeY + 8.f);
	_staminaBar->setColor(Color(255, 255, 255, SetTransparent(player) ? 120 : 255));
	_staminaShapeBack->setPosition(staminaX - 22, lifeY + 5.f);
	_staminaShapeBack->setFillColor(Color(42, 42, 42, SetTransparent(player) ? 120 : 255));
	Color frontColor = _staminaShapeFront->getFillColor();
	_staminaShapeFront->setFillColor(Color(frontColor.r, frontColor.g, frontColor.b, SetTransparent(player) ? 120 : 255));
	_staminaShapeFront->setPosition(staminaX - 22, lifeY + 5.f);
	_staminaShapeFront->setSize(Vector2f(42 * _stamina / 100.f, 6));
	Color frontColor2 = _staminaShapeFront2->getFillColor();
	_staminaShapeFront2->setFillColor(Color(frontColor2.r, frontColor2.g, frontColor2.b, SetTransparent(player) ? 120 : 255));
	_staminaShapeFront2->setPosition(staminaX - 22, lifeY + 5.f);
	if(_staminaShapeFront2->getSize().x > _staminaShapeFront->getSize().x)
		_staminaShapeFront2->setSize(Vector2f(_staminaShapeFront2->getSize().x - deltaTime, 6));
	else
		_staminaShapeFront2->setSize(Vector2f(_staminaShapeFront->getSize().x, 6));

	//Stamina empty flash red animation
	if (_stamina == 0.f)
	{
		int fiveMultiple = _staminaEmptyFrames;
		while (fiveMultiple > 10)
			fiveMultiple -= 10;
		if(fiveMultiple >= 8)
			_staminaShapeBack->setFillColor(Color(42, 42, 42, SetTransparent(player) ? 120 : 255));
		else 
			_staminaShapeBack->setFillColor(Color(101 + fiveMultiple*22, 0, 0, SetTransparent(player) ? 120 : 255));
	}
}

void playerHUD::Display(RenderWindow * window)
{
	//Stamina
	if(_staminaShapeBack != NULL && _stamina > 0.f)
		utils::DrawLoopShape(window, _staminaShapeBack);
	if (_staminaShapeFront2 != NULL && _stamina > 0.f)
		utils::DrawLoopShape(window, _staminaShapeFront2);
	if (_staminaShapeFront != NULL && _stamina > 0.f)
		utils::DrawLoopShape(window, _staminaShapeFront);
	if (_staminaBar != NULL)
		utils::DrawLoopSprite(window, _staminaBar);
	if (_staminaShapeFront != NULL && _stamina == 0.f)
		utils::DrawLoopShape(window, _staminaShapeBack);

	//Life
	for (int i = 0; i < _lifeSprites.size(); i++)
		if(_lifeSprites[i] != NULL)
			utils::DrawLoopSprite(window, _lifeSprites[i]);
}