#include "graph.hpp"

static const float SCALE = 30.f;

graph::graph()
{
}

graph::graph(vector<Vector2f> nodes, vector<graphEdge> edges)
{
	_nodes = nodes;
	_edges = edges;
}

void graph::Clear()
{
	_nodes.clear();
	_edges.clear();
}

bool graph::AddNode(Vector2f node)
{
	//Test if node already exists in graph
	for (int i = 0; i < _nodes.size(); i++)
		if (_nodes[i] == node)
			return false;

	//Add node
	_nodes.push_back(node);
	return true;
}

bool graph::AddEdge(Vector2f node1, Vector2f node2)
{
	//Test if nodes exist in graph
	int node1ID = -1;
	int node2ID = -1;
	for (int i = 0; i < _nodes.size(); i++)
	{
		if (_nodes[i] == node1)
			node1ID = i;
		if (_nodes[i] == node2)
			node2ID = i;
		if (node1ID != -1 && node2ID != -1)
			break;
	}
	if (node1ID == -1 || node2ID == -1 || node1ID == node2ID)
		return false;

	//Test if edge already exists in graph
	for (int i = 0; i < _edges.size(); i++)
		if ((_edges[i].node1ID == node1ID && _edges[i].node2ID == node2ID) ||
			(_edges[i].node2ID == node1ID && _edges[i].node1ID == node2ID))
			return false;

	//Add edge
	int cost = (int)utils::DistanceBetween(_nodes[node1ID], _nodes[node2ID]);
	_edges.push_back(graphEdge(node1ID, node2ID, cost));
	return true;
}

bool graph::RemoveNode(int i)
{
	if (i >= 0 && i < _nodes.size())
	{
		_nodes.erase(_nodes.begin() + i);
		return true;
	}
	return false;
}

bool graph::RemoveNode(Vector2f node)
{
	for (int i = 0; i < _nodes.size(); i++)
	{
		if (_nodes[i] == node)
		{
			_nodes.erase(_nodes.begin() + i);
			return true;
		}
	}
	return false;
}

bool graph::RemoveEdge(int i)
{
	if (i >= 0 && i < _edges.size())
	{
		_edges.erase(_edges.begin() + i);
		return true;
	}
	return false;
}

bool graph::RemoveEdge(Vector2f node1, Vector2f node2)
{
	for (int i = 0; i < _edges.size(); i++)
	{
		if ((_nodes[_edges[i].node1ID] == node1 && _nodes[_edges[i].node2ID] == node2) ||
			(_nodes[_edges[i].node1ID] == node2 && _nodes[_edges[i].node2ID] == node1))
		{
			_edges.erase(_edges.begin() + i);
			return true;
		}
	}
	return false;
}

int graph::FindClosestNode(Vector2f pos)
{
	//Searching closest point to pos in graph
	int dist = 999999, pointID;
	for (int i = 0; i < _nodes.size(); i++)
	{
		int currentDist = (int)utils::DistanceBetween(_nodes[i], pos);
		if (currentDist < dist)
		{
			pointID = i;
			dist = currentDist;
		}
	}
	return dist == 999999 ? -1 : pointID;
}

int graph::FindNextDijkstraNode(vector<dijkstraRow> dijkstraTable, int endID, int * currentID)
{
	int minCost = 999999;
	int nodeID = -1;
	for (int i = 0; i < dijkstraTable.size(); i++)
	{
		if (!dijkstraTable[i].visited && dijkstraTable[i].cost >= 0 && dijkstraTable[i].cost < minCost)
		{
			minCost = dijkstraTable[i].cost;
			nodeID = i;
			*currentID = i;
		}
	}

	//Return
	if (nodeID == -1 && dijkstraTable[endID].cost == -1)
	{
		*currentID = endID;
		return endID;
	}
	return nodeID;
}

int graph::GetNodesCount()
{
	return _nodes.size();
}

Vector2f graph::GetNode(int i)
{
	return i < 0 || i >= _nodes.size() ? Vector2f() : _nodes[i];
}

vector<Vector2f> graph::FindDijkstraShortestPath(int startID, int endID)
{
	//Returns empty list if points ID aren't valid
	vector<Vector2f> path;
	if (startID < 0 || endID < 0 || startID >= _nodes.size() || endID >= _nodes.size())
		return path;

	//Initialization
	Vector2f start = _nodes[startID];
	Vector2f end = _nodes[endID];
	int currentID;
	vector<dijkstraRow> dijkstraTable;
	for (int i = 0; i < _nodes.size(); i++)
		dijkstraTable.push_back(dijkstraRow(i, i == startID ? 0 : -1));

	//Start algorithm
	while (FindNextDijkstraNode(dijkstraTable, endID, &currentID) != -1)
	{
		dijkstraTable[currentID].visited = true;
		for (int i = 0; i < _edges.size(); i++)
		{
			if (_edges[i].node1ID == currentID || _edges[i].node2ID == currentID)
			{
				int neighborID = _edges[i].node1ID == currentID ? _edges[i].node2ID : _edges[i].node1ID;
				if (!dijkstraTable[neighborID].visited && (dijkstraTable[neighborID].cost == -1 ||
					dijkstraTable[neighborID].cost > dijkstraTable[currentID].cost + _edges[i].cost))
				{
					//Update neighbor cost + antecedent
					dijkstraTable[neighborID].cost = dijkstraTable[currentID].cost + _edges[i].cost;
					dijkstraTable[neighborID].antecedent = currentID;
				}
			}
		}
	}

	//Reconstitute the path
	path.push_back(_nodes[endID]);
	currentID = endID;
	while (currentID != startID)
	{
		path.insert(path.begin(), _nodes[dijkstraTable[currentID].antecedent]);
		currentID = dijkstraTable[currentID].antecedent;
	}
	return path;
}

