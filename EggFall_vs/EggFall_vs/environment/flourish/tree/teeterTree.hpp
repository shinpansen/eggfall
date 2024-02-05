#ifndef teeterTree_HPP
#define teeterTree_HPP

#include "../teeter.hpp"

using namespace std;
using namespace sf;

class teeterTree : public teeter
{
public:
	teeterTree();
	teeterTree(
		b2World * world,
		effects * effectBox,
		Texture * teeterTexture,
		Vector2f pos,
		int teeterHeight,
		Vector2f rectColorID,
		int leavesColorID,
		teeterNode root) :
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
		_leavesColorID = leavesColorID;
		_lastAngle = 0.f;
		_effectTimeout = 0.f;
	};
	using teeter::StepChild;
	void StepChild(float deltaTime);
	using teeter::DisplayChild;
	void DisplayChild(RenderWindow * window);
protected:
	int _leavesColorID;
	float _lastAngle;
	float _effectTimeout;
	vector<IntRect> _leavesBounds;
};
#endif
