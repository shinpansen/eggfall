#ifndef teeterLamp_HPP
#define teeterLamp_HPP

#include "../teeter.hpp"

using namespace std;
using namespace sf;

class teeterLamp : public teeter
{
public:
	~teeterLamp();
	teeterLamp();
	teeterLamp(
		b2World * world,
		effects * effectBox,
		Texture * teeterTexture,
		Texture * lampOffTexture,
		Vector2f pos,
		int teeterHeight,
		Vector2f rectColorID,
		teeterNode root,
		Vector2f lightsRelativePos = Vector2f(0, 0),
		bool isLit = true,
		float lightRadius = 24.f) :
		teeter(
			world,
			effectBox,
			teeterTexture,
			pos,
			teeterHeight,
			rectColorID,
			root)
	{
		//Variables
		_isLit = isLit;
		_lightRadius = lightRadius;
		_lightsRelativePos = lightsRelativePos;

		//Graphics
		_lampOffTexture = lampOffTexture;
	};
	void LoadLights();
	using teeter::StepChild;
	void StepChild(float deltaTime);
	using teeter::DisplayChild;
	void DisplayChild(RenderWindow * window);
protected:
	//Variables
	bool _isLit;
	float _lightRadius;
	Vector2f _lightsRelativePos;

	//Graphics
	Texture * _lampOffTexture;
	vector<CircleShape*> _lightsShapes;
	vector<pair<float, float>> _lightsShapesBorders;
};
#endif
