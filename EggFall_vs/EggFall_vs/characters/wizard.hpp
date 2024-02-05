#ifndef wizard_hpp
#define wizard_hpp

#include "hero.hpp"

using namespace std;
using namespace sf;

class wizard : public hero
{
public:
	wizard(int colorID, 
		int life, 
		Vector2f pos, 
		b2World* world, 
		input* input, 
		effects* effectBox,
		projectiles* projectileBox, 
		sound* soundBox, 
		scoreManager* scoreManager, 
		Texture * heroTexture,
		Texture * heroTextureFlash, 
		Texture * lifeStaminaTexture, 
		Texture * aimArrowTexture) :
		hero(
			colorID, 
			life, 
			pos, 
			world, 
			input, 
			effectBox, 
			projectileBox, 
			soundBox, 
			scoreManager,
			heroTexture,
			heroTextureFlash,
			lifeStaminaTexture,
			aimArrowTexture)
	{
	};

	//Hidden methods
	using hero::PerformAttack;
	bool PerformAttack();
	using hero::AnimAttack;
	void AnimAttack();
	using hero::AnimateChild;
	void AnimateChild();
	using hero::PerformCounteractChildEffect;
	void PerformCounteractChildEffect();
};
#endif
