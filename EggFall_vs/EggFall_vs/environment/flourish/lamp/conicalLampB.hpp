#ifndef conicalLampB_HPP
#define conicalLampB_HPP

#include "teeterLamp.hpp"

using namespace std;
using namespace sf;

class conicalLampB : public teeterLamp
{
public:
	conicalLampB(
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
			156,
			colorID,
			teeterNode(IntRect(108, 69, 108, 132), Vector2f(54, 128), Vector2f(0, 0), false, false, 3.5f, 0.6f, 1.35f),
			Vector2f(-2, 0),
			isLit)
	{
		//Nodes
		vector<teeterNode> nodes;
		nodes.push_back(teeterNode(IntRect(152, 44, 20, 24), Vector2f(10, 24), Vector2f(0, -128), false, true, 0.f, 0.f, 0.f));
		nodes.push_back(teeterNode(IntRect(152, 44, 20, 26), Vector2f(10, 26), Vector2f(-15, -110), false, true, 0.f, 0.f, 0.f));
		nodes.push_back(teeterNode(IntRect(152, 44, 20, 26), Vector2f(10, 26), Vector2f(15, -110), false, true, 0.f, 0.f, 0.f));
		LoadNodes(nodes);

		//Lights
		LoadLights();
	}
};
#endif
