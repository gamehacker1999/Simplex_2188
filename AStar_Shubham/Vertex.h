#pragma once
#include<memory>
class Vertex
{
public:
	Vertex();
	Vertex(int xPos, int yPos);
	Vertex(const Vertex& vertex);
	bool operator!=(Vertex& vertex);
	Vertex& operator=(const Vertex& vertex);
	bool operator==(const Vertex& vertex) const;
	bool operator<(const Vertex& vertex) const;
	~Vertex();
	int xPos;
	int yPos;
	//h(x)
	float heuristic;
	//g(x)
	float cost;
	//f(x)
	float priority;
	bool visited;
	bool open = false;;
	bool closed = false;
	std::shared_ptr<Vertex> previousVertex;
	std::shared_ptr<Vertex> nextVertex;

	//Vertex* previousVertex;
};
