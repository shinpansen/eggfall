#ifndef hatLamp_HPP
#define hatLamp_HPP

#include "teeterLamp.hpp"

using namespace std;
using namespace sf;

class hatLamp : public teeterLamp
{
public:
	hatLamp(
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
			154,
			colorID,
			teeterNode(IntRect(432, 46, 108, 154), Vector2f(54, 150), Vector2f(0, 0), false, false, 4.f, 0.6f, 1.35f), 
			Vector2f(-2, 10),
			isLit,
			28.f)
	{
		//Nodes
		vector<teeterNode> nodes;
		nodes.push_back(teeterNode(IntRect(436, 0, 22, 32), Vector2f(11, 0), Vector2f(-40, -120), false, true, 6.f, 1.35f, 1.2f));
		nodes.push_back(teeterNode(IntRect(436, 0, 22, 32), Vector2f(11, 0), Vector2f(40, -120), false, true, 6.f, 1.35f, 1.2f));
		LoadNodes(nodes);

		//Lights
		LoadLights();
	}
};
#endif
