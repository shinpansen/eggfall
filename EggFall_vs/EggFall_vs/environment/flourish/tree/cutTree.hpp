#ifndef cutTree_HPP
#define cutTree_HPP

#include "teeterTree.hpp"

using namespace std;
using namespace sf;

class cutTree : public teeterTree
{
public:
	cutTree(
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
			teeterNode(IntRect(232, 138, 116, 62), Vector2f(58, 56), Vector2f(0, 0), false, false, 4.f))
	{
		//Nodes
		vector<teeterNode> nodes;
		nodes.push_back(teeterNode(IntRect(232, 50, 116, 80), Vector2f(58, 70), Vector2f(0, -46), false, true, 3.5f));
		LoadNodes(nodes);

		//Leaves contact pos
		_leavesBounds.push_back(IntRect(pos.x - 5, pos.y - 120, 10, 106));
	}
};
#endif
