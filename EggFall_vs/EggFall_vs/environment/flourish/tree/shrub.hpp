#ifndef shrub_HPP
#define shrub_HPP

#include "teeterTree.hpp"

using namespace std;
using namespace sf;

class shrub : public teeterTree
{
public:
	shrub(
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
			86,
			colorID,
			leavesColorID,
			teeterNode(IntRect(748, 168, 12, 32), Vector2f(5, 26), Vector2f(0, 0), false, false, 4.f, 1.5f))
	{
		//Nodes
		vector<teeterNode> nodes;
		nodes.push_back(teeterNode(IntRect(722, 106, 64, 62), Vector2f(32, 62), Vector2f(0, -24), false, false, 4.f, 1.25f));
		nodes.push_back(teeterNode(IntRect(726, 174, 16, 16), Vector2f(16, 8), Vector2f(-1, -10), false, false, 4.f, 2.5f));
		nodes.push_back(teeterNode(IntRect(762, 170, 22, 18), Vector2f(0, 8), Vector2f(3, -16), false, false, 4.f, 2.f));
		LoadNodes(nodes);

		//Leaves contact pos
		_leavesBounds.push_back(IntRect(pos.x - 5, pos.y - 86, 10, 62));
	}
};
#endif
