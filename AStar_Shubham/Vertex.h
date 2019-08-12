#pragma once
#include<memory>
class Vertex
{
public:
	Vertex();
	Vertex(int xPos, int yPos);
	Vertex(const Vertex& vertex);

	//overloading the not equals operator
	bool operator!=(Vertex& vertex);
	//copy assignment
	Vertex& operator=(const Vertex& vertex);

	//quality and less than operator overload
	bool operator==(const Vertex& vertex) const;
	bool operator<(const Vertex& vertex) const;
	~Vertex();

	//x and y positions of the vertex
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

	//previous and next vertex in the path
	std::shared_ptr<Vertex> previousVertex;
	std::shared_ptr<Vertex> nextVertex;
};
