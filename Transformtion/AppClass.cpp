#include "AppClass.h"
void Application::InitVariables(void)
{
	//init the mesh
	m_pMesh = new MyMesh();
	//m_pMesh->GenerateCube(1.0f, C_WHITE);
	m_pMesh->GenerateSphere(1.0f, 5, C_WHITE);

	m_uMeshCount = 46;

	//allocate memory for all the meshes
	for (int i = 0; i < m_uMeshCount; i++)
	{
		MyMesh* myMesh = new MyMesh();
		m_pMeshes.emplace_back(myMesh);
		m_pMeshes[i]->GenerateCube(1.0, C_BLACK);
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

	m_pModelMatrices.clear();
	matrix4 m4View = m_pCameraMngr->GetViewMatrix();
	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix();
	
	static float deltaPos = 0;

	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 11; j++)
		{
			int xLoc = 0;
			int yLoc = 0;

			xLoc = j-5;
			yLoc = i;

			if (m_pMeshLocation[i][j] == 1)
			{
				float deltaYPos = sin(deltaPos);
				matrix4 movement = glm::translate(vector3(deltaPos, deltaYPos, 0));
				m_pModelMatrices.emplace_back(movement*glm::translate(vector3(xLoc, yLoc, 1)));
			}
		}
	}
	//matrix4 m4Space = glm::translate(deltaPos, 0, 0);
	for (int i = 0; i < m_pMeshes.size(); i++)
	{
		m_pMeshes[i]->Render(m4Projection,m4View,m_pModelMatrices[i]);
	}

	deltaPos += 0.05;

	//matrix4 m4Model = m4Translate * m4Scale;
	matrix4 m4Model = IDENTITY_M4;

	//m_pMesh->Render(m4Projection, m4View, m4Model);
	
	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();
	
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
	SafeDelete(m_pMesh);
	for (int i = 0; i < m_pMeshes.size(); i++)
	{
		SafeDelete(m_pMeshes[i]);
	}

	//release GUI
	ShutdownGUI();
}