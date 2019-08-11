
#pragma once
#include<vector>
#include<stack>
#include<queue>
#include"Vertex.h"
#include<memory>
#include<map>
class Graph
{
public:
	Graph();
	~Graph();

	int width;
	int height;

	//start and end vertices
	Vertex startVertex;
	Vertex endVertex;

	//current vertex to check
	Vertex currentVertex;

	//method to set the maze
	void SetMaze(int** maze);

	//function to set start and end maze
	void SetStartVertex(const int& xPos, const int& yPos);
	void SetEndVertex(const int& xPos, const int& yPos);

	//std::stack<Vertex> FindShortestPath();
	void FindShortestPath(std::vector<Vertex>& path);

	//set the open and the closed list
	void SetClosed();
	void SetOpen();

	//function to clear the graph
	void ClearGraph();

private:

	//Comparing the priority of the vertices
	struct CompareVertices
	{
		bool operator()(const Vertex& v1, const Vertex& v2) const
		{
			return v1.priority > v2.priority;
		}
	};

	//method to calculate g cost
	//inline float CalculateCost(Vertex& v1, Vertex& v2);

	void FindNeighbors(std::vector<Vertex>& neighbors);

	int** adjMatrix;

	//data structures for open and closed list
	std::priority_queue<Vertex, std::vector<Vertex>, CompareVertices> openQueue;

	//stack to store the path
	std::stack<Vertex> path;
	std::vector<Vertex> neighbors;

	//open and closed lists
	bool** inClosed;
	std::map<Vertex, bool> closed;
	float** inOpen;
	float** heuristics;

	Vertex* found;


};