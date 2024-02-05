#ifndef firLong_HPP
#define firLong_HPP

#include "teeterTree.hpp"

using namespace std;
using namespace sf;

class firLong : public teeterTree
{
public:
	firLong(
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
			144,
			colorID,
			leavesColorID,
			teeterNode(IntRect(116, 110, 116, 90), Vector2f(58, 84), Vector2f(0, 0), false, false, 5.f))
	{
		//Nodes
		vector<teeterNode> nodes;
		nodes.push_back(teeterNode(IntRect(116, 50, 116, 58), Vector2f(58, 58), Vector2f(0, -80), false, false, 4.f));
		LoadNodes(nodes);

		//Leaves contact pos
		_leavesBounds.push_back(IntRect(pos.x - 5, pos.y - 82, 10, 74));
		_leavesBounds.push_back(IntRect(pos.x - 5, pos.y - 144, 10, 42));
	}
};
#endif
