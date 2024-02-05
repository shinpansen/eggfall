#include "AI.hpp"

static const float SCALE = 30.f;
static const float NEXTTOPOINTX = 15.f;
static const float NEXTTOPOINTY = 20.f;

AI::~AI()
{
}

AI::AI(int numPlayer, input * input, float aiLevel)
{
	//Variables
	_numPlayer = numPlayer;
	_hooked = 0;
	_aiLevel = aiLevel;
	_reachPointTimeout = 0.f;
	_dashAbovePitTimeout = 0.f;
	_stuck = 0.f;
	_newPathTries = 0.f;
	_distToPoint = 99999.f;
	_tryToReach = true;
	_cpuPlayerPos = Vector2f();
	_cpuPlayerSpeed = Vector2f();
	_pointToReach = Vector2f();

	//Objects
	_input = input;
	_input->SetAI(true);
}

void AI::LoadObstaclesList(b2World * world, vector<FloatRect> obstacles, vector<Vector2f> listPointsToReach)
{
	//Update obstacles
	_world = world;
	_obstacles = obstacles;

	//Update points allowed to reach by AI
	_mapGraph.Clear();
	for (int i = 0; i < listPointsToReach.size(); i++)
	{
		if ((!PointInObstacle(Vector2f(listPointsToReach[i].x, listPointsToReach[i].y + 32)) ||
			 !PointInObstacle(Vector2f(listPointsToReach[i].x, listPointsToReach[i].y - 32))) &&
			(!PointInObstacle(Vector2f(listPointsToReach[i].x - 32, listPointsToReach[i].y)) ||
			 !PointInObstacle(Vector2f(listPointsToReach[i].x + 32, listPointsToReach[i].y))))
			_mapGraph.AddNode(listPointsToReach[i]);
	}

	//Graph edges
	for (int pt1 = 0; pt1 < _mapGraph.GetNodesCount(); pt1++)
	{
		for (int pt2 = 0; pt2 < _mapGraph.GetNodesCount(); pt2++)
		{
			if (pt2 < pt1)
				continue;
			Vector2f point1 = _mapGraph.GetNode(pt1);
			Vector2f point2 = _mapGraph.GetNode(pt2);
			if (CanAddNodeBetween(point1, point2) ||
			   ((fabs(point1.x - point2.x) < 50.f && point1.y > point2.y &&
				 Box2DTools::TestLadderPoint(_world, b2Vec2(point1.x / SCALE, point1.y / SCALE))) ||
				(fabs(point1.x - point2.x) < 50.f && point1.y < point2.y &&
				 Box2DTools::TestLadderPoint(_world, b2Vec2(point2.x / SCALE, point2.y / SCALE)))))
				_mapGraph.AddEdge(point1, point2);
		}
	}
}

int AI::GetNumPlayer()
{
	return _numPlayer;
}

bool AI::PointInObstacle(Vector2f point)
{
	for (int i = 0; i < _obstacles.size(); i++)
		if (_obstacles[i].contains(point))
			return true;
	return false;
}

bool AI::ShootingPathInvalid(RectangleShape rect, bool aboveTarget)
{
	for (int i = 0; i < _obstacles.size(); i++)
	{
		if (_obstacles[i].intersects(rect.getGlobalBounds()))
		{
			if((_obstacles[i].height < 32.f && aboveTarget) || _obstacles[i].height >= 32.f)
				return true;
		}
	}
	return false;
}

bool AI::CanAddNodeBetween(Vector2f pt1, Vector2f pt2)
{
	//Default distance between points
	float cost = utils::DistanceBetween(pt1, pt2);

	//Add cost if intersect obstacle
	bool ptToTest[5] = { false, false, false, false, false };
	for (int i = 0; i < _obstacles.size(); i++)
	{
		if (_obstacles[i].height < 32.f)
			continue;
		ptToTest[0] = !ptToTest[0] ? utils::SegmentIntersectsRectangle(_obstacles[i], pt1, pt2) : ptToTest[0];
		ptToTest[1] = !ptToTest[1] ? utils::SegmentIntersectsRectangle(_obstacles[i], Vector2f(pt1.x - 15, pt1.y), pt2) : ptToTest[1];
		ptToTest[2] = !ptToTest[2] ? utils::SegmentIntersectsRectangle(_obstacles[i], Vector2f(pt1.x + 15, pt1.y), pt2) : ptToTest[2];
		ptToTest[3] = !ptToTest[3] ? utils::SegmentIntersectsRectangle(_obstacles[i], pt1, Vector2f(pt2.x - 15, pt2.y)) : ptToTest[3];
		ptToTest[4] = !ptToTest[4] ? utils::SegmentIntersectsRectangle(_obstacles[i], pt1, Vector2f(pt2.x + 15, pt2.y)) : ptToTest[4];
		if (ptToTest[0] && ptToTest[1] && ptToTest[2] && ptToTest[3] && ptToTest[4])
		{
			cost = 99999.f;
			break;
		}
	}
	return cost < 200.f;
}

void AI::DijkstraAlgorithmPath(Vector2f start, Vector2f end)
{
	//Compute dijkstra algorithm
	_shortestPath.clear();
	int startPointID = _mapGraph.FindClosestNode(start);
	int endPointID = _mapGraph.FindClosestNode(end);
	_shortestPath = _mapGraph.FindDijkstraShortestPath(startPointID, endPointID);
}

RectangleShape AI::GetShapeVector(Vector2f pt1, Vector2f pt2, int thickness)
{
	RectangleShape targetShape;
	float distBetween = utils::DistanceBetween(pt1, pt2);
	targetShape.setSize(Vector2f(distBetween, thickness));
	targetShape.setOrigin(0, ((float)thickness) / 2.f);
	if (pt1.x > pt2.x)
	{
		targetShape.setRotation(utils::GetVectorRotation(pt2, pt1));
		targetShape.setPosition(pt2);
	}
	else
	{
		targetShape.setRotation(utils::GetVectorRotation(pt1, pt2));
		targetShape.setPosition(_cpuPlayerPos);
	}
	return targetShape;
}

bool AI::PointsAreNeighbors(Vector2f pt1, Vector2f pt2)
{
	return fabs(pt1.x - pt2.x) < NEXTTOPOINTX && fabs(pt1.y - pt2.y) < NEXTTOPOINTY;
}

void AI::SetPointToReach(Vector2f point)
{
	if (point != _pointToReach)
		_pointToReach = point;
}

bool AI::PointToReachValid(Vector2f point)
{
	if (_obstacles.size() > 0)
		return !PointInObstacle(point) && PointInObstacle(Vector2f(point.x, point.y + 16));
	return true;
}

void AI::GenerateRandomReachPoint()
{
	//Point to reach
	int iteration = 0;
	_pointToReach = _mapGraph.GetNode(utils::RandomNumber(_mapGraph.GetNodesCount()-1, 0));
	while (iteration < 42 && utils::DistanceBetween(_pointToReach, _cpuPlayerPos) < 420.f)
	{
		_pointToReach = _mapGraph.GetNode(utils::RandomNumber(_mapGraph.GetNodesCount() - 1, 0));
		iteration++;
	}
	if (_cpuPlayerPos != Vector2f() && _pointToReach != Vector2f())
		DijkstraAlgorithmPath(_cpuPlayerPos, _pointToReach);

	if (_shortestPath.size() > 0)
		_pointToReach = _shortestPath[0];
	_pathID = 0;

	//Reset timer
	_reachPointTimeout = utils::RandomNumber(1600, 800);
	_stuck = 0.f;
	_newPathTries = 0.f;
	_distToPoint = utils::DistanceBetween(_cpuPlayerPos, _pointToReach);
	_tryToReach = true;
}

bool AI::NeedToJump()
{
	Vector2f frontPosA = Vector2f(_cpuPlayerPos.x + (_input->isKeyPressed(CMD_LEFT) ? -30.f : 30.f), _cpuPlayerPos.y - (1.f - _playerToControl->GetScale().y)*10.f);
	Vector2f frontPosB = Vector2f(_cpuPlayerPos.x + (_input->isKeyPressed(CMD_LEFT) ? -15.f : 15.f), _cpuPlayerPos.y - (1.f - _playerToControl->GetScale().y)*10.f);
	Vector2f frontPosVoid = Vector2f(_cpuPlayerPos.x + _cpuPlayerSpeed.x, _cpuPlayerPos.y + 25.f);
	Vector2f abovePos = Vector2f(_cpuPlayerPos.x - _cpuPlayerSpeed.x, _cpuPlayerPos.y + 66.f);
	bool voidInFront = true;
	for (int i = 0; i < _obstacles.size(); i++)
	{
		if (_playerToControl->IsOnTheFloor() && (_obstacles[i].contains(frontPosA) ||
			_obstacles[i].contains(frontPosB))) //Avoid abstacle
			return true;
		else if (!_playerToControl->IsOnTheFloor() && (_obstacles[i].contains(frontPosA) ||
			_obstacles[i].contains(frontPosB)) && _cpuPlayerPos.y > _pointToReach.y) //Walljump
			return true;
		else if (_obstacles[i].contains(frontPosVoid)) //There is no pit in front
			voidInFront = false;
	}
	if (voidInFront && _cpuPlayerPos.y > _pointToReach.y + 50.f) //Jump above the pit
		return true;
	else if (_cpuPlayerPos.y > _pointToReach.y && fabs(_cpuPlayerPos.y - _pointToReach.y) > 42.f &&
		fabs(_cpuPlayerPos.x - _pointToReach.x) < 66.f) //Reach the point by jumping
		return true;

	//Nothing to jump above
	return false;
}

void AI::Step(hero * playerToControl, projectiles * projectileBox, float deltaTime)
{
	//Update cpu player location
	_playerToControl = playerToControl;
	_cpuPlayerPos = playerToControl->GetPositionA(true);
	_cpuPlayerSpeed = playerToControl->GetSpeed();
	_hooked = playerToControl->GetHooked();

	//MOVE
	//Moving to point to reach
	if (_pointToReach != Vector2f())
	{
		bool dodgePlayer = false;
		for (int i = 0; i < playerToControl->_players.size(); i++)
			if (playerToControl->_players[i] != playerToControl &&
				fabs(playerToControl->_players[i]->GetPositionA().x - playerToControl->GetPositionA().x) < 30.f &&
				fabs(playerToControl->_players[i]->GetPositionA().y - playerToControl->GetPositionA().y) < 30.f &&
				playerToControl->_players[i]->GetSigneSpeed() != playerToControl->GetSigneSpeed())
				dodgePlayer = true;
		if (dodgePlayer)
		{
			_input->PressKeyAI(CMD_RIGHT, _cpuPlayerSpeed.x > 0.f ? 2.f : 0.f);
			_input->PressKeyAI(CMD_LEFT, _cpuPlayerSpeed.x < 0.f ? 2.f : 0.f);
			_input->PressKeyAI(CMD_DASH, 2.f);
		}
		else if (_hooked != 0)
		{
			playerToControl->SetHasJumped(false);
			_input->PressKeyAI(CMD_JUMP, 0.f);
			_input->PressKeyAI(CMD_DOWN, 0.f);
			if (_cpuPlayerPos.y > _pointToReach.y + 20.f)
			{
				_input->PressKeyAI(CMD_JUMP, utils::RandomNumber(20, 10));
				_input->PressKeyAI((_cpuPlayerPos.x > _pointToReach.x ? CMD_LEFT : CMD_RIGHT), 2.f);
			}
			else
				_input->PressKeyAI(CMD_DOWN, 5.f);
		}
		else if (playerToControl->ClimbingLadder() && fabs(_cpuPlayerSpeed.y) < 0.1f && !PointsAreNeighbors(_cpuPlayerPos, _pointToReach))
		{
			playerToControl->SetHasJumped(false);
			_input->PressKeyAI(CMD_JUMP, 10.f); //Leave ladder
		}
		else if (playerToControl->OnLadder() && _cpuPlayerPos.y > _pointToReach.y && fabs(_cpuPlayerPos.y - _pointToReach.y) >= 20.f)
			_input->PressKeyAI(CMD_UP, 2.f); //Climb up
		else if (playerToControl->OnLadder() && _cpuPlayerSpeed.y > 2.f && _cpuPlayerPos.y < _pointToReach.y && 
			     fabs(_cpuPlayerPos.y - _pointToReach.y) >= 20.f)
			_input->PressKeyAI(CMD_DOWN, 2.f); //Climb down
		else
		{
			if (fabs(_cpuPlayerPos.x - _pointToReach.x) > NEXTTOPOINTX) //Move toward point horizontally
			{
				if (NeedToJump())
				{
					playerToControl->SetHasJumped(false);
					int jumpHeight = fabs(_cpuPlayerPos.y - _pointToReach.y) <= 240.f ? fabs(_cpuPlayerPos.y - _pointToReach.y) / 16.f :
						utils::RandomNumber(15, 5);
					_input->PressKeyAI(CMD_JUMP, jumpHeight + 5);
				}
				int pressDuration = playerToControl->ClimbingLadder() ? 6 : 2;
				pressDuration == pressDuration == 2 && fabs(_cpuPlayerPos.x - _pointToReach.x) < 32.f ? 1 : 2;
				_input->PressKeyAI((_cpuPlayerPos.x > _pointToReach.x ? CMD_LEFT : CMD_RIGHT), pressDuration);
			}
			if (_cpuPlayerPos.y < _pointToReach.y && fabs(_cpuPlayerPos.y - _pointToReach.y) > 66.f &&
				playerToControl->OnPlatform()) //Pass through platforms
			{
				if (utils::RandomNumber(3, 0) == 0)
				{
					if (!_input->isKeyPressed(CMD_DOWN))
						playerToControl->SetHasJumped(false);
					_input->PressKeyAI(CMD_LEFT, 0.f);
					_input->PressKeyAI(CMD_RIGHT, 0.f);
					_input->PressKeyAI(CMD_DOWN, 10.f);
					_input->PressKeyAI(CMD_JUMP, 10.f);
				}
			}
			//Dash above pit
			if (_dashAbovePitTimeout > 10.f && !playerToControl->IsOnTheFloor() && _cpuPlayerSpeed.y >= 8.f &&
				fabs(_cpuPlayerPos.x - _pointToReach.x) > NEXTTOPOINTX*2.f && _cpuPlayerPos.y + 16.f > _pointToReach.y && 
				!PointInObstacle(Vector2f(_cpuPlayerPos.x, _cpuPlayerPos.y + 66.f)))
			{
				_input->PressKeyAI(CMD_DOWN, 0.f);
				_input->PressKeyAI(CMD_UP, 0.f);
				_input->PressKeyAI(_cpuPlayerPos.x > _pointToReach.x ? CMD_LEFT : CMD_RIGHT, 2.f);
				_input->PressKeyAI(CMD_DASH, 2.f);
			}
		}
		if (!PointsAreNeighbors(_cpuPlayerPos, _pointToReach) && fabs(_cpuPlayerSpeed.x) < 0.1f && fabs(_cpuPlayerSpeed.y) < 3.f &&
			!playerToControl->ClimbingLadder())
		{
			_stuck += deltaTime;
			if (_tryToReach && _stuck > 10.f) //Dash if stuck
			{
				_tryToReach = false;
				if (utils::RandomNumber(3, 0) == 0)
				{
					_input->PressKeyAI(CMD_UP, 1.f);
					_input->PressKeyAI(CMD_DASH, 1.f);
				}
				else
					_input->PressKeyAI(utils::RandomNumber(1, 0) == 1 ? CMD_LEFT : CMD_RIGHT, utils::RandomNumber(15, 10));
			}
		}
	}

	//ATTACK
	//Shoot other players
	if (playerToControl != NULL && utils::RandomNumber(42, 0) == 0 && 
		(playerToControl->GetStamina() > 50.f || utils::RandomNumber(10, 0) == 0))
	{
		_targetShape.setSize(Vector2f(0, 0));
		//_targetShape.setFillColor(Color(0, 0, 255, 150));
		for (int i = 0; i < playerToControl->_players.size(); i++)
		{
			if (playerToControl->_players[i] != playerToControl && !playerToControl->_players[i]->IsStunned())
			{
				//Calculating path to player
				_targetShape = GetShapeVector(_cpuPlayerPos, playerToControl->_players[i]->GetPositionA(), 10);

				//Shoot if path free
				if (!ShootingPathInvalid(_targetShape, _cpuPlayerPos.y + 40.f < playerToControl->_players[i]->GetPositionA().y))
				{
					_input->PressAnalogDirAI(utils::GetVectorDirection(_cpuPlayerPos, playerToControl->_players[i]->GetPositionA()), 15.f);
					_input->PressKeyAI(CMD_ATTACK, 5.f);
					break;
				}
			}
		}
	}

	//COUNTERACT - DODGE
	if (projectileBox != NULL)
	{
		vector<Vector2f> projectilesPos = projectileBox->GetProjectilesPos(true);
		for (int i = 0; i < projectilesPos.size(); i++)
		{
			if (utils::DistanceBetween(projectilesPos[i], _cpuPlayerPos) <= 66.f)
			{
				int rand = utils::RandomNumber(2, 0);
				_input->PressKeyAI(CMD_LEFT, projectilesPos[i].x < _cpuPlayerPos.x ? 2.f : 0.f);
				_input->PressKeyAI(CMD_RIGHT, projectilesPos[i].x > _cpuPlayerPos.x ? 2.f : 0.f);
				if (rand == 0)
					_input->PressKeyAI(CMD_DASH, 2.f);
				else if (rand == 1)
					_input->PressKeyAI(CMD_COUNTERACT, 2.f);
				break;
			}
		}
	}

	//Reach timeout
	_reachPointTimeout = utils::StepCooldown(_reachPointTimeout, 1.f, deltaTime);
	_dashAbovePitTimeout += deltaTime;
	if (_reachPointTimeout == 0.f || _stuck > 100.f) //Can't reach point - give up
		GenerateRandomReachPoint();
	else if (PointsAreNeighbors(_cpuPlayerPos, _pointToReach) && (playerToControl->IsOnTheFloor() ||
		playerToControl->ClimbingLadder())) //Next point in path
	{
		if (_pathID < _shortestPath.size() - 1)
		{
			_pathID++;
			_pointToReach = _shortestPath[_pathID];
			_reachPointTimeout = utils::RandomNumber(1600, 800);
			_stuck = 0.f;
			_dashAbovePitTimeout = 0.f;
			_tryToReach = true;
		}
		else
			GenerateRandomReachPoint();
	}
	else if (_stuck > 20.f && !_tryToReach) //Maybe got lost
	{
		if (_newPathTries < 2.f)
		{
			DijkstraAlgorithmPath(_cpuPlayerPos, _shortestPath[_shortestPath.size() - 1]);
			_pathID = 0;
			_pointToReach = _shortestPath[0];
			_reachPointTimeout = utils::RandomNumber(1600, 800);
			_stuck = 0.f;
			_newPathTries++;
		}
		else
			GenerateRandomReachPoint();
	}
}

void AI::Display(RenderWindow * window)
{
	if (_pointToReach != Vector2f())
	{
		CircleShape cReach = CircleShape(10);
		cReach.setOrigin(10, 10);
		cReach.setFillColor(Color(255, 255, 255, 200));
		cReach.setPosition(_pointToReach);
		window->draw(cReach);
	}
	//window->draw(_targetShape);
	/*for (int i = 0; i < _obstacles.size(); i++)
	{
		RectangleShape rect = RectangleShape(Vector2f(_obstacles[i].width, _obstacles[i].height));
		rect.setPosition(_obstacles[i].left, _obstacles[i].top);
		rect.setFillColor(Color(0, 255, 0, 150));
		window->draw(rect);
	}
	for (int i = 0; i < _listPointsToReach.size(); i++)
	{
		CircleShape cirea = CircleShape(16.f);
		cirea.setOrigin(16, 16);
		cirea.setPosition(_listPointsToReach[i]);
		cirea.setFillColor(Color(0, 0, 255, 50));
		window->draw(cirea);
	}*/
	if (_shortestPath.size() > 1)
	{
		for (int i = 0; i < _shortestPath.size() - 1; i++)
		{
			Vertex line[] =
			{
				Vertex(_shortestPath[i]),
				Vertex(_shortestPath[i + 1])
			};
			window->draw(line, 2, sf::Lines);
		}
	}
}