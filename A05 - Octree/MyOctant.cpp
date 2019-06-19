#include "MyOctant.h"

using namespace Simplex;

//initializing the static values
uint MyOctant::m_uOctantCount = 0;
uint MyOctant::m_uMaxLevel = 1;
uint MyOctant::m_uIdealEntityCount = 1;

MyOctant* Simplex::MyOctant::GetChild(uint a_nChild)
{
	//return the specified child
	return m_pChild[a_nChild];
}

bool Simplex::MyOctant::IsLeaf()
{
	//return true if there are no children
	if (m_uChildren == 0)
	{
		return true;
	}

	return false;
}

bool Simplex::MyOctant::ContainsMoreThan(uint a_nEntities)
{
	//return true if it contains more then the ideal amount of entities
	if (m_EntityList.size() > a_nEntities)
	{
		return true;
	}

	return false;
}

void Simplex::MyOctant::KillBranches()
{
	//looping through the octants till we reach an octant with no children
	//if it has been found, backtrack to the parent and delete and that child
	for (uint i = 0; i < m_uChildren; i++)
	{
		m_pChild[i]->KillBranches();
		delete m_pChild[i];
		m_pChild[i] = nullptr;
	}

	m_uChildren = 0;
}

void Simplex::MyOctant::AssignIDtoEntity()
{
	//assigning the smallest possible leaf to the entities that collide with it

	//assigning id to entities that are in the children to reach the leaf

	//if there are children then go to a child that is a leaf
	if (m_uChildren != 0)
	{
		for (int i = 0; i < m_uChildren; i++)
		{
			m_pChild[i]->AssignIDtoEntity();
		}
	}

	//if this is a leaf node
	if (m_uChildren == 0)
	{
		//get all entities
		uint uEntityCount = m_pEntityMngr->GetEntityCount();

		//loop through all entities to check if it is colliding with this octant
		for (int i = 0; i < uEntityCount; i++)
		{
			if (IsColliding(i))
			{
				//if it is colliding then add it to the entity list and add the dimention to the entity
				m_EntityList.emplace_back(i);

				m_pEntityMngr->AddDimension(i, m_uID);
			}
		}
	}

}

uint Simplex::MyOctant::GetOctantCount()
{
	//return the number of octants
	return m_uOctantCount;
}

MyOctant::~MyOctant()
{
	//call release
	Release();
}

void Simplex::MyOctant::Release()
{
	//if this is a base octant then call kill branches
	if (m_uLevel == 0)
	{
		KillBranches();
	}

	m_uChildren = 0;
	m_fSize = 0;
	m_EntityList.clear();
	m_lChild.clear();
}

void Simplex::MyOctant::Init()
{
	//initializing the number of children, the size, the octant count and the level of the oct tree
	m_uChildren = 0;
	m_fSize = 0;
	m_uID = m_uOctantCount;
	m_uLevel = 0;

	//initializing the center, min and max of oct tree
	m_v3Center = vector3(0.0f);
	m_v3Min = vector3(0.0f);
	m_v3Max = vector3(0.0f);

	//initializing the root and parent to nullptr
	m_pRoot = nullptr;
	m_pParent = nullptr;

	//getting the mesh manager instance
	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = MyEntityManager::GetInstance();

	for (uint i = 0; i < 8; i++)
	{
		m_pChild[i] = nullptr;
	}
}

void Simplex::MyOctant::ConstructList()
{
	//looping and going to the subdivisions until you get to the leaf descendant
	for (size_t i = 0; i < m_uChildren; i++)
	{
		m_pChild[i]->ConstructList();
	}

	//add it to the root's children
	m_pRoot->m_lChild.emplace_back(this);
}

Simplex::MyOctant::MyOctant(uint a_nMaxLevel, uint a_nIdealEntityCount)
{
	Init();

	//setting the number of octants and the max level of subdivision
	m_uOctantCount = 0;
	m_uMaxLevel = a_nMaxLevel;

	//setting the ideal number of entities per octant
	m_uIdealEntityCount = a_nIdealEntityCount;
	m_uID = m_uOctantCount;

	//setting octant as the root octant
	m_pRoot = this;
	m_lChild.clear();

	std::vector<vector3> lMinMax;

	//getting the number of entities on the scene
	uint uObjectsInScene = m_pEntityMngr->GetEntityCount();

	//looping through the number of entities
	for (uint i = 0; i < uObjectsInScene; i++)
	{
		//getting the rigid body of each entity and storing their min and max values
		MyEntity* pEntity = m_pEntityMngr->GetEntity(i);
		MyRigidBody* pRigidBody = pEntity->GetRigidBody();
		lMinMax.emplace_back(pRigidBody->GetMinGlobal());
		lMinMax.emplace_back(pRigidBody->GetMaxGlobal());
	}

	//creating a rigid body with this new set of vertices
	MyRigidBody* pRigidBody = new MyRigidBody(lMinMax);

	//getting the halfwidth of the rigid body with the vertices
	vector3 v3HalfWidth = pRigidBody->GetHalfWidth();

	//finding the maximum value of the bounding object by calculating the maximum coordinate in halfwidth
	float fMax = v3HalfWidth.x;

	for (int i = 1; i < 3; i++)
	{
		if (fMax < v3HalfWidth[i])
		{
			fMax = v3HalfWidth[i];
		}
	}

	//getting the center of the bounding box
	vector3 v3Center = pRigidBody->GetCenterLocal();

	//deleting rigid body
	SafeDelete(pRigidBody);

	//finding the size of the size of the bounding box
	m_fSize = fMax * 2.0f;

	//calculating the min and the max vertex of the bounding box
	m_v3Min = m_v3Center-vector3(fMax);
	m_v3Max = m_v3Center + vector3(fMax);

	//increasing the octant count
	m_uOctantCount++;

	//constructing the tree based on the number of the max level
	Subdivide(a_nMaxLevel);

}

Simplex::MyOctant::MyOctant(vector3 a_v3Center, float a_fSize)
{
	Init();

	//calculating the center, size, and min and max vectors for this child octant
	m_v3Center = a_v3Center;

	m_fSize = a_fSize;

	m_v3Min = m_v3Center - vector3(m_fSize / 2);
	m_v3Max = m_v3Center + vector3(m_fSize / 2);

	m_uOctantCount++;

}

Simplex::MyOctant::MyOctant(const MyOctant & other)
{
	//copy contructor copying data from other to this
	(m_uChildren= other.m_uChildren);
	(m_fSize = other.m_fSize);
	(m_uID =  other.m_uID);
	(m_pRoot =  other.m_pRoot);
	(m_lChild =  other.m_lChild);
	(m_v3Center =  other.m_v3Center);
	(m_v3Min =  other.m_v3Min);
	(m_v3Max =  other.m_v3Max);

	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = MyEntityManager::GetInstance();

	for (uint i = 0; i < 8; i++)
	{
		(m_pChild[i] =  other.m_pChild[i]);
	}
}

MyOctant & Simplex::MyOctant::operator=(const MyOctant & other)
{
	//if this object is not the same as the other
	if (this != &other)
	{
		//release all previously allocated memeory
		Release();

		//copying data from the other octant
		(m_uChildren = other.m_uChildren);
		(m_fSize = other.m_fSize);
		(m_uID = other.m_uID);
		(m_pRoot = other.m_pRoot);
		(m_lChild = other.m_lChild);
		(m_v3Center = other.m_v3Center);
		(m_v3Min = other.m_v3Min);
		(m_v3Max = other.m_v3Max);

		m_pMeshMngr = MeshManager::GetInstance();
		m_pEntityMngr = MyEntityManager::GetInstance();

		for (uint i = 0; i < 8; i++)
		{
			(m_pChild[i] = other.m_pChild[i]);
		}
	}

	return *this;
}

void Simplex::MyOctant::Swap(MyOctant & other)
{
	//swapping data from this octant to the other using std::swap
	std::swap(m_uChildren, other.m_uChildren);
	std::swap(m_fSize, other.m_fSize);
	std::swap(m_uID, other.m_uID);
	std::swap(m_pRoot, other.m_pRoot);
	std::swap(m_lChild, other.m_lChild);
	std::swap(m_v3Center, other.m_v3Center);
	std::swap(m_v3Min, other.m_v3Min);
	std::swap(m_v3Max, other.m_v3Max);

	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = MyEntityManager::GetInstance();

	for (uint i = 0; i < 8; i++)
	{
		std::swap(m_pChild[i], other.m_pChild[i]);
	}


}

float Simplex::MyOctant::GetSize()
{
	//return size of the side of the bounding box
	return m_fSize;
}

vector3 Simplex::MyOctant::GetCenterGlobal()
{
	//return global position of center
	return m_v3Center;
}

vector3 Simplex::MyOctant::GetMinGlobal()
{
	//return global position of min location
	return m_v3Min;
}

vector3 Simplex::MyOctant::GetMaxGlobal()
{
	//return global position of max location
	return m_v3Max;
}

bool Simplex::MyOctant::IsColliding(uint index)
{
	//checking if entity is colliding with the octant using arbb vs arbb collision

	//get the entity using the index

	MyEntity* eEntity = m_pEntityMngr->GetEntity(index);
	MyRigidBody* rRigidBody = eEntity->GetRigidBody();

	//getting the min and max values of the entity
	vector3 v3MinG = rRigidBody->GetMinGlobal();
	vector3 v3MaxG = rRigidBody->GetMaxGlobal();
	vector3 v3Center = rRigidBody->GetCenterGlobal();

	//arbb check for x
	if (v3MaxG.x<m_v3Min.x || v3MinG.x>m_v3Max.x)
	{
		return false;
	}

	//arbb check for y
	if (v3MaxG.y<m_v3Min.y || v3MinG.y>m_v3Max.y)
	{
		return false;
	}

	//arbb check for z
	if (v3MaxG.z<m_v3Min.z || v3MinG.z>m_v3Max.z)
	{
		return false;
	}

	return true;
	
}

void Simplex::MyOctant::Display(uint a_uIndex, vector3 a_v3Color)
{
	//looping through the children until we find the children of the index
	for (int i = 0; i < m_uChildren; i++)
	{
		m_pChild[i]->Display(a_uIndex);
	}

	//adding a wire cube that has size of the octant and the location of the center of the octant
	if (a_uIndex==m_uID)
		m_pMeshMngr->AddWireCubeToRenderList(glm::translate(m_v3Center)*glm::scale(vector3(m_fSize)), a_v3Color);
}

void Simplex::MyOctant::Display(vector3 a_v3Color)
{
	//displaying all the children and sub children of the cube
	for (int i = 0; i < m_uChildren; i++)
	{
		m_pChild[i]->Display();
	}

	//adding a wire cube that has size of the octant and the location of the center of the octant
	if(IsLeaf())
		m_pMeshMngr->AddWireCubeToRenderList(glm::translate(m_v3Center)*glm::scale(vector3(m_fSize)), a_v3Color);
}

void Simplex::MyOctant::Subdivide(uint a_nMaxLevel)
{

	//return if this already has children
	if (m_uChildren != 0)
	{
		return;
	}

	//reducing the max level
	a_nMaxLevel--;

	//clearing the entity list and assigning all entities to that are in this octant
	m_EntityList.clear();
	AssignIDtoEntity();

	//if max level is greater then 0 and the entity list size is greater than ideal count of entities
	//start the subdivide algorithm
	if (a_nMaxLevel > 0&&m_EntityList.size()>m_uIdealEntityCount)
	{
		//remove this octant from the entity's id list
		for (int i = 0; i < m_EntityList.size(); i++)
		{
			m_pEntityMngr->RemoveDimension(m_EntityList[i], m_uID);
		}
		m_EntityList.clear();
		m_uChildren = 8;

		//adding 8 children to this octree
		float fNewSize = m_fSize / 2;

		//8 new centers for the new octants
		vector3 v3Center1;
		vector3 v3Center2;
		vector3 v3Center3;
		vector3 v3Center4;
		vector3 v3Center5;
		vector3 v3Center6;
		vector3 v3Center7;
		vector3 v3Center8;

		//top front right child
		v3Center1 = m_v3Center + vector3(fNewSize / 2);
		m_pChild[0] = new MyOctant(v3Center1, fNewSize);
		m_pChild[0]->m_pRoot = this->m_pRoot;
		m_pChild[0]->m_pParent = this;
		m_pChild[0]->m_uLevel = this->m_uLevel + 1;

		//bottom back left child
		v3Center2 = m_v3Center + vector3(-fNewSize / 2);
		m_pChild[1] = new MyOctant(v3Center2, fNewSize);
		m_pChild[1]->m_pRoot = this->m_pRoot;
		m_pChild[1]->m_pParent = this;
		m_pChild[1]->m_uLevel = this->m_uLevel + 1;

		//top front left child
		v3Center3 = m_v3Center + vector3(-fNewSize / 2, fNewSize / 2, fNewSize / 2);
		m_pChild[2] = new MyOctant(v3Center3, fNewSize);
		m_pChild[2]->m_pRoot = this->m_pRoot;
		m_pChild[2]->m_pParent = this;
		m_pChild[2]->m_uLevel = this->m_uLevel + 1;

		//bottom front right child
		v3Center4 = m_v3Center + vector3(fNewSize / 2, -fNewSize / 2, fNewSize / 2);
		m_pChild[3] = new MyOctant(v3Center4, fNewSize);
		m_pChild[3]->m_pRoot = this->m_pRoot;
		m_pChild[3]->m_pParent = this;
		m_pChild[3]->m_uLevel = this->m_uLevel + 1;

		//bottom front left
		v3Center5 = m_v3Center + vector3(-fNewSize / 2, -fNewSize / 2, fNewSize / 2);
		m_pChild[4] = new MyOctant(v3Center5, fNewSize);
		m_pChild[4]->m_pRoot = this->m_pRoot;
		m_pChild[4]->m_pParent = this;
		m_pChild[4]->m_uLevel = this->m_uLevel + 1;

		//bottom back right
		v3Center6 = m_v3Center + vector3(fNewSize / 2, -fNewSize / 2, -fNewSize / 2);
		m_pChild[5] = new MyOctant(v3Center6, fNewSize);
		m_pChild[5]->m_pRoot = this->m_pRoot;
		m_pChild[5]->m_pParent = this;
		m_pChild[5]->m_uLevel = this->m_uLevel + 1;

		//top back right
		v3Center7 = m_v3Center + vector3(fNewSize / 2, fNewSize / 2, -fNewSize / 2);
		m_pChild[6] = new MyOctant(v3Center7, fNewSize);
		m_pChild[6]->m_pRoot = this->m_pRoot;
		m_pChild[6]->m_pParent = this;
		m_pChild[6]->m_uLevel = this->m_uLevel + 1;

		//top back left
		v3Center8 = m_v3Center + vector3(-fNewSize / 2, fNewSize / 2, -fNewSize / 2);
		m_pChild[7] = new MyOctant(v3Center8, fNewSize);
		m_pChild[7]->m_pRoot = this->m_pRoot;
		m_pChild[7]->m_pParent = this;
		m_pChild[7]->m_uLevel = this->m_uLevel + 1;

		m_uChildren = 8;

		//subdividing each of the further children until the max level is reached
		for (int i = 0; i < m_uChildren; i++)
		{
			m_pChild[i]->Subdivide(a_nMaxLevel);
		}
	}
}
