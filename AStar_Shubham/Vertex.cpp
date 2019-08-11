#include<corecrt.h>
#include "Vertex.h"

//constructor
Vertex::Vertex()
{
	this->xPos = 0;
	this->yPos = 0;
	cost = _CRT_INT_MAX;
	heuristic = _CRT_INT_MAX;
	previousVertex = nullptr;
	visited = false;
	open = false;
	closed = false;
}

//parametrized constructor that takes the width and height
Vertex::Vertex(int xPos, int yPos)
{
	this->xPos = xPos;
	this->yPos = yPos;
	cost = _CRT_INT_MAX;
	heuristic = _CRT_INT_MAX;
	visited = false;
	previousVertex = nullptr;
	open = false;
	closed = false;

}

//copy constructor
Vertex::Vertex(const Vertex& vertex)
{
	this->xPos = vertex.xPos;
	this->yPos = vertex.yPos;
	this->heuristic = vertex.heuristic;
	this->cost = vertex.cost;
	this->priority = vertex.priority;
	this->previousVertex = vertex.previousVertex;
	this->visited = vertex.visited;
	this->open = vertex.open;
	this->closed = vertex.closed;
}

//not equals operator overload
bool Vertex::operator!=(Vertex & vertex)
{
	if (this->xPos == vertex.xPos&&this->yPos == vertex.yPos)
	{
		return false;
	}

	return true;
}

//equals operator overload
Vertex & Vertex::operator=(const Vertex & vertex)
{
	this->xPos = vertex.xPos;
	this->yPos = vertex.yPos;
	this->heuristic = vertex.heuristic;
	this->cost = vertex.cost;
	this->previousVertex = vertex.previousVertex;
	this->priority = vertex.priority;
	this->visited = vertex.visited;
	this->open = vertex.open;
	this->closed = vertex.closed;

	return *this;
}

//equality check operator overload
bool Vertex::operator==(const Vertex& vertex) const
{
	return (this->xPos == vertex.xPos) && (this->yPos == vertex.yPos);
}

//greater than operator overload 
bool Vertex::operator<(const Vertex & vertex) const
{
	//return this->priority < vertex.priority;
	return this->xPos < vertex.xPos < xPos&&this->yPos < vertex.yPos;
}

Vertex::~Vertex()
{

}