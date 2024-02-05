#ifndef firShort_HPP
#define firShort_HPP

#include "teeterTree.hpp"

using namespace std;
using namespace sf;

class firShort : public teeterTree
{
public:
	firShort(
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
			90,
			colorID,
			leavesColorID,
			teeterNode(IntRect(0, 132, 116, 68), Vector2f(58, 62), Vector2f(0, 0), false, false, 5.f))
	{
		//Nodes
		vector<teeterNode> nodes;
		nodes.push_back(teeterNode(IntRect(0, 50, 116, 82), Vector2f(58, 80), Vector2f(0, -52), false, true, 4.5f, 0.75f));
		LoadNodes(nodes);

		//Leaves contact pos
		_leavesBounds.push_back(IntRect(pos.x - 5, pos.y - 90, 10, 82));
	}
};
#endif
