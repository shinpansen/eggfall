#include "teeterTree.hpp"

teeterTree::teeterTree()
{
}

void teeterTree::StepChild(float deltaTime)
{
	//If no valid color for leaves - stop
	if (utils::GetGreeneryColor(_leavesColorID) == Color())
		return;

	//Leaves fall effect
	if (_contactPos != Vector2i() && _leavesBounds.size() > 0 && _effectTimeout == 0.f)
	{
		for (int i = 0; i < _leavesBounds.size(); i++)
		{
			//Bounds for loop
			IntRect topLoopBound = _leavesBounds[i];
			topLoopBound.top -= utils::VIEW_HEIGHT;
			IntRect bottomLoopBound = _leavesBounds[i];
			bottomLoopBound.top += utils::VIEW_HEIGHT;

			//Testing if touching bounds
			if (_leavesBounds[i].contains(_contactPos) || (_leavesBounds[i].top < 0 && bottomLoopBound.contains(_contactPos)) ||
			    (_leavesBounds[i].top + _leavesBounds[i].height > utils::VIEW_HEIGHT && topLoopBound.contains(_contactPos)))
			{
				//Drawing leaves
				int random = utils::RandomNumber(8, 4);
				drawOption leafDrawOption = drawOption(Vector2f(0, 0), Vector2f(6, 2), utils::GetGreeneryColor(_leavesColorID));
				leafDrawOption.pos = Vector2f(_contactPos.x, _contactPos.y - 5);
				fadeOption foLeaves = fadeOption(Vector2f(0, 0), 12.f, 0.f, 150.f, 0.f, Vector2f(0, 0), 20.f);
				for (int i = 0; i < random; i++)
					_effectBox->AddRectangleShape(leafDrawOption, foLeaves, physics(_world, 0.3));

				//Limiting amount of particles along time
				_effectTimeout = 20.f;
			}
		}
	}

	//Last angle - effect timeout
	_lastAngle = _root.angle;
	_effectTimeout = utils::StepCooldown(_effectTimeout, 1.f, deltaTime);
}

void teeterTree::DisplayChild(RenderWindow * window)
{
	/*for (int i = 0; i < _leavesBounds.size(); i++)
	{
		RectangleShape rect = RectangleShape(Vector2f(_leavesBounds[i].width, _leavesBounds[i].height));
		rect.setPosition(_leavesBounds[i].left, _leavesBounds[i].top);
		rect.setFillColor(Color(255, 0, 0, 200));
		window->draw(rect);
	}*/
}