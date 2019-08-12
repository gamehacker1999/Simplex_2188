#include "AppClass.h"
#include<random>
using namespace Simplex;
void Application::InitVariables(void)
{
	m_sProgrammer = "Shubham Sachdeva - ss1594@g.rit.edu";

	//Set the position and target of the camera
	m_pCameraMngr->SetPositionTargetAndUpward(
		vector3(0.0f, 9.0f, 20.0f), //Position
		vector3(0.0f, 0.0f, 0.0f),	//Target
		AXIS_Y);					//Up

	m_pLightMngr->SetPosition(vector3(0.0f, 3.0f, 13.0f), 1); //set the position of first light (0 is reserved for ambient light)

	//setting the start and end positions
	m_v3startPos = vector3(0,1, 3);
	m_v3endPos = vector3(19,1,19);
															 
	m_pEntityMngr = MyEntityManager::GetInstance(); //Initialize the entity manager

	//adding the blocks on the scene for the path following
	//I am making a 20x20 maze on which the path follower and target will be placed
	//This maze data has specifications for where to put the blocks and obstacles
	for (int i = 0; i < 20; i++)
	{
		for (int j = 0; j < 20; j++)
		{
			int ixLoc = i - 10;
			int izLoc = j - 10;
			if (m_i2DMaze[i][j] > 0)
			{
				m_pEntityMngr->AddEntity("Minecraft\\Cube.obj", "Cube_" + std::to_string(j*20+i));
				vector3 v3position = vector3(ixLoc, 0, izLoc);
				matrix4 m4Position = glm::translate(v3position);
				m_pEntityMngr->SetModelMatrix(m4Position);
			}
		}
	}

	//steve will be the path follower, Adding steve to the scene and initializing him in the starting position
	m_pEntityMngr->AddEntity("Minecraft\\Steve.obj", "Steve");
	vector3 v3stevePosition = vector3(m_v3startPos.x-10+0.5, 1, m_v3startPos.z-10+0.5);

	//the creeper will represent the target, we can move the creeper around to change the end position
	m_pEntityMngr->AddEntity("Minecraft\\Creeper.obj", "Creeper");
	m_v3TargetPos = vector3(m_v3endPos.x - 10 + 0.5, 1, m_v3endPos.z - 10 + 0.5);
	m_pEntityMngr->SetModelMatrix(glm::translate(m_v3TargetPos));

	//adding random zombies across the maze in the form of obstacles that the path follower has to push away
	//these will be placed across the maze and will represents nodes which have a higher cost
	std::random_device rd;
	std::mt19937 eng(rd());
	std::uniform_int_distribution<int> dist(0, 19);
	for (size_t i = 0; i < 20; i++)
	{
		float xPos = dist(eng);
		float yPos = dist(eng);

		//if the node is a traversable node then add a zombie there and then increase the cost of that node
		if (m_i2DMaze[int(xPos)][int(yPos)] != 0)
		{
			xPos -= 9.5;
			yPos -= 9.5;
			vector3 v3PigPos = vector3(RoundToNearestHalf(xPos), 1, RoundToNearestHalf(yPos));
			m_pEntityMngr->AddEntity("Minecraft\\Zombie.obj", "Zombie" + std::to_string(i));
			m_pEntityMngr->SetModelMatrix(glm::translate(v3PigPos));
			m_pEntityMngr->UsePhysicsSolver();
		}
	}

	//adding the path follower at the starting position

	//initializing the graph class by sending the maze data, start and end vertex
	m_gGraph.SetStartVertex(m_v3startPos.x, m_v3startPos.z);
	m_gGraph.SetEndVertex(m_v3endPos.x, m_v3endPos.z);

	m_gGraph.width = 20;
	m_gGraph.height = 20;

	//making a temporary pointer to hold the data
	int* intermediateMaze[20];

	//adding the maze data to this temporary pointer
	for (size_t i = 0; i < 20; i++)
	{
		intermediateMaze[i] = m_i2DMaze[i];
	}

	//setting the maze
	m_gGraph.SetMaze(intermediateMaze);
	
	//finding the shortest path between the start and end points
	m_gGraph.FindShortestPath(m_vPath);

	//if path is found then do the necessary calculation
	m_bPathFound = false;
	if (m_vPath.size() > 0)
	{
		m_bPathFound = true;
	}

	//initializing the clock
	uClock = m_pSystem->GenClock();

	//initializing the octree to make the scene a little bit more efficient
	m_pRoot = new MyOctant(m_uOctantCount, 3);

}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the arcball active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();

	//Update Entity Manager
	m_pEntityMngr->Update();

	//using collision detection to represent whether the path follower has reached the target
	//get steve's rigid body and see if it collides with the creeper
	//if it does then that means that the path has been found
	auto steveRB = m_pEntityMngr->GetRigidBody("Steve");

	bool bIsPathFound = steveRB->IsColliding(m_pEntityMngr->GetRigidBody("Creeper"));

	//printing out whether the path has been found or not
	m_pMeshMngr->Print("\nIs Path Found: ",C_YELLOW);

	if (bIsPathFound)
	{
		m_pMeshMngr->Print("YES!!!",C_RED);
	}

	else
	{
		m_pMeshMngr->Print("NO...",C_BLUE);
	}

	//recalculating the octree every two seconds in order to account 
	//for the moving of steve and the zombies
	static uint nClock = m_pSystem->GenClock();
	static bool bStarted = false;
	if (m_pSystem->IsTimerDone(nClock) || !bStarted)
	{
		bStarted = true;
		m_pSystem->StartTimerOnClock(0.5, nClock);
		SafeDelete(m_pRoot);
		m_pRoot = new MyOctant(m_uOctantCount, 5);
	}
	//Add objects to render list
	m_pEntityMngr->AddEntityToRenderList(-1, true);

	//if the user presses the arrow keys they can move the creeper
	//in this case the path has to be recalculated
	if (m_bHasMoved == true)
	{
		m_v3endPos=m_v3TargetPos;
		CalculatePath();
		m_bHasMoved = false;
	}
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	if (m_bIsOctreeVisible)
	{
		m_pRoot->Display(C_YELLOW);
	}
	
	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();

	//if the path is found
	if (m_bPathFound)
	{

		for (size_t i = 0; i < 20; i++)
		{
			for (size_t j = 0; j < 20; j++)
			{
				//for each valid node draw a green plane to represent a node which is not in the path,
				// a red node to represent the path
				//and a magenta node to represent possible paths but with a higher cost
				if (m_i2DMaze[i][j] > 0)
				{
					Vertex v(i, j);
					int ixLoc = i - 10;
					int izLoc = j - 10;

					//positioning these cubes above the node
					vector3 v3CubePosition = vector3(ixLoc + 0.5, 1.1, izLoc + 0.5);
					matrix4 m4ModelMatrix = glm::translate(v3CubePosition)*glm::scale(vector3(0.9, 0.02, 0.9));

					if (std::find(m_vPath.begin(), m_vPath.end(), v) == m_vPath.end())
					{
						if (m_i2DMaze[i][j] == 10)
							m_pMeshMngr->AddCubeToRenderList(m4ModelMatrix, C_MAGENTA);

						else
							m_pMeshMngr->AddCubeToRenderList(m4ModelMatrix, C_GREEN);
					}

					else
					{
						m_pMeshMngr->AddCubeToRenderList(m4ModelMatrix, C_RED);
					}
				}

			}
		}
	}

	int currPos = m_vPath.size() - 1;
	//calculate the current position
	static vector3 v3CurrentPos = vector3(0.0f, 0.0f, 0.0f);

	if (currPos - 1 >= 0)
	{
		fTimer += m_pSystem->GetDeltaTime(uClock); //get the delta time for that timer

		//mapping the time value onto the percent value from 0 to 1
		float fPercent = MapValue(fTimer, 0.0f, 0.6f, 0.0f, 1.f);

		//setting the start and end position
		vector3 v3startPos = vector3(m_vPath[currPos].xPos-10+0.5, 1, m_vPath[currPos].yPos - 10 + 0.5);
		vector3 v3endPos = vector3(m_vPath[currPos - 1].xPos - 10 + 0.5, 1, m_vPath[currPos - 1].yPos - 10 + 0.5);

		//if percent is more than 1
		if (fPercent >= 1.0f)
		{
			fPercent = 1.f;
			//go to the next position
			currPos--;
			fTimer = m_pSystem->GetDeltaTime(uClock); //reset clock
			m_vPath.pop_back();
		}

		//linear interpolation between start and end
		v3CurrentPos = glm::lerp(v3startPos, v3endPos, fPercent);
		m_v3curPos = v3CurrentPos;

		//calculating steve's veclocity by subtracting the current position from the previous position
		auto steveRB = m_pEntityMngr->GetRigidBody("Steve");
		vector3 v3SteveVelocity = m_v3curPos - v3startPos;

		//checking steve's collision with all the zombies in the scene
		for (size_t i = 0; i < m_pEntityMngr->GetEntityCount(); i++)
		{
			auto curEntity = m_pEntityMngr->GetEntity(i);
			vector3 v3EntityPos = curEntity->GetPosition();

			//if steve and zombies are colliding
			if (steveRB->IsColliding(curEntity->GetRigidBody()) &&
				curEntity->GetUniqueID().find("Zombie") != std::string::npos)
			{
				//apply this velocity as force to zombie
				curEntity->ApplyForce(v3SteveVelocity);
				m_pEntityMngr->Update();
				v3EntityPos = curEntity->GetPosition();
			}
		}
	}

	//setting the model matrix for steve and creeper
	m_pEntityMngr->SetModelMatrix(glm::translate(m_v3curPos), "Steve");

	m_pEntityMngr->SetModelMatrix(glm::translate(m_v3TargetPos), "Creeper");

	int calls = m_pMeshMngr->Render();
	//clear the render list
	m_pMeshMngr->ClearRenderList();
	
	//draw gui
	DrawGUI();
	
	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}

void Application::Release(void)
{
	//release GUI
	ShutdownGUI();
}

bool Application::CalculatePath()
{
	uClock = m_pSystem->GenClock(); //generate a new clock for that timer
	fTimer = 0;
	//generate a new graph
	m_gGraph.ClearGraph();
	m_gGraph = Graph();

	//set the start and end vertex of this new graph
	m_gGraph.SetStartVertex(RoundToNearestHalf(m_v3curPos.x + 9.5) , RoundToNearestHalf(m_v3curPos.z + 9.5));
	m_gGraph.SetEndVertex(RoundToNearestHalf(m_v3TargetPos.x + 9.5) , RoundToNearestHalf(m_v3TargetPos.z + 9.5));

	//setting the width and height
	m_gGraph.width = 20;
	m_gGraph.height = 20;

	//making a temporary pointer to hold the data
	int* intermediateMaze[20];

	//storing data in the intermediate pointer
	for (size_t i = 0; i < 20; i++)
	{
		intermediateMaze[i] = m_i2DMaze[i];
	}

	//setting the maze
	m_gGraph.SetMaze(intermediateMaze);

	//clearing the path and calculating a new one
	m_vPath.clear();
	m_gGraph.FindShortestPath(m_vPath);

	return true;
}