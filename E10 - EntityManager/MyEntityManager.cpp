#include "MyEntityManager.h"
using namespace Simplex;
//  MyEntityManager
MyEntityManager* MyEntityManager::m_pInstance = nullptr;
void MyEntityManager::Init(void)
{
}
void MyEntityManager::Release(void)
{
}
MyEntityManager* MyEntityManager::GetInstance()
{
	if (m_pInstance == nullptr)
	{
		m_pInstance = new MyEntityManager();
	}
	return m_pInstance;
}
void MyEntityManager::ReleaseInstance()
{
}
int Simplex::MyEntityManager::GetEntityIndex(String a_sUniqueID)
{
	return -1;
}
//Accessors
Model* Simplex::MyEntityManager::GetModel(uint a_uIndex)
{
	return nullptr;
}
Model* Simplex::MyEntityManager::GetModel(String a_sUniqueID)
{
	return nullptr;
}
RigidBody* Simplex::MyEntityManager::GetRigidBody(uint a_uIndex)
{
	return nullptr;
}
RigidBody* Simplex::MyEntityManager::GetRigidBody(String a_sUniqueID)
{
	return nullptr;
}
matrix4 Simplex::MyEntityManager::GetModelMatrix(uint a_uIndex)
{
	return IDENTITY_M4;
}
matrix4 Simplex::MyEntityManager::GetModelMatrix(String a_sUniqueID)
{
	return IDENTITY_M4;
}
void Simplex::MyEntityManager::SetModelMatrix(matrix4 a_m4ToWorld, String a_sUniqueID)
{
	/*
	for (uint i = 0; i < m_uEntityCount; i++)
	{
		if (m_entityList[i]->GetUniqueID() == a_sUniqueID)
		{
			SetModelMatrix(a_m4ToWorld,i);
			break;
		}
	}
	*/
}
void Simplex::MyEntityManager::SetModelMatrix(matrix4 a_m4ToWorld, uint a_uIndex)
{
	if (a_uIndex < 0 || a_uIndex < m_uEntityCount)
	{
		m_entityList[a_uIndex]->SetModelMatrix(a_m4ToWorld);
	}
}
//The big 3
MyEntityManager::MyEntityManager(){Init();}
MyEntityManager::MyEntityManager(MyEntityManager const& other){ }
MyEntityManager& MyEntityManager::operator=(MyEntityManager const& other) { return *this; }
MyEntityManager::~MyEntityManager(){Release();};
// other methods
void Simplex::MyEntityManager::Update(void)
{
	AddEntityToRenderList(-1, true);
}
void Simplex::MyEntityManager::AddEntity(String a_sFileName, String a_sUniqueID)
{
	m_uEntityCount++; 
	m_entityList.push_back(&MyEntity(a_sFileName, a_sUniqueID));
}
void Simplex::MyEntityManager::RemoveEntity(uint a_uIndex)
{
	m_uEntityCount--;
	m_entityList.erase(m_entityList.begin() + a_uIndex);
}
void Simplex::MyEntityManager::RemoveEntity(String a_sUniqueID)
{
	for (uint i = 0; i < m_uEntityCount; i++)
	{
		if (m_entityList[i]->GetUniqueID() == a_sUniqueID)
		{
			RemoveEntity(i);
			break;
		}
	}
}
String Simplex::MyEntityManager::GetUniqueID(uint a_uIndex)
{
	return "";
}
MyEntity* Simplex::MyEntityManager::GetEntity(uint a_uIndex)
{
	return nullptr;
}
void Simplex::MyEntityManager::AddEntityToRenderList(uint a_uIndex, bool a_bRigidBody)
{
	if (a_uIndex < 0)
	{
		for (uint i = 0; i < m_uEntityCount; i++)
		{
			AddEntityToRenderList(i, a_bRigidBody);
		}
	}
	else if (a_uIndex < m_uEntityCount)
	{
		m_entityList[a_uIndex]->GetModel()->AddToRenderList();
		if (a_bRigidBody)
		{
			m_entityList[a_uIndex]->GetRigidBody()->AddToRenderList();
		}
	}
}
void Simplex::MyEntityManager::AddEntityToRenderList(String a_sUniqueID, bool a_bRigidBody)
{
	for (uint i = 0; i < m_uEntityCount; i++)
	{
		if (m_entityList[i]->GetUniqueID() == a_sUniqueID)
		{
			AddEntityToRenderList(i, a_bRigidBody);
			break;
		}
	}
}