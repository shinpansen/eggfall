#ifndef oak_HPP
#define oak_HPP

#include "teeterTree.hpp"

using namespace std;
using namespace sf;

class oak : public teeterTree
{
public:
	oak(
		b2World * world,
		effects * effectBox,
		Texture * teeterTexture,
		Vector2f pos,
		Vector2f colorID,
		int leavesColorID) :
		teeterTree(
			world,
			effectBox,
			teeterTexture,
			pos,
			120,
			colorID,
			leavesColorID,
			teeterNode(IntRect(464, 124, 116, 76), Vector2f(58, 70), Vector2f(0, 0), false, false, 5.f, 0.75f))
	{
		//Nodes
		vector<teeterNode> nodes;
		nodes.push_back(teeterNode(IntRect(464, 0, 116, 90), Vector2f(58, 70), Vector2f(0, -46), false, false, 4.5f, 0.6f));
		LoadNodes(nodes);

		//Leaves contact pos
		_leavesBounds.push_back(IntRect(pos.x - 5, pos.y - 120, 10, 112));
	}
};
#endif
