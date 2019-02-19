#include "MyMesh.h"
void MyMesh::Init(void)
{
	m_bBinded = false;
	m_uVertexCount = 0;

	m_VAO = 0;
	m_VBO = 0;

	m_pShaderMngr = ShaderManager::GetInstance();
}
void MyMesh::Release(void)
{
	m_pShaderMngr = nullptr;

	if (m_VBO > 0)
		glDeleteBuffers(1, &m_VBO);

	if (m_VAO > 0)
		glDeleteVertexArrays(1, &m_VAO);

	m_lVertex.clear();
	m_lVertexPos.clear();
	m_lVertexCol.clear();
}
MyMesh::MyMesh()
{
	Init();
}
MyMesh::~MyMesh() { Release(); }
MyMesh::MyMesh(MyMesh& other)
{
	m_bBinded = other.m_bBinded;

	m_pShaderMngr = other.m_pShaderMngr;

	m_uVertexCount = other.m_uVertexCount;

	m_VAO = other.m_VAO;
	m_VBO = other.m_VBO;
}
MyMesh& MyMesh::operator=(MyMesh& other)
{
	if (this != &other)
	{
		Release();
		Init();
		MyMesh temp(other);
		Swap(temp);
	}
	return *this;
}
void MyMesh::Swap(MyMesh& other)
{
	std::swap(m_bBinded, other.m_bBinded);
	std::swap(m_uVertexCount, other.m_uVertexCount);

	std::swap(m_VAO, other.m_VAO);
	std::swap(m_VBO, other.m_VBO);

	std::swap(m_lVertex, other.m_lVertex);
	std::swap(m_lVertexPos, other.m_lVertexPos);
	std::swap(m_lVertexCol, other.m_lVertexCol);

	std::swap(m_pShaderMngr, other.m_pShaderMngr);
}
void MyMesh::CompleteMesh(vector3 a_v3Color)
{
	uint uColorCount = m_lVertexCol.size();
	for (uint i = uColorCount; i < m_uVertexCount; ++i)
	{
		m_lVertexCol.push_back(a_v3Color);
	}
}
void MyMesh::AddVertexPosition(vector3 a_v3Input)
{
	m_lVertexPos.push_back(a_v3Input);
	m_uVertexCount = m_lVertexPos.size();
}
void MyMesh::AddVertexColor(vector3 a_v3Input)
{
	m_lVertexCol.push_back(a_v3Input);
}
void MyMesh::CompileOpenGL3X(void)
{
	if (m_bBinded)
		return;

	if (m_uVertexCount == 0)
		return;

	CompleteMesh();

	for (uint i = 0; i < m_uVertexCount; i++)
	{
		//Position
		m_lVertex.push_back(m_lVertexPos[i]);
		//Color
		m_lVertex.push_back(m_lVertexCol[i]);
	}
	glGenVertexArrays(1, &m_VAO);//Generate vertex array object
	glGenBuffers(1, &m_VBO);//Generate Vertex Buffered Object

	glBindVertexArray(m_VAO);//Bind the VAO
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);//Bind the VBO
	glBufferData(GL_ARRAY_BUFFER, m_uVertexCount * 2 * sizeof(vector3), &m_lVertex[0], GL_STATIC_DRAW);//Generate space for the VBO

	// Position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)0);

	// Color attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)(1 * sizeof(vector3)));

	m_bBinded = true;

	glBindVertexArray(0); // Unbind VAO
}
void MyMesh::Render(matrix4 a_mProjection, matrix4 a_mView, matrix4 a_mModel)
{
	// Use the buffer and shader
	GLuint nShader = m_pShaderMngr->GetShaderID("Basic");
	glUseProgram(nShader); 

	//Bind the VAO of this object
	glBindVertexArray(m_VAO);

	// Get the GPU variables by their name and hook them to CPU variables
	GLuint MVP = glGetUniformLocation(nShader, "MVP");
	GLuint wire = glGetUniformLocation(nShader, "wire");

	//Final Projection of the Camera
	matrix4 m4MVP = a_mProjection * a_mView * a_mModel;
	glUniformMatrix4fv(MVP, 1, GL_FALSE, glm::value_ptr(m4MVP));
	
	//Solid
	glUniform3f(wire, -1.0f, -1.0f, -1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);  

	//Wire
	glUniform3f(wire, 1.0f, 0.0f, 1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_POLYGON_OFFSET_LINE);
	glPolygonOffset(-1.f, -1.f);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);
	glDisable(GL_POLYGON_OFFSET_LINE);

	glBindVertexArray(0);// Unbind VAO so it does not get in the way of other objects
}
void MyMesh::AddTri(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft)
{
	//C
	//| \
	//A--B
	//This will make the triangle A->B->C 
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);
}
void MyMesh::AddQuad(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft, vector3 a_vTopRight)
{
	//C--D
	//|  |
	//A--B
	//This will make the triangle A->B->C and then the triangle C->B->D
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);

	AddVertexPosition(a_vTopLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopRight);
}
void MyMesh::GenerateCube(float a_fSize, vector3 a_v3Color)
{
	if (a_fSize < 0.01f)
		a_fSize = 0.01f;

	Release();
	Init();

	float fValue = a_fSize * 0.5f;
	//3--2
	//|  |
	//0--1

	vector3 point0(-fValue,-fValue, fValue); //0
	vector3 point1( fValue,-fValue, fValue); //1
	vector3 point2( fValue, fValue, fValue); //2
	vector3 point3(-fValue, fValue, fValue); //3

	vector3 point4(-fValue,-fValue,-fValue); //4
	vector3 point5( fValue,-fValue,-fValue); //5
	vector3 point6( fValue, fValue,-fValue); //6
	vector3 point7(-fValue, fValue,-fValue); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCuboid(vector3 a_v3Dimensions, vector3 a_v3Color)
{
	Release();
	Init();

	vector3 v3Value = a_v3Dimensions * 0.5f;
	//3--2
	//|  |
	//0--1
	vector3 point0(-v3Value.x, -v3Value.y, v3Value.z); //0
	vector3 point1(v3Value.x, -v3Value.y, v3Value.z); //1
	vector3 point2(v3Value.x, v3Value.y, v3Value.z); //2
	vector3 point3(-v3Value.x, v3Value.y, v3Value.z); //3

	vector3 point4(-v3Value.x, -v3Value.y, -v3Value.z); //4
	vector3 point5(v3Value.x, -v3Value.y, -v3Value.z); //5
	vector3 point6(v3Value.x, v3Value.y, -v3Value.z); //6
	vector3 point7(-v3Value.x, v3Value.y, -v3Value.z); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCone(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// Replace this with your code
	vector3 topPoint(0, a_fHeight / 2.0f, 0);
	vector3 bottomPoint(0, a_fHeight / -2.0f, 0);
	float sliceRad = glm::radians(360 / static_cast<float>(a_nSubdivisions));
	std::vector<vector3> circlePoints;
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		circlePoints.push_back(vector3(a_fRadius * cos(sliceRad * i), a_fHeight / -2.0f, a_fRadius * sin(sliceRad * i)));
	}
	for (int i = 0; i < a_nSubdivisions - 1; i++)
	{
		AddTri(circlePoints[i], circlePoints[i+1], topPoint);
		AddTri(circlePoints[i+1], circlePoints[i], bottomPoint);
	}
	AddTri(circlePoints[a_nSubdivisions - 1], circlePoints[0], topPoint);
	AddTri(circlePoints[0], circlePoints[a_nSubdivisions - 1], bottomPoint);
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCylinder(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// Replace this with your code
	vector3 topPoint(0, a_fHeight / 2.0f, 0);
	vector3 bottomPoint(0, a_fHeight / -2.0f, 0);
	float sliceRad = glm::radians(360 / static_cast<float>(a_nSubdivisions));
	std::vector<vector3> topPoints;
	std::vector<vector3> bottomPoints;
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		topPoints.push_back(vector3(a_fRadius * cos(sliceRad * i), a_fHeight / 2.0f, a_fRadius * sin(sliceRad * i)));
		bottomPoints.push_back(vector3(a_fRadius * cos(sliceRad * i), a_fHeight / -2.0f, a_fRadius * sin(sliceRad * i)));
	}
	for (int i = 0; i < a_nSubdivisions - 1; i++)
	{
		AddTri(topPoints[i + 1], topPoints[i], topPoint);
		AddTri(bottomPoints[i], bottomPoints[i + 1], bottomPoint);
		AddQuad(bottomPoints[i + 1], bottomPoints[i], topPoints[i + 1], topPoints[i]);
	}
	AddTri(topPoints[0], topPoints[a_nSubdivisions - 1], topPoint);
	AddTri(bottomPoints[a_nSubdivisions - 1], bottomPoints[0], bottomPoint);
	AddQuad(bottomPoints[0], bottomPoints[a_nSubdivisions - 1], topPoints[0], topPoints[a_nSubdivisions - 1]);
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateTube(float a_fOuterRadius, float a_fInnerRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// Replace this with your code
	vector3 topPoint(0, a_fHeight / 2.0f, 0);
	vector3 bottomPoint(0, a_fHeight / -2.0f, 0);
	float sliceRad = glm::radians(360 / static_cast<float>(a_nSubdivisions));
	std::vector<vector3> topPoints;
	std::vector<vector3> bottomPoints;
	std::vector<vector3> topInnerPoints;
	std::vector<vector3> bottomInnerPoints;
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		topPoints.push_back(vector3(a_fOuterRadius * cos(sliceRad * i), a_fHeight / 2.0f, a_fOuterRadius * sin(sliceRad * i)));
		bottomPoints.push_back(vector3(a_fOuterRadius * cos(sliceRad * i), a_fHeight / -2.0f, a_fOuterRadius * sin(sliceRad * i)));
		topInnerPoints.push_back(vector3(a_fInnerRadius * cos(sliceRad * i), a_fHeight / 2.0f, a_fInnerRadius * sin(sliceRad * i)));
		bottomInnerPoints.push_back(vector3(a_fInnerRadius * cos(sliceRad * i), a_fHeight / -2.0f, a_fInnerRadius * sin(sliceRad * i)));
	}
	for (int i = 0; i < a_nSubdivisions - 1; i++)
	{
		AddQuad(topInnerPoints[i + 1], topPoints[i + 1], topInnerPoints[i], topPoints[i]);
		AddQuad(bottomInnerPoints[i], bottomPoints[i], bottomInnerPoints[i + 1], bottomPoints[i + 1]);
		AddQuad(bottomPoints[i + 1], bottomPoints[i], topPoints[i + 1], topPoints[i]);
		AddQuad(bottomInnerPoints[i + 1], topInnerPoints[i + 1], bottomInnerPoints[i], topInnerPoints[i]);
	}
	AddQuad(topInnerPoints[0], topPoints[0], topInnerPoints[a_nSubdivisions - 1], topPoints[a_nSubdivisions - 1]);
	AddQuad(bottomInnerPoints[a_nSubdivisions - 1], bottomPoints[a_nSubdivisions - 1], bottomInnerPoints[0], bottomPoints[0]);
	AddQuad(bottomPoints[0], bottomPoints[a_nSubdivisions - 1], topPoints[0], topPoints[a_nSubdivisions - 1]);
	AddQuad(bottomInnerPoints[0], topInnerPoints[0], bottomInnerPoints[a_nSubdivisions - 1], topInnerPoints[a_nSubdivisions - 1]);
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateTorus(float a_fOuterRadius, float a_fInnerRadius, int a_nSubdivisionsA, int a_nSubdivisionsB, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_nSubdivisionsA < 3)
		a_nSubdivisionsA = 3;
	if (a_nSubdivisionsA > 360)
		a_nSubdivisionsA = 360;

	if (a_nSubdivisionsB < 3)
		a_nSubdivisionsB = 3;
	if (a_nSubdivisionsB > 360)
		a_nSubdivisionsB = 360;

	Release();
	Init();

	// Replace this with your code
	float sliceRad = glm::radians(360 / static_cast<float>(a_nSubdivisionsA));
	float bendSliceRad = glm::radians(360 / static_cast<float>(a_nSubdivisionsB));
	std::vector<vector3> torusBends;
	std::vector<std::vector<vector3>> vertices;
	float radiusToCenter = (a_fOuterRadius + a_fInnerRadius) / 2.0f;
	float torusRadius = a_fOuterRadius - a_fInnerRadius;
	vector3 perpendicularVector = vector3(0, 1, 0);
	for (int i = 0; i < a_nSubdivisionsA; i++)
	{
		torusBends.push_back(vector3(radiusToCenter * cos(sliceRad * i), 0, radiusToCenter * sin(sliceRad * i)));
	}
	for (int i = 0; i < a_nSubdivisionsA; i++)
	{
		vector3 normalizedBendVector = glm::normalize(torusBends[i]);
		std::vector<vector3> bendPoints;
		for (int j = 0; j < a_nSubdivisionsB; j++)
		{
			float bendPointXPrimitive = torusRadius * cos(bendSliceRad * i);
			float bendPointYPrimitive = torusRadius * sin(bendSliceRad * i);
			vector3 bendPoint = torusBends[i] + (bendPointXPrimitive * normalizedBendVector) + (bendPointYPrimitive * perpendicularVector);
			bendPoints.push_back(bendPoint);
		}
		vertices.push_back(bendPoints);
	}
	


	for (int i = 0; i < a_nSubdivisionsA - 1; i++)
	{
		for (int j = 0; j < a_nSubdivisionsB - 1; j++)
		{
			AddQuad(vertices[i][j], vertices[i+1][j], vertices[i][j+1], vertices[i+1][j+1]);
		}
	}
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateSphere(float a_fRadius, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	//Sets minimum and maximum of subdivisions
	if (a_nSubdivisions < 1)
	{
		GenerateCube(a_fRadius * 2.0f, a_v3Color);
		return;
	}
	if (a_nSubdivisions > 6)
		a_nSubdivisions = 6;

	Release();
	Init();
/*
	std::vector<float> radi;
	std::vector<float> yvalues;
	std::vector<std::vector<vector3>> vertices;
	float radiancut = glm::radians(360 / static_cast<float>(a_nsubdivisions));
	for (int i = 0; i < a_nsubdivisions; i++)
	{
		yvalues.push_back(a_fradius * sin(270 + (i * 0.5f *radiancut)));
		radi.push_back(a_fradius * cos(270 + (i * 0.5f *radiancut)));
	}
	yvalues.push_back(a_fradius);
	radi.push_back(0);
	for (int i = 0; i < a_nsubdivisions + 1; i++)
	{
		std::vector<vector3> newring;
		for (int j = 0; i < a_nsubdivisions; j++)
		{
			newring.push_back(vector3(radi[i] * cos(radiancut * j), yvalues[i], radi[i] * sin(radiancut * j)));
		}
		vertices.push_back(newring);
	}
	for (int i = 0; i < a_nsubdivisions + 1; i++)
	{
		for (int j = 0; i < a_nsubdivisions-1; j++)
		{
			addquad(vertices[i][j], vertices[i][j + 1], vertices[i + 1][j], vertices[i + 1][j + 1]);
		}
	}
*/
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}