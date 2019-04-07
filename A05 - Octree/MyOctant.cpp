#include "MyOctant.h"
using namespace Simplex;

////MyOctant
uint MyOctant::m_uOctantCount = 0;
uint MyOctant::m_uMaxLevel = 3;
uint MyOctant::m_uIdealEntityCount = 5;
uint MyOctant::GetOctantCount(void) { return m_uOctantCount; }
void MyOctant::Init(void) {
	//Initializes the values of the octant
	m_uChildren = 0;
	m_fSize = 0.0f;
	m_uID = m_uOctantCount;
	m_uLevel = 0;

	m_v3Center = vector3(0.0f);
	m_v3Min = vector3(0.0f);
	m_v3Max = vector3(0.0f);

	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = MyEntityManager::GetInstance();

	m_pRoot = nullptr;
	m_pParent = nullptr;
	for (uint i = 0; i < 8; i++)
	{
		m_pChild[i] = nullptr;
	}
}
void MyOctant::Swap(MyOctant& other)
{
	//Swaps two octants
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

	std::swap(m_uLevel, other.m_uLevel);
	std::swap(m_pParent, other.m_pParent);
	for (uint i = 0; i < 8; i++)
	{
		std::swap(m_pChild[i], other.m_pChild[i]);
	}
}
MyOctant * MyOctant::GetParent(void) { return m_pParent; }
void MyOctant::Release(void)
{
	//Releases the octant
	if (m_uLevel == 0)
	{
		KillBranches();
	}
	m_uChildren = 0;
	m_fSize = 0.0f;
	m_EntityList.clear();
	m_lChild.clear();
}
////THREE REQUIRED
MyOctant::MyOctant(uint a_nMaxLevel, uint a_nIdealEntityCount) 
{
	//Initializes the octant
	Init();

	m_uOctantCount = 0;
	m_uMaxLevel = a_nMaxLevel;
	m_uIdealEntityCount = a_nIdealEntityCount;
	m_uID = m_uOctantCount;

	m_pRoot = this;
	m_lChild.clear();

	//Finds the dimensions of the octant
	std::vector<vector3> lMinMax;

	uint nObjects = m_pEntityMngr->GetEntityCount();
	for (uint i = 0; i < nObjects; i++)
	{
		MyEntity* pEntity = m_pEntityMngr->GetEntity(i);
		MyRigidBody* pRigidBody = pEntity->GetRigidBody();
		lMinMax.push_back(pRigidBody->GetMinGlobal());
		lMinMax.push_back(pRigidBody->GetMaxGlobal());
	}
	MyRigidBody* pRigidBody = new MyRigidBody(lMinMax);

	vector3 vHalfWidth = pRigidBody->GetHalfWidth();
	float fMax = vHalfWidth.x;
	for (int i = 1; i < 3; i++)
	{
		if (fMax < vHalfWidth[i])
			fMax = vHalfWidth[i];
	}
	vector3 v3Center = pRigidBody->GetCenterLocal();
	lMinMax.clear();
	SafeDelete(pRigidBody);

	m_fSize = fMax * 2.0f;
	m_v3Center = v3Center;
	m_v3Min = m_v3Center - (vector3(fMax));
	m_v3Max = m_v3Center + (vector3(fMax));

	m_uOctantCount++;

	ConstructTree(m_uMaxLevel);
}
MyOctant::MyOctant(vector3 a_v3Center, float a_fSize)
{
	//Sets up branches
	Init();
	m_v3Center = a_v3Center;
	m_fSize = a_fSize;

	m_v3Min = m_v3Center - (vector3(m_fSize) / 2.0f);
	m_v3Min = m_v3Center + (vector3(m_fSize) / 2.0f);

	m_uOctantCount++;
}
MyOctant::MyOctant(MyOctant const& other)
{
	//Copy constructor
	m_uChildren = other.m_uChildren;
	m_v3Center = other.m_v3Center;
	m_v3Min = other.m_v3Min;
	m_v3Max = other.m_v3Max;

	m_fSize = other.m_fSize;
	m_uID = other.m_uID;
	m_uLevel = other.m_uLevel;
	m_pParent = other.m_pParent;

	m_pRoot, other.m_pRoot;
	m_lChild, other.m_lChild;

	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = MyEntityManager::GetInstance();

	m_uLevel = other.m_uLevel;
	m_pParent = other.m_pParent;
	for (uint i = 0; i < 8; i++)
	{
		m_pChild[i] = other.m_pChild[i];
	}
}
MyOctant& MyOctant::operator=(MyOctant const& other)
{
	//Copy operator
	if (this != &other)
	{
		Release();
		Init();
		MyOctant temp(other);
		Swap(temp);
	}
	return *this;
}
MyOctant::~MyOctant() { Release(); };

////ACCESSORS
float MyOctant::GetSize(void) { return m_fSize; }
vector3 MyOctant::GetCenterGlobal(void) { return m_v3Center; }
vector3 MyOctant::GetMinGlobal(void) { return m_v3Min; }
vector3 MyOctant::GetMaxGlobal(void) { return m_v3Max; }

////METHODS
void MyOctant::Display(uint a_nIndex, vector3 a_v3Color) 
{
	//Display octree of an ID
	if (m_uID == a_nIndex)
	{
		m_pMeshMngr->AddWireCubeToRenderList(glm::translate(IDENTITY_M4, m_v3Center) *
			glm::scale(vector3(m_fSize)), a_v3Color, RENDER_WIRE);
		return;
	}
	for (uint nIndex = 0; nIndex < m_uChildren; nIndex++)
	{
		m_pChild[nIndex]->Display(a_nIndex);
	}
}
void MyOctant::Display(vector3 a_v3Color)
{
	//Display octree and all children
	for (uint nIndex = 0; nIndex < m_uChildren; nIndex++)
	{
		m_pChild[nIndex]->Display(a_v3Color);
	}
	m_pMeshMngr->AddWireCubeToRenderList(glm::translate(IDENTITY_M4, m_v3Center) *
		glm::scale(vector3(m_fSize)), a_v3Color, RENDER_WIRE);
}
void MyOctant::Subdivide(void)
{
	//Subdivides the octree into child octree branches
	if (m_uLevel >= m_uMaxLevel)
		return;

	if (m_uChildren != 0)
		return;

	m_uChildren = 8;

	float fSize = m_fSize / 4.0f;
	float fSizeD = fSize * 2.0f;
	vector3 v3Center;

	//Bottom Left Back
	v3Center = m_v3Center;
	v3Center.x -= fSize;
	v3Center.y -= fSize;
	v3Center.z -= fSize;
	m_pChild[0] = new MyOctant(v3Center, fSizeD);

	//Bottom Right Back
	v3Center.x += fSizeD;
	m_pChild[1] = new MyOctant(v3Center, fSizeD);

	//Bottom Right Front
	v3Center.z += fSizeD;
	m_pChild[2] = new MyOctant(v3Center, fSizeD);

	//Bottom Left Front
	v3Center.x -= fSizeD;
	m_pChild[3] = new MyOctant(v3Center, fSizeD);

	//Top Left Front
	v3Center.y += fSizeD;
	m_pChild[4] = new MyOctant(v3Center, fSizeD);

	//Top Left Back
	v3Center.z -= fSizeD;
	m_pChild[5] = new MyOctant(v3Center, fSizeD);

	//Top Right Back
	v3Center.x += fSizeD;
	m_pChild[6] = new MyOctant(v3Center, fSizeD);

	//Top Right Front
	v3Center.z += fSizeD;
	m_pChild[7] = new MyOctant(v3Center, fSizeD);

	for (uint nIndex = 0; nIndex < 8; nIndex++)
	{
		m_pChild[nIndex]->m_pRoot = m_pRoot;
		m_pChild[nIndex]->m_pParent = this;
		m_pChild[nIndex]->m_uLevel = m_uLevel + 1;
		if (m_pChild[nIndex]->ContainsMoreThan(m_uIdealEntityCount))
		{
			m_pChild[nIndex]->Subdivide();
		}
	}
}
MyOctant * MyOctant::GetChild(uint a_nChild)
{
	//Return a child
	if (a_nChild > 7) return nullptr;
	return m_pChild[a_nChild];
}
bool MyOctant::IsColliding(uint a_uRBIndex)
{
	//Test if entity collides with the octree
	uint nObjectCount = m_pEntityMngr->GetEntityCount();

	if (a_uRBIndex >= nObjectCount)
		return false;

	MyEntity* pEntity = m_pEntityMngr->GetEntity(a_uRBIndex);
	MyRigidBody* pRigidBody = pEntity->GetRigidBody();
	vector3 v3MinO = pRigidBody->GetMinGlobal();
	vector3 v3MaxO = pRigidBody->GetMaxGlobal();

	if (m_v3Max.x < v3MinO.x)
		return false;
	if (m_v3Min.x < v3MaxO.x)
		return false;

	if (m_v3Max.y < v3MinO.y)
		return false;
	if (m_v3Min.y < v3MaxO.y)
		return false;

	if (m_v3Max.z < v3MinO.z)
		return false;
	if (m_v3Min.z < v3MaxO.z)
		return false;

	return true;
}
bool MyOctant::IsLeaf(void) { return m_uChildren == 0; }
bool MyOctant::ContainsMoreThan(uint a_nEntities)
{
	//Check if the octree contains more than the threshold for entities
	uint nCount = 0;
	uint nObjectCount = m_pEntityMngr->GetEntityCount();
	for (uint i = 0; i < nObjectCount; i++)
	{
		if (IsColliding(i))
			nCount++;
		if (nCount > a_nEntities)
			return true;
	}
	return false;
}
bool MyOctant::ContainsThisMany()
{
	//Testing method, finds number of entities in the octree
	uint nCount = 0;
	uint nObjectCount = m_pEntityMngr->GetEntityCount();
	for (uint i = 0; i < nObjectCount; i++)
	{
		if (IsColliding(i))
			nCount++;
	}
	return nCount;
}
void MyOctant::KillBranches(void)
{
	//Destroys all child octrees
	for (uint nIndex = 0; nIndex < m_uChildren; nIndex++)
	{
		m_pChild[nIndex]->KillBranches();
		delete m_pChild[nIndex];
		m_pChild[nIndex] = nullptr;
	}
	m_uChildren = 0;
}
void MyOctant::DisplayLeafs(vector3 a_v3Color)
{
	//Displays all octree "leafs"
	uint nLeafs = m_lChild.size();
	for (uint nChild = 0; nChild < nLeafs; nChild++)
	{
		m_lChild[nChild]->DisplayLeafs(a_v3Color);
	}
	m_pMeshMngr->AddWireCubeToRenderList(glm::translate(IDENTITY_M4, m_v3Center) *
		glm::scale(vector3(m_fSize)), a_v3Color, RENDER_WIRE);
}
void MyOctant::ClearEntityList(void)
{
	//Clears entities from list
	for (uint nChild = 0; nChild < m_uChildren; nChild++)
	{
		m_pChild[nChild]->ClearEntityList();
	}
	m_EntityList.clear();
}
void MyOctant::ConstructTree(uint a_nMaxLevel)
{
	//Constructs an octree from the root
	if (m_uLevel != 0)
		return;

	m_uMaxLevel = a_nMaxLevel;

	m_uOctantCount = 1;

	ClearEntityList();
	KillBranches();
	m_lChild.clear();

	if (true) //Should use ContainsMoreThan(m_uIdealEntityCount) normally
	{
		Subdivide();
	}

	AssignIDtoEntity();
	ConstructList();
}
void MyOctant::AssignIDtoEntity(void)
{
	for (uint nChild = 0; nChild < m_uChildren; nChild++)
	{
		m_pChild[nChild]->AssignIDtoEntity();
	}
	if (m_uChildren == 0)
	{
		uint nEntities = m_pEntityMngr->GetEntityCount();
		for (uint nIndex = 0; nIndex < nEntities; nIndex++)
		{
			if (IsColliding(nIndex))
			{
				m_EntityList.push_back(nIndex);
				m_pEntityMngr->AddDimension(nIndex, m_uID);
			}
		}
	}
}
void MyOctant::ConstructList(void)
{
	for (uint nChild = 0; nChild < m_uChildren; nChild++)
	{
		m_pChild[nChild]->ConstructList();
	}

	if (m_EntityList.size() > 0)
	{
		m_pRoot->m_lChild.push_back(this);
	}
}