#ifndef shrubSmall_HPP
#define shrubSmall_HPP

#include "teeterTree.hpp"

using namespace std;
using namespace sf;

class shrubSmall : public teeterTree
{
public:
	shrubSmall(
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
			54,
			colorID,
			leavesColorID,
			teeterNode(IntRect(636, 174, 6, 26), Vector2f(3, 20), Vector2f(0, 0), false, false, 4.f, 1.75f))
	{
		//Nodes
		vector<teeterNode> nodes;
		nodes.push_back(teeterNode(IntRect(579, 140, 115, 36), Vector2f(57, 36), Vector2f(-3, -18), false, false, 4.f, 1.5f));
		nodes.push_back(teeterNode(IntRect(622, 176, 12, 10), Vector2f(12, 4), Vector2f(-1, -14), false, false, 4.f, 2.5f));
		nodes.push_back(teeterNode(IntRect(644, 184, 14, 10), Vector2f(0, 4), Vector2f(1, -6), false, false, 4.f, 2.f));
		LoadNodes(nodes);

		//Leaves contact pos
		_leavesBounds.push_back(IntRect(pos.x - 5, pos.y - 54, 10, 36));
	}
};
#endif
