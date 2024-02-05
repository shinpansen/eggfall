#ifndef graph_HPP
#define graph_HPP

#include <iostream>
#include <math.h>
#include <SFML/Graphics.hpp>
#include "utils.hpp"

using namespace std;
using namespace sf;

class graphEdge
{
public:
	graphEdge(int node1ID, int node2ID, float cost)
	{
		this->node1ID = node1ID;
		this->node2ID = node2ID;
		this->cost = cost;
	};
	int node1ID;
	int node2ID;
	int cost;
};

class dijkstraRow
{
public:
	dijkstraRow(int nodeID, int cost = -1, int antecedent = -1, bool visited = false)
	{
		this->nodeID = nodeID;
		this->cost = cost;
		this->antecedent = antecedent;
		this->visited = visited;
	};
	int nodeID;
	int cost;
	int antecedent;
	bool visited;
};

class graph
{
public:
	graph();
	graph(vector<Vector2f> nodes, vector<graphEdge> edges);
	void Clear();
	bool AddNode(Vector2f node);
	bool AddEdge(Vector2f node1, Vector2f node2);
	bool RemoveNode(int i);
	bool RemoveNode(Vector2f node);
	bool RemoveEdge(int i);
	bool RemoveEdge(Vector2f node1, Vector2f node2);
	int FindClosestNode(Vector2f pos);
	int FindNextDijkstraNode(vector<dijkstraRow> dijkstraTable, int endID, int * currentID);
	int GetNodesCount();
	Vector2f GetNode(int i);
	vector<Vector2f> FindDijkstraShortestPath(int startID, int endID);
private:
	vector<Vector2f> _nodes;
	vector<graphEdge> _edges;
};
#endif
