#ifndef AI_hpp
#define AI_hpp

#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <Box2D/Box2D.h>
#include "../tools/utils.hpp"
#include "../tools/input.hpp"
#include "../tools/Box2DTools.hpp"
#include "../tools/graph.hpp"
#include "../projectiles/projectiles.hpp"
#include "../characters/hero.hpp"

using namespace std;
using namespace sf;

class hero;
class projectiles;
class AI
{
public:
	~AI();
	AI(int numPlayer, input * input, float aiLevel);
	void LoadObstaclesList(b2World * world, vector<FloatRect> obstacles, vector<Vector2f> listPointsToReach);
	int GetNumPlayer();
	bool PointInObstacle(Vector2f point);
	bool ShootingPathInvalid(RectangleShape rect, bool aboveTarget);
	bool CanAddNodeBetween(Vector2f pt1, Vector2f pt2);
	void DijkstraAlgorithmPath(Vector2f start, Vector2f end);
	RectangleShape GetShapeVector(Vector2f pt1, Vector2f pt2, int thickness);
	bool PointsAreNeighbors(Vector2f pt1, Vector2f pt2);
	void SetPointToReach(Vector2f point);
	bool PointToReachValid(Vector2f point);
	void GenerateRandomReachPoint();
	bool NeedToJump();
	void Step(hero * playerToControl, projectiles * projectileBox, float deltaTime);
	void Display(RenderWindow * window);
private:
	//Variables
	int _numPlayer;
	int _hooked;
	int _pathID;
	float _aiLevel;
	float _reachPointTimeout;
	float _dashAbovePitTimeout;
	float _stuck;
	float _newPathTries;
	float _distToPoint;
	bool _tryToReach;
	Vector2f _cpuPlayerPos;
	Vector2f _cpuPlayerSpeed;
	Vector2f _pointToReach;

	//Objects
	b2World * _world;
	hero * _playerToControl;
	input * _input;
	vector<FloatRect> _obstacles;
	vector<Vector2f> _shortestPath;
	graph _mapGraph;

	//Graphics
	RectangleShape _targetShape;
};
#endif
