
#include "Graph.h"
#include<math.h>
#include "Vertex.h"
#include<thread>
Graph::Graph()
{
	inClosed = nullptr;
	inOpen = nullptr;
	heuristics = nullptr;
	found = nullptr;
}


Graph::~Graph()
{
	//deleting the open,closed and heuristics pointers
	if (inClosed != nullptr)
	{
		for (size_t i = 0; i < width; i++)
		{
			delete[] inClosed[i];
		}

		delete[] inClosed;
		inClosed = nullptr;
	}

	if (inOpen != nullptr)
	{
		for (size_t i = 0; i < width; i++)
		{
			delete[] inOpen[i];
		}

		delete[] inOpen;
		inOpen = nullptr;
	}

	if (heuristics != nullptr)
	{
		for (size_t i = 0; i < width; i++)
		{
			delete[] heuristics[i];
		}

		delete[] heuristics;
		heuristics = nullptr;
	}

	if (found != nullptr)
	{
		delete found;
		found = nullptr;
	}
}

void Graph::SetMaze(int** maze)
{
	//setting up the graph
	adjMatrix = maze;

	//precomputing the heuristics to every point on the graph
	heuristics = new float*[width];
	for (int i = 0; i < width; i++)
	{
		heuristics[i] = new float[height];
	}

	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			if (adjMatrix[i][j] > 0)
			{
				//calculate heuristics for open spaces
				float sqHeuristic = pow((i - endVertex.xPos), 2) + pow((j - endVertex.yPos), 2);
				float heuristic = sqrt(sqHeuristic);
				heuristics[i][j] = heuristic;
			}
		}
	}

	//setting the open and closed lits
	SetOpen();
	SetClosed();
}

void Graph::SetStartVertex(const int& xPos, const int& yPos)
{
	startVertex.xPos = xPos;
	startVertex.yPos = yPos;
}

void Graph::SetEndVertex(const int& xPos, const int& yPos)
{
	endVertex.xPos = xPos;
	endVertex.yPos = yPos;
}

void Graph::FindShortestPath(std::vector<Vertex>& path)
{

	//setting start vertex
	currentVertex = startVertex;

	//calculating start vertex
	currentVertex.cost = 0;
	currentVertex.heuristic = heuristics[currentVertex.xPos][currentVertex.yPos];
	currentVertex.priority = currentVertex.cost + currentVertex.heuristic;

	//adding start vertex to open list
	openQueue.push(currentVertex);
	inOpen[currentVertex.xPos][currentVertex.yPos] = 0;

	//while open list is empty
	while (!openQueue.empty())
	{
		currentVertex = openQueue.top();
		openQueue.pop();
		inClosed[currentVertex.xPos][currentVertex.yPos] = true;
		neighbors.clear();
		//closed.emplace(currentVertex, true);
		//std::vector<Vertex> neighbors;

		//calling function to store neighbours
		FindNeighbors(neighbors);

		for (int i = 0; i < neighbors.size(); i++)
		{
			//calulating cost to current neighbors
			float cost = currentVertex.cost + adjMatrix[neighbors[i].xPos][neighbors[i].yPos];

			//check if neighbour in in closed
			if (inClosed[neighbors[i].xPos][neighbors[i].yPos] == true)
				//if( closed[neighbors[i]] == true)
			{
				continue;
			}

			//check if current neighbour is in open
			if (cost < inOpen[neighbors[i].xPos][neighbors[i].yPos] ||
				inOpen[neighbors[i].xPos][neighbors[i].yPos] == DBL_MAX)
			{
				//calculate its cost and heuristic
				neighbors[i].cost = cost;
				inOpen[neighbors[i].xPos][neighbors[i].yPos] = cost;
				neighbors[i].heuristic = heuristics[neighbors[i].xPos][neighbors[i].yPos];
				//calculating the priority of neighbour
				neighbors[i].priority = neighbors[i].heuristic*1.2 + neighbors[i].cost;
				//set the parent node to current
				neighbors[i].previousVertex = std::make_shared<Vertex>(currentVertex);
				openQueue.push(neighbors[i]);

				//if the neighbour is the goal
				if (neighbors[i] == endVertex)
				{

					//found = new Vertex(neighbors[i]);

					//algorithm for reconstructing path
					Vertex* foundVertex = &neighbors[i];

					do
					{
						//adding path nodes to a stack
						path.emplace_back(*foundVertex);
						foundVertex = foundVertex->previousVertex.get();

					} while (foundVertex != nullptr);

					return;
				}
			}

		}

	}

	//if no path found
	if (path.size() == 0)
	{
		path.emplace_back(Vertex(startVertex.xPos, startVertex.yPos));
	}
	//return path;

}

//function to find all valid neighbours
void Graph::FindNeighbors(std::vector<Vertex>& neighbors)
{
	//Calculating the cost of each neighbor of the vertex
	Vertex neighbor1(currentVertex.xPos, currentVertex.yPos + 1);
	Vertex neighbor2(currentVertex.xPos, currentVertex.yPos - 1);
	Vertex neighbor3(currentVertex.xPos + 1, currentVertex.yPos);
	Vertex neighbor4(currentVertex.xPos - 1, currentVertex.yPos);
	Vertex neighbor5(currentVertex.xPos + 1, currentVertex.yPos - 1);
	Vertex neighbor6(currentVertex.xPos - 1, currentVertex.yPos + 1);
	Vertex neighbor7(currentVertex.xPos - 1, currentVertex.yPos - 1);
	Vertex neighbor8(currentVertex.xPos + 1, currentVertex.yPos + 1);

	//before the neighbours are added, we check if the x and y positions 
	//are within the bounds of the array, and then we check whether they
	//are a wall or not

	//Adding orthogonal neighbours

	//top neighbour
	if (neighbor1.xPos >= 0 && neighbor1.yPos >= 0
		&& neighbor1.xPos < width&&neighbor1.yPos < height
		&&adjMatrix[neighbor1.xPos][neighbor1.yPos] > 0)
	{
		neighbors.emplace_back(neighbor1);
	}

	//bottom neighbour
	if (neighbor2.xPos >= 0 && neighbor2.yPos >= 0
		&& neighbor2.xPos < width&&neighbor2.yPos < height
		&&adjMatrix[neighbor2.xPos][neighbor2.yPos] > 0)
	{
		neighbors.emplace_back(neighbor2);
	}

	//right neighbour
	if (neighbor3.xPos >= 0 && neighbor3.yPos >= 0
		&& neighbor3.xPos < width&&neighbor3.yPos < height
		&&adjMatrix[neighbor3.xPos][neighbor3.yPos] > 0)
	{
		neighbors.emplace_back(neighbor3);
	}

	//left neighbour
	if (neighbor4.xPos >= 0 && neighbor4.yPos >= 0
		&& neighbor4.xPos < width&&neighbor4.yPos < height
		&&adjMatrix[neighbor4.xPos][neighbor4.yPos] > 0)
	{
		neighbors.emplace_back(neighbor4);
	}

}

void Graph::SetClosed()
{
	//setting the closed list, this list represents nodes which couldn't possibly be part of the path
	if (inClosed != nullptr)
	{
		for (size_t i = 0; i < width; i++)
		{
			delete[] inClosed[i];
		}

		delete[] inClosed;

		inClosed = nullptr;
	}

	//setting up the closed list

	//creating this closed list as a 2d array of width x height dimensions
	inClosed = new bool*[width];

	for (int i = 0; i < width; i++)
	{
		inClosed[i] = new bool[height];
	}

	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			if (adjMatrix[i][j] > 0)
			{
				//inClosed = false;
				inClosed[i][j] = false;
			}
		}
	}
}

void Graph::SetOpen()
{
	//setting the open list of points which could potentially be part of the path 
	if (inOpen != nullptr)
	{
		for (size_t i = 0; i < width; i++)
		{
			delete[] inOpen[i];
		}

		delete[] inOpen;

		inOpen = nullptr;
	}

	//initializing the open list
	inOpen = new float*[width];

	for (int i = 0; i < width; i++)
	{
		inOpen[i] = new float[height];
	}

	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			if (adjMatrix[i][j] > 0)
			{
				//inClosed = false;
				inOpen[i][j] = DBL_MAX;
			}
		}
	}
}

void Graph::ClearGraph()
{
	//clearing the open, closed and heuristic list
	if (inClosed != nullptr)
	{
		for (size_t i = 0; i < width; i++)
		{
			delete[] inClosed[i];
		}

		delete[] inClosed;
		inClosed = nullptr;
	}

	if (inOpen != nullptr)
	{
		for (size_t i = 0; i < width; i++)
		{
			delete[] inOpen[i];
		}

		delete[] inOpen;
		inOpen = nullptr;
	}

	if (heuristics != nullptr)
	{
		for (size_t i = 0; i < width; i++)
		{
			delete[] heuristics[i];
		}

		delete[] heuristics;
		heuristics = nullptr;
	}

	if (found != nullptr)
	{
		delete found;
		found = nullptr;
	}
}

