#include "MyRigidBody.h"
using namespace Simplex;
//Allocation
void MyRigidBody::Init(void)
{
	m_pMeshMngr = MeshManager::GetInstance();
	m_bVisibleOBB = true;
	m_bVisibleARBB = true;

	m_fRadius = 0.0f;

	m_v3ColorColliding = C_RED;
	m_v3ColorNotColliding = C_WHITE;

	m_v3Center = ZERO_V3;
	m_v3Minimum = ZERO_V3;
	m_v3Maximum = ZERO_V3;

	m_v3MinimumG = ZERO_V3;
	m_v3MaximumG = ZERO_V3;

	m_v3Size = ZERO_V3;
	m_v3ARBBSize = ZERO_V3;

	m_m4ToWorld = IDENTITY_M4;

	m_OBB = ZERO_V3;
}
void MyRigidBody::Swap(MyRigidBody& other)
{
	std::swap(m_pMeshMngr, other.m_pMeshMngr);
	std::swap(m_bVisibleOBB, other.m_bVisibleOBB);
	std::swap(m_bVisibleARBB, other.m_bVisibleARBB);

	std::swap(m_fRadius, other.m_fRadius);

	std::swap(m_v3ColorColliding, other.m_v3ColorColliding);
	std::swap(m_v3ColorNotColliding, other.m_v3ColorNotColliding);

	std::swap(m_v3Center, other.m_v3Center);
	std::swap(m_v3Minimum, other.m_v3Minimum);
	std::swap(m_v3Maximum, other.m_v3Maximum);

	std::swap(m_v3MinimumG, other.m_v3MinimumG);
	std::swap(m_v3MaximumG, other.m_v3MaximumG);

	std::swap(m_v3Size, other.m_v3Size);
	std::swap(m_v3ARBBSize, other.m_v3ARBBSize);

	std::swap(m_m4ToWorld, other.m_m4ToWorld);

	std::swap(m_CollidingRBSet, other.m_CollidingRBSet);
}
void MyRigidBody::Release(void)
{
	m_pMeshMngr = nullptr;
	ClearCollidingList();
}
//Accessors
bool MyRigidBody::GetVisibleOBB(void) { return m_bVisibleOBB; }
void MyRigidBody::SetVisibleOBB(bool a_bVisible) { m_bVisibleOBB = a_bVisible; }
bool MyRigidBody::GetVisibleARBB(void) { return m_bVisibleARBB; }
void MyRigidBody::SetVisibleARBB(bool a_bVisible) { m_bVisibleARBB = a_bVisible; }
float MyRigidBody::GetRadius(void) { return m_fRadius; }
vector3 MyRigidBody::GetColorColliding(void) { return m_v3ColorColliding; }
vector3 MyRigidBody::GetColorNotColliding(void) { return m_v3ColorNotColliding; }
void MyRigidBody::SetColorColliding(vector3 a_v3Color) { m_v3ColorColliding = a_v3Color; }
void MyRigidBody::SetColorNotColliding(vector3 a_v3Color) { m_v3ColorNotColliding = a_v3Color; }
vector3 MyRigidBody::GetCenterLocal(void) { return m_v3Center; }
vector3 MyRigidBody::GetMinLocal(void) { return m_v3Minimum; }
vector3 MyRigidBody::GetMaxLocal(void) { return m_v3Maximum; }
vector3 MyRigidBody::GetCenterGlobal(void) { return vector3(m_m4ToWorld * vector4(m_v3Center, 1.0f)); }
vector3 MyRigidBody::GetMinGlobal(void) { return m_v3MinimumG; }
vector3 MyRigidBody::GetMaxGlobal(void) { return m_v3MaximumG; }
vector3 MyRigidBody::GetSize(void) { return m_v3Size; }
matrix4 MyRigidBody::GetModelMatrix(void) { return m_m4ToWorld; }
void MyRigidBody::SetModelMatrix(matrix4 a_m4ModelMatrix)
{
	//to save some calculations if the model matrix is the same there is nothing to do here
	if (a_m4ModelMatrix == m_m4ToWorld)
		return;

	//Assign the model matrix
	m_m4ToWorld = a_m4ModelMatrix;

	//Finds each corner
	std::vector<vector3> m_v3Corners = std::vector<vector3>();

	m_v3Corners.push_back(m_v3Minimum);
	m_v3Corners.push_back(m_v3Maximum);
	m_v3Corners.push_back(vector3(m_v3Maximum.x, m_v3Minimum.y, m_v3Minimum.z));
	m_v3Corners.push_back(vector3(m_v3Minimum.x, m_v3Maximum.y, m_v3Minimum.z));
	m_v3Corners.push_back(vector3(m_v3Maximum.x, m_v3Maximum.y, m_v3Minimum.z));
	m_v3Corners.push_back(vector3(m_v3Minimum.x, m_v3Minimum.y, m_v3Maximum.z));
	m_v3Corners.push_back(vector3(m_v3Maximum.x, m_v3Minimum.y, m_v3Maximum.z));
	m_v3Corners.push_back(vector3(m_v3Minimum.x, m_v3Maximum.y, m_v3Maximum.z));

	for (uint i = 0; i < 8; ++i)
	{
		m_v3Corners[i] = vector3(m_m4ToWorld * vector4(m_v3Corners[i], 1.0f));
	}
	m_v3MaximumG = m_v3MinimumG = m_v3Corners[0];
	for (uint i = 1; i < 8; i++)
	{
		if (m_v3MaximumG.x < m_v3Corners[i].x) m_v3MaximumG.x = m_v3Corners[i].x;
		else if (m_v3MinimumG.x > m_v3Corners[i].x) m_v3MinimumG.x = m_v3Corners[i].x;

		if (m_v3MaximumG.y < m_v3Corners[i].y) m_v3MaximumG.y = m_v3Corners[i].y;
		else if (m_v3MinimumG.y > m_v3Corners[i].y) m_v3MinimumG.y = m_v3Corners[i].y;

		if (m_v3MaximumG.z < m_v3Corners[i].z) m_v3MaximumG.z = m_v3Corners[i].z;
		else if (m_v3MinimumG.z > m_v3Corners[i].z) m_v3MinimumG.z = m_v3Corners[i].z;
	}
	m_v3ARBBSize = m_v3MaximumG - m_v3MinimumG;
}
//The big 3
MyRigidBody::MyRigidBody(std::vector<vector3> a_pointList)
{
	Init();
	//Count the points of the incoming list
	uint uVertexCount = a_pointList.size();

	//If there are none just return, we have no information to create the BS from
	if (uVertexCount == 0)
		return;

	m_v3Maximum = m_v3Minimum = a_pointList[0];
	for (uint i = 1; i < uVertexCount; i++)
	{
		if (m_v3Minimum.x > a_pointList[i].x)
			m_v3Minimum.x = a_pointList[i].x;

		else if (m_v3Maximum.x < a_pointList[i].x)
			m_v3Maximum.x = a_pointList[i].x;

		if (m_v3Minimum.y > a_pointList[i].y)
			m_v3Minimum.y = a_pointList[i].y;

		else if (m_v3Maximum.y < a_pointList[i].y)
			m_v3Maximum.y = a_pointList[i].y;

		if (m_v3Minimum.z > a_pointList[i].z)
			m_v3Minimum.z = a_pointList[i].z;

		else if (m_v3Maximum.z < a_pointList[i].z)
			m_v3Maximum.z = a_pointList[i].z;
	}

	m_v3Center = (m_v3Maximum + m_v3Minimum) / 2.0f;

	for (uint i = 0; i < uVertexCount; i++)
	{
		float fDistance = glm::distance(m_v3Center, a_pointList[i]);
		if (fDistance > m_fRadius)
			m_fRadius = fDistance;
	}

	m_v3Size = m_v3Maximum - m_v3Minimum;
}

MyRigidBody::MyRigidBody(MyRigidBody const& other)
{
	m_pMeshMngr = other.m_pMeshMngr;
	m_bVisibleOBB = other.m_bVisibleOBB;
	m_bVisibleARBB = other.m_bVisibleARBB;

	m_fRadius = other.m_fRadius;

	m_v3ColorColliding = other.m_v3ColorColliding;
	m_v3ColorNotColliding = other.m_v3ColorNotColliding;

	m_v3Center = other.m_v3Center;
	m_v3Minimum = other.m_v3Minimum;
	m_v3Maximum = other.m_v3Maximum;

	m_v3MinimumG = other.m_v3MinimumG;
	m_v3MaximumG = other.m_v3MaximumG;

	m_v3Size = other.m_v3Size;
	m_v3ARBBSize = other.m_v3ARBBSize;

	m_m4ToWorld = other.m_m4ToWorld;

	m_CollidingRBSet = other.m_CollidingRBSet;
}
MyRigidBody& MyRigidBody::operator=(MyRigidBody const& other)
{
	if (this != &other)
	{
		Release();
		Init();
		MyRigidBody temp(other);
		Swap(temp);
	}
	return *this;
}
MyRigidBody::~MyRigidBody() { Release(); };

//--- other Methods
void MyRigidBody::AddCollisionWith(MyRigidBody* other)
{
	/*
		check if the object is already in the colliding set, if
		the object is already there return with no changes
	*/
	auto element = m_CollidingRBSet.find(other);
	if (element != m_CollidingRBSet.end())
		return;
	// we couldn't find the object so add it
	m_CollidingRBSet.insert(other);
}
void MyRigidBody::RemoveCollisionWith(MyRigidBody* other)
{
	m_CollidingRBSet.erase(other);
}
void MyRigidBody::ClearCollidingList(void)
{
	m_CollidingRBSet.clear();
}
bool MyRigidBody::IsColliding(MyRigidBody* const other)
{
	//check if spheres are colliding
	bool bColliding = true;

	if (bColliding) //they are colliding with bounding box also
	{
		if (this->m_v3MaximumG.x < other->m_v3MinimumG.x) { bColliding = false; }
		if (this->m_v3MaximumG.y < other->m_v3MinimumG.y) { bColliding = false; }
		if (this->m_v3MaximumG.z < other->m_v3MinimumG.z) { bColliding = false; }
		if (this->m_v3MinimumG.x > other->m_v3MaximumG.x) { bColliding = false; }
		if (this->m_v3MinimumG.y > other->m_v3MaximumG.y) { bColliding = false; }
		if (this->m_v3MinimumG.z > other->m_v3MaximumG.z) { bColliding = false; }

		if (bColliding) //they are colliding with bounding box also
		{
			this->AddCollisionWith(other);
			other->AddCollisionWith(this);
		}
		else //they are not colliding with bounding box
		{
			this->RemoveCollisionWith(other);
			other->RemoveCollisionWith(this);
		}
	}
	else //they are not colliding with bounding box
	{
		this->RemoveCollisionWith(other);
		other->RemoveCollisionWith(this);
	}

	return bColliding;
}

void MyRigidBody::AddToRenderList(void)
{
	if (m_bVisibleOBB)
	{
		if (m_CollidingRBSet.size() > 0)
			m_pMeshMngr->AddWireCubeToRenderList(glm::translate(m_m4ToWorld, m_v3Center) * glm::scale(m_v3Size), m_v3ColorColliding);
		else
			m_pMeshMngr->AddWireCubeToRenderList(glm::translate(m_m4ToWorld, m_v3Center) * glm::scale(m_v3Size), m_v3ColorNotColliding);
	}
	if (m_bVisibleARBB)
	{
		if (m_CollidingRBSet.size() > 0)
			m_pMeshMngr->AddWireCubeToRenderList(glm::translate(GetCenterGlobal()) * glm::scale(m_v3ARBBSize), C_YELLOW);
		else
			m_pMeshMngr->AddWireCubeToRenderList(glm::translate(GetCenterGlobal()) * glm::scale(m_v3ARBBSize), C_YELLOW);
	}
}