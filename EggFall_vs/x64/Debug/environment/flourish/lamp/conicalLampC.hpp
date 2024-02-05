#ifndef conicalLampC_HPP
#define conicalLampC_HPP

#include "teeterLamp.hpp"

using namespace std;
using namespace sf;

class conicalLampC : public teeterLamp
{
public:
	conicalLampC(
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
			144,
			colorID,
			teeterNode(IntRect(356, 40, 44, 160), Vector2f(22, 156), Vector2f(0, 0), false, false, 3.5f, 0.6f, 1.35f), 
			Vector2f(-2, 0),
			isLit)
	{
		//Nodes
		vector<teeterNode> nodes;
		nodes.push_back(teeterNode(IntRect(326, 76, 20, 24), Vector2f(10, 0), Vector2f(-14, -120), false, true, 6.f, 2.f, 1.25f));
		nodes.push_back(teeterNode(IntRect(326, 76, 20, 24), Vector2f(10, 0), Vector2f(14, -120), false, true, 6.f, 2.f, 1.25f));
		LoadNodes(nodes);

		//Lights
		LoadLights();
	}
};
#endif
