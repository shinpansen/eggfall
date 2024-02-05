#ifndef conicalLampA_HPP
#define conicalLampA_HPP

#include "teeterLamp.hpp"

using namespace std;
using namespace sf;

class conicalLampA : public teeterLamp
{
public:
	conicalLampA(
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
			120,
			colorID,
			teeterNode(IntRect(0, 105, 108, 96), Vector2f(54, 92), Vector2f(0, 0), false, false, 3.5f, 0.6f, 1.35f), 
			Vector2f(-2, 0),
			isLit)
	{
		//Nodes
		vector<teeterNode> nodes;
		nodes.push_back(teeterNode(IntRect(44, 80, 20, 24), Vector2f(10, 24), Vector2f(0, -92), false, true, 0.f, 0.f, 0.f));
		LoadNodes(nodes);

		//Lights
		LoadLights();
	}
};
#endif
