#include "MyOctant.h"

using namespace Simplex;


MyOctant::MyOctant()
{
}


Octant * Simplex::MyOctant::GetChild(uint a_nChild)
{
	return nullptr;
}

Octant * Simplex::MyOctant::GetParent()
{
	return nullptr;
}

bool Simplex::MyOctant::IsLeaf()
{
	return false;
}

bool Simplex::MyOctant::ContainsMoreThan(uint a_nEntities)
{
	return false;
}

void Simplex::MyOctant::KillBranches()
{
}

void Simplex::MyOctant::ConstructTree(uint a_nMaxLevel)
{
}

void Simplex::MyOctant::AssignIDtoEntity()
{
}

uint Simplex::MyOctant::GetOctantCount(void)
{
	return uint();
}

MyOctant::~MyOctant()
{
}

void Simplex::MyOctant::Release()
{
}

void Simplex::MyOctant::Init()
{
}

void Simplex::MyOctant::ConstructList()
{
}

Simplex::MyOctant::MyOctant(uint a_nMaxLevel, uint a_nIdealEntityCount)
{
}

Simplex::MyOctant::MyOctant(vector3 a_v3Center, float a_fSize)
{
}

Simplex::MyOctant::MyOctant(const MyOctant & other)
{
}

MyOctant & Simplex::MyOctant::operator=(const MyOctant & other)
{
	// TODO: insert return statement here
}

void Simplex::MyOctant::Swap(const MyOctant & other)
{
}

float Simplex::MyOctant::GetSize()
{
	return 0.0f;
}

vector3 Simplex::MyOctant::GetCenterGlobal()
{
	return vector3();
}

vector3 Simplex::MyOctant::GetMinGlobal()
{
	return vector3();
}

vector3 Simplex::MyOctant::GetMaxGlobal()
{
	return vector3();
}

bool Simplex::MyOctant::IsColliding(uint index)
{
	return false;
}

void Simplex::MyOctant::Display(uint a_uIndex, vector3 a_v3Color)
{
}

void Simplex::MyOctant::Display(vector3 a_v3Color)
{
}

void Simplex::MyOctant::DisplayLeaves(vector3 a_v3Color)
{
}

void Simplex::MyOctant::ClearEntityList()
{
}

void Simplex::MyOctant::Subdivide()
{
}
