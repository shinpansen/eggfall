#ifndef playerHUD_HPP
#define playerHUD_HPP

#include <iostream>
#include <vector>
#include <string>
#include <math.h>
#include <SFML/Graphics.hpp>
#include "../../tools/files.hpp"
#include "../../characters/hero.hpp"

using namespace std;
using namespace sf;

class hero;
class playerHUD
{
public:
	~playerHUD();
	playerHUD(Texture * lifeStaminaTexture, int maxLife);
	bool SetTransparent(hero * player);
	void Step(hero * player, float deltaTime);
    void Display(RenderWindow * window);
private:
    //Variables - Objects
	int  _maxLife;
	int  _life;
	float _stamina;
	float _staminaEmptyFrames;

    //Graphics
    Texture * _lifeStaminaTexture;
	vector<Sprite*> _lifeSprites;
	Sprite * _staminaBar;
	RectangleShape * _staminaShapeFront;
	RectangleShape * _staminaShapeFront2;
	RectangleShape * _staminaShapeBack;
};
#endif
