#include "AppClass.h"
void Application::InitVariables(void)
{
	//Change this to your name and email
	m_sProgrammer = "Shubham Sachdeva - ss1594@g.rit.edu";
	
	//Set the position and target of the camera
	//(I'm at [0,0,10], looking at [0,0,0] and up is the positive Y axis)
	m_pCameraMngr->SetPositionTargetAndUpward(AXIS_Z * 20.0f, ZERO_V3, AXIS_Y);

	//if the light position is zero move it
	if (m_pLightMngr->GetPosition(1) == ZERO_V3)
		m_pLightMngr->SetPosition(vector3(0.0f, 0.0f, 3.0f));

	//if the background is cornflowerblue change it to black (its easier to see)
	if (vector3(m_v4ClearColor) == C_BLUE_CORNFLOWER)
	{
		m_v4ClearColor = vector4(ZERO_V3, 1.0f);
	}
	
	//if there are no segments create 7
	if(m_uOrbits < 1)
		m_uOrbits = 7;

	float fSize = 1.0f; //initial size of orbits
	float fRadius = 0.95; //initial radius of orbits

	//creating a color using the spectrum 
	uint uColor = 650; //650 is Red
	//prevent division by 0
	float decrements = 250.0f / (m_uOrbits > 1? static_cast<float>(m_uOrbits - 1) : 1.0f); //decrement until you get to 400 (which is violet)
	/*
		This part will create the orbits, it start at 3 because that is the minimum subdivisions a torus can have
	*/
	uint uSides = 3; //start with the minimal 3 sides
	for (uint i = uSides; i < m_uOrbits + uSides; i++)
	{
		vector3 v3Color = WaveLengthToRGB(uColor); //calculate color based on wavelength
		m_shapeList.push_back(m_pMeshMngr->GenerateTorus(fSize, fSize - 0.1f, 3, i, v3Color)); //generate a custom torus and add it to the meshmanager
		std::vector<vector3> stopList; //a list of stops to add to the list of list of stops

		//calculate the position of the stops
		//these are the vertices of the toroids calculated using their radial coordinates
		float angle = (2.0f*PI) / i;
		float currAngle = 0;

		//looping through the number of stops
		for (float j = 0; j <i; j++)
		{
			//using trig to calculate the postion of stop at current angle
			float xPos = cos(currAngle)*fRadius;
			float yPos = sin(currAngle)*fRadius;

			//incrementing the current angle
			currAngle += angle;

			//add the stop to the stop list
			stopList.emplace_back(vector3(xPos, yPos, 0.0f));
		}

		//adding the stops of this toroid to the list of stop lists
		listOfStopLists.emplace_back(stopList);
		fSize += 0.5f; //increment the size for the next orbit
		fRadius += 0.5f;
		uColor -= static_cast<uint>(decrements); //decrease the wavelength
	}
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the arcball active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	matrix4 m4View = m_pCameraMngr->GetViewMatrix(); //view Matrix
	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix(); //Projection Matrix
	matrix4 m4Offset = IDENTITY_M4; //offset of the orbits, starts as the global coordinate system

	//calculating the current time
	static float time = 0;
	//getting the system clock
	static uint clock = m_pSystem->GenClock();
	//adding the delta time to the current time
	time += m_pSystem->GetDeltaTime(clock);

	//mapping the value of the time to the percent by converting a range from 0 - 0.6 to 0 - 1.0
	float percent = MapValue(time, 0.0f, 0.6f, 0.0f, 1.0f);

	//vector that holds the current position of all the orbits
	//initializing it to 0 for the number of orbits
	static std::vector<int> currPos(m_uOrbits,0);

	/*
		The following offset will orient the orbits as in the demo, start without it to make your life easier.
	*/
	m4Offset = glm::rotate(IDENTITY_M4, 1.5708f, AXIS_Z);

	//looping through all the orbits to find the current stop on them
	for (uint i = 0; i < m_uOrbits; ++i)
	{
		m_pMeshMngr->AddMeshToRenderList(m_shapeList[i], glm::rotate(m4Offset, 1.5708f, AXIS_X));

		//start position at this time step
		vector3 startPos = listOfStopLists[i][currPos[i]];

		//end position at this time step
		vector3 endPos = listOfStopLists[i][(currPos[i]+1) % listOfStopLists[i].size()];

		//calculate the current position
		vector3 currentPos = ZERO_V3;

		//calculating the current position at this step by linear interpolation
		currentPos = glm::lerp(startPos, endPos, percent);

		//if time is greater than 1 second
		if (percent >= 1.0)
		{
			//increase the position
			currPos[i]++;
			//reset the clock
			time = m_pSystem->GetDeltaTime(clock);
			//make sure curr pos does not exceed the side of the stop list
			currPos[i] %= listOfStopLists[i].size();
		}

		//translating the sphere's model matrix
		matrix4 m4Model = glm::translate(m4Offset, currentPos);

		//draw spheres
		m_pMeshMngr->AddSphereToRenderList(m4Model * glm::scale(vector3(0.1)), C_WHITE);
	}

	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

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