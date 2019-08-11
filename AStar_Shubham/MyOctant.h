#ifndef __MYOCTANTCLASS_H_
#define __MYOCTANTCLASS_H_

#include"MyEntityManager.h"
#include "Simplex/Simplex.h"
#include "Simplex/Physics/Octant.h"
namespace Simplex
{
	class MyOctant
	{

		static uint m_uOctantCount; // stores the number of octants
		static uint m_uMaxLevel;  //stores the maximum level of octants
		static uint m_uIdealEntityCount; //ideal number of entities in an octant

		uint m_uID = 0; //Will store the current ID for this octant
		uint m_uLevel = 0; //Will store the current level of the octant
		uint m_uChildren = 0;// Number of children on the octant (either 0 or 8)

		float m_fSize = 0.0f; //Size of the octant

		MeshManager* m_pMeshMngr = nullptr;//Mesh Manager singleton
		MyEntityManager* m_pEntityMngr = nullptr; //Entity Manager Singleton

		vector3 m_v3Center = vector3(0.0f); //Will store the center point of the octant
		vector3 m_v3Min = vector3(0.0f); //Will store the minimum vector of the octant
		vector3 m_v3Max = vector3(0.0f); //Will store the maximum vector of the octant

		MyOctant* m_pParent = nullptr;// Will store the parent of current octant
		MyOctant* m_pChild[8];//Will store the children of the current octant

		std::vector<uint> m_EntityList; //List of Entities under this octant (Index in Entity Manager)

		MyOctant* m_pRoot = nullptr;//Root octant
		std::vector<MyOctant*> m_lChild; //list of nodes that contain objects (this will be applied to root only)

	public:

		//contructor that takes in the maximum level allowed and the idealnumber of entities in one octant
		MyOctant(uint a_nMaxLevel = 2, uint a_nIdealEntityCount = 5);

		//contructor that defines the location of the center of the octant and the size of the octant
		MyOctant(vector3 a_v3Center, float a_fSize);

		//copy constructor
		MyOctant(const MyOctant& other);

		//copy assignment
		MyOctant& operator = (const MyOctant& other);

		//swaps data between this and the next octant
		void Swap(MyOctant& other);

		//accessor - gets the size of the octant
		float GetSize();

		//accessor - gets center of octant
		vector3 GetCenterGlobal();

		//accessor gets the min vertex of octant
		vector3 GetMinGlobal();

		//accessor - gets the max global of the octant
		vector3 GetMaxGlobal();

		//function to get if there is a collision with the entity specified by the entity manager
		bool IsColliding(uint index);

		//function to display the octant specified by its index
		void Display(uint a_uIndex, vector3 a_v3Color = C_YELLOW);

		//function to display octant specified by the color
		void Display(vector3 a_v3Color = C_YELLOW);

		//allocates 8 smaller octants in the child pointers
		void Subdivide(uint a_nMaxLevel = 3);

		//gets the child specified by the index from 0 - 7
		MyOctant* GetChild(uint a_nChild);

		//function to check if the octant contains any children
		bool IsLeaf();

		//Asks the Octant if it contains more than this many Bounding Objects
		bool ContainsMoreThan(uint a_nEntities);

		//deletes all the children and subchildren of this node
		void KillBranches();

		// Traverse the tree up to the leafs and sets the objects in them to the index
		void AssignIDtoEntity();

		//gets the total number of octants in the world
		uint GetOctantCount();

		//destructor
		~MyOctant();

	private:

		//deallocate all memory
		void Release();

		//allocate all memory and variables
		void Init();

		//construct a list of all leaves that contain objects
		void ConstructList();

	};

}

#endif

