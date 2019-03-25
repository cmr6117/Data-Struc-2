#include "AppClass.h"
using namespace Simplex;
void Application::InitVariables(void)
{
	//Change this to your name and email
	m_sProgrammer = "Christian Rego - cmr6117@rit.edu";

	//Set the position and target of the camera
	m_pCameraMngr->SetPositionTargetAndUpward(
		vector3(0.0f, 3.0f, 13.0f), //Position
		vector3(0.0f, 3.0f, 12.0f),	//Target
		AXIS_Y);					//Up

	m_pLightMngr->SetPosition(vector3(0.0f, 3.0f, 13.0f), 1); //set the position of first light(0 is reserved for global light)

	//creeper
	m_pCreeper = new Model();
	m_pCreeper->Load("Minecraft\\Creeper.obj");
	m_pCreeperRB = new MyRigidBody(m_pCreeper->GetVertexList());

	//steve
	m_pSteve = new Model();
	m_pSteve->Load("Minecraft\\Steve.obj");
	m_pSteveRB = new MyRigidBody(m_pSteve->GetVertexList());
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the ArcBall active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();

	//Set model matrix to the creeper
	matrix4 mCreeper = glm::translate(m_v3Creeper) * ToMatrix4(m_qCreeper) * ToMatrix4(m_qArcBall);
	m_pCreeper->SetModelMatrix(mCreeper);
	m_pCreeperRB->SetModelMatrix(mCreeper);
	m_pMeshMngr->AddAxisToRenderList(mCreeper);

	//Set model matrix to Steve
	matrix4 mSteve = glm::translate(vector3(2.25f, 0.0f, 0.0f)) * glm::rotate(IDENTITY_M4, glm::radians(-55.0f), AXIS_Z);
	m_pSteve->SetModelMatrix(mSteve);
	m_pSteveRB->SetModelMatrix(mSteve);
	m_pMeshMngr->AddAxisToRenderList(mSteve);

	bool bColliding = m_pCreeperRB->IsColliding(m_pSteveRB);


	m_pCreeper->AddToRenderList();
	m_pCreeperRB->AddToRenderList();

	m_pSteve->AddToRenderList();
	m_pSteveRB->AddToRenderList();

	m_pMeshMngr->Print("Colliding: ");
	if (bColliding)
		m_pMeshMngr->PrintLine("YES!", C_RED);
	else
		m_pMeshMngr->PrintLine("no", C_YELLOW);
	switch (m_pCreeperRB->m_satResult)
	{
		case 0:
			m_pMeshMngr->Print("SAT_NONE", C_RED);
			break;
		case 1:
			m_pMeshMngr->Print("SAT_AX", C_BLUE);
			break;
		case 2:
			m_pMeshMngr->Print("SAT_AY", C_BLUE);
			break;
		case 3:
			m_pMeshMngr->Print("SAT_AZ", C_BLUE);
			break;
		case 4:
			m_pMeshMngr->Print("SAT_BX", C_BLUE);
			break;
		case 5:
			m_pMeshMngr->Print("SAT_BY", C_BLUE);
			break;
		case 6:
			m_pMeshMngr->Print("SAT_BZ", C_BLUE);
			break;
		case 7:
			m_pMeshMngr->Print("SAT_AXxBX", C_BLUE);
			break;
		case 8:
			m_pMeshMngr->Print("SAT_AXxBY", C_BLUE);
			break;
		case 9:
			m_pMeshMngr->Print("SAT_AXxBZ", C_BLUE);
			break;
		case 10:
			m_pMeshMngr->Print("SAT_AYxBX", C_BLUE);
			break;
		case 11:
			m_pMeshMngr->Print("SAT_AYxBY", C_BLUE);
			break;
		case 12:
			m_pMeshMngr->Print("SAT_AYxBZ", C_BLUE);
			break;
		case 13:
			m_pMeshMngr->Print("SAT_AZxBX", C_BLUE);
			break;
		case 14:
			m_pMeshMngr->Print("SAT_AZxBY", C_BLUE);
			break;
		case 15:
			m_pMeshMngr->Print("SAT_AZxBZ", C_BLUE);
			break;
		default:
			break;
	}
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();
	
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

	//release the model
	SafeDelete(m_pCreeper);

	//release the rigid body for the model
	SafeDelete(m_pCreeperRB);

	//release the model
	SafeDelete(m_pSteve);

	//release the rigid body for the model
	SafeDelete(m_pSteveRB);

	//release GUI
	ShutdownGUI();
}