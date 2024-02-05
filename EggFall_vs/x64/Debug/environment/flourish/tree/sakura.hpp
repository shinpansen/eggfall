#ifndef sakura_HPP
#define sakura_HPP

#include "teeterTree.hpp"

using namespace std;
using namespace sf;

class sakura : public teeterTree
{
public:
	sakura(
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
			115,
			colorID,
			leavesColorID,
			teeterNode(IntRect(846, 132, 48, 68), Vector2f(24, 58), Vector2f(0, 0), false, false, 6.f, 0.6f))
	{
		//Nodes
		vector<teeterNode> nodes;
		nodes.push_back(teeterNode(IntRect(812, 0, 116, 64), Vector2f(46, 50), Vector2f(-12, -66), false, true, 5.f, 0.75f));
		nodes.push_back(teeterNode(IntRect(854, 66, 74, 60), Vector2f(28, 44), Vector2f(12, -60), false, true, 5.f, 0.75f));
		nodes.push_back(teeterNode(IntRect(812, 154, 30, 20), Vector2f(28, 18), Vector2f(-8, -14), false, true, 5.f, 1.5f));
		nodes.push_back(teeterNode(IntRect(812, 104, 36, 20), Vector2f(2, 12), Vector2f(8, -30), false, true, 5.f, 1.f));
		LoadNodes(nodes);

		//Leaves contact pos
		_leavesBounds.push_back(IntRect(pos.x - 5, pos.y - 115, 10, 62));
	}
};
#endif
