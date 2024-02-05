#ifndef roundTree_HPP
#define roundTree_HPP

#include "teeterTree.hpp"

using namespace std;
using namespace sf;

class roundTree : public teeterTree
{
public:
	roundTree(
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
			146,
			colorID,
			leavesColorID,
			teeterNode(IntRect(348, 72, 116, 128), Vector2f(58, 122), Vector2f(0, 0), false, false, 5.f, 0.8f))
	{
		//Nodes
		vector<teeterNode> nodes;
		nodes.push_back(teeterNode(IntRect(348, 0, 116, 70), Vector2f(58, 44), Vector2f(0, -98), false, false, 5.f, 0.35f));
		LoadNodes(nodes);

		//Leaves contact pos
		_leavesBounds.push_back(IntRect(pos.x - 5, pos.y - 146, 10, 110));
	}
};
#endif
