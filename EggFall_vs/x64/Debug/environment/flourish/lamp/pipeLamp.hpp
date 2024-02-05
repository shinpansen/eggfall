#ifndef pipeLamp_HPP
#define pipeLamp_HPP

#include "teeterLamp.hpp"

using namespace std;
using namespace sf;

class pipeLamp : public teeterLamp
{
public:
	pipeLamp(
		b2World * world,
		effects * effectBox,
		Texture * teeterTexture,
		Texture * lampOffTexture,
		Vector2f pos,
		Vector2f colorID,
		bool isLit = true) :
		teeterLamp(
			world,
			effectBox,
			teeterTexture,
			lampOffTexture,
			pos,
			148,
			colorID,
			teeterNode(IntRect(216, 80, 108, 120), Vector2f(54, 116), Vector2f(0, 0), false, false, 4.f, 0.6f, 1.35f), 
			Vector2f(-4, -6),
			isLit,
			18.f)
	{
		//Nodes
		vector<teeterNode> nodes;
		nodes.push_back(teeterNode(IntRect(262, 52, 16, 28), Vector2f(8, 32), Vector2f(0, -116), false, true, 0.f, 0.f, 0.f));
		nodes.push_back(teeterNode(IntRect(234, 52, 10, 28), Vector2f(5, 28), Vector2f(-31, -96), false, true, 2.5f, 0.5f, 1.5f));
		nodes.push_back(teeterNode(IntRect(234, 52, 10, 28), Vector2f(5, 28), Vector2f(31, -96), false, true, 2.5f, 0.5f, 1.5f));
		nodes.push_back(teeterNode(IntRect(234, 52, 10, 28), Vector2f(5, 28), Vector2f(-17, -106), false, true, 2.5f, 0.5f, 1.5f));
		nodes.push_back(teeterNode(IntRect(234, 52, 10, 28), Vector2f(5, 28), Vector2f(17, -106), false, true, 2.5f, 0.5f, 1.5f));
		LoadNodes(nodes);

		//Lights
		LoadLights();
		if (_lightsShapes.size() > 0)
			_lightsShapes[0]->setOrigin(_lightsShapes[0]->getOrigin().x - 3.f, _lightsShapes[0]->getOrigin().y - 4.f);
	}
};
#endif
