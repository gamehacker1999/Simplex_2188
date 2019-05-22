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

	vector3 point0(-fValue, -fValue, fValue); //0
	vector3 point1(fValue, -fValue, fValue); //1
	vector3 point2(fValue, fValue, fValue); //2
	vector3 point3(-fValue, fValue, fValue); //3

	vector3 point4(-fValue, -fValue, -fValue); //4
	vector3 point5(fValue, -fValue, -fValue); //5
	vector3 point6(fValue, fValue, -fValue); //6
	vector3 point7(-fValue, fValue, -fValue); //7

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
	//GenerateCube(a_fRadius * 2.0f, a_v3Color);

	//calculate all the angle values and the angle location of each vertex of the base
	float pos = 0;
	float angle = 360 / a_nSubdivisions;

	//vector to hold the points
	std::vector<vector3> points;

	//creating the base using sin and cosine of the angle times a positive integer value
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		vector3 point = vector3(cos(i * angle*PI / 180)*a_fRadius, -a_fHeight / 2, sin(i * angle*PI / 180)*a_fRadius);
		points.emplace_back(point);
	}

	//for all subdivision locations, add a try between two adjacent vertices of the base and the centers
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		if (i < a_nSubdivisions - 1)
		{
			AddTri(points[i], points[i + 1], vector3(0, -a_fHeight / 2, 0));
		}

		//if the loop is at the last position, then loop back to the first vertex
		else
		{
			AddTri(points[i], points[0], vector3(0, -a_fHeight / 2, 0));
		}
	}

	//for all subdivision locations, add a try between two adjacent vertices of the base and the tip of the cone
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		if (i < a_nSubdivisions - 1)
		{
			AddTri(points[i + 1], points[i], vector3(0, a_fHeight / 2, 0));
		}

		else
		{
			AddTri(points[0], points[i], vector3(0, a_fHeight / 2, 0));
		}
	}
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
	//GenerateCube(a_fRadius * 2.0f, a_v3Color);

	//calculate the angle of the subdivision location
	float pos = 0;
	float angle = 360 / a_nSubdivisions;

	//points of the pottom base
	std::vector<vector3> points;

	//creating a base by calculating the vertex of the circumfrence of the circle using sin and cosine
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		vector3 point = vector3(cos(i * angle*PI / 180)*a_fRadius, -a_fHeight / 2, sin(i * angle*PI / 180)*a_fRadius);
		points.emplace_back(point);
	}


	//creating the base by adding the tri from two adjacent vertices and the center location of base
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		if (i < a_nSubdivisions - 1)
		{
			AddTri(points[i], points[i + 1], vector3(0, -a_fHeight / 2, 0));
		}

		else
		{
			AddTri(points[i], points[0], vector3(0, -a_fHeight / 2, 0));
		}
	}

	//the entire aforementioned procedure is repeated for the top base
	//the only difference is the height
	std::vector<vector3> pointsUp;

	//calculating vertices of base
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		vector3 point = vector3(cos(i * angle*PI / 180)*a_fRadius, a_fHeight / 2, sin(i * angle*PI / 180)*a_fRadius);
		pointsUp.emplace_back(point);
	}

	//creating top base
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		if (i < a_nSubdivisions - 1)
		{
			AddTri(vector3(0, a_fHeight / 2, 0), pointsUp[i + 1], pointsUp[i]);
		}

		else
		{
			AddTri(vector3(0, a_fHeight / 2, 0), pointsUp[0], pointsUp[i]);
		}
	}

	//looping through all the vertices by the number of subdivisions
	//adding a quad using two adjacent vertices from the same base,
	//and two parallel vertices from the other base
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		if (i < a_nSubdivisions - 1)
		{
			AddQuad(points[i + 1], points[i], pointsUp[i + 1], pointsUp[i]);
		}

		else
		{
			AddQuad(points[0], points[i], pointsUp[0], pointsUp[i]);
		}
	}
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

	//calculating the radial position of each vertex in each subdivision
	float pos = 0;
	float angle = 360 / a_nSubdivisions;

	//the following code creates 4 bases as done with othe shapes
	//outer lower, inner lower, outer upper, and inner upper

	//lower outer points
	std::vector<vector3> points;

	for (int i = 0; i < a_nSubdivisions; i++)
	{
		vector3 point = vector3(cos(i * angle*PI / 180)*a_fOuterRadius, -a_fHeight / 2, sin(i * angle*PI / 180)*a_fOuterRadius);
		points.emplace_back(point);
	}

	//upper outer points
	std::vector<vector3> pointsUp;

	for (int i = 0; i < a_nSubdivisions; i++)
	{
		vector3 point = vector3(cos(i * angle*PI / 180)*a_fOuterRadius, a_fHeight / 2, sin(i * angle*PI / 180)*a_fOuterRadius);
		pointsUp.emplace_back(point);
	}

	//lower inner points
	std::vector<vector3> pointsInner;

	for (int i = 0; i < a_nSubdivisions; i++)
	{
		vector3 point = vector3(cos(i * angle*PI / 180)*a_fInnerRadius, -a_fHeight / 2, sin(i * angle*PI / 180)*a_fInnerRadius);
		pointsInner.emplace_back(point);
	}

	//upper inner points
	std::vector<vector3> pointsInnerUp;

	for (int i = 0; i < a_nSubdivisions; i++)
	{
		vector3 point = vector3(cos(i * angle*PI / 180)*a_fInnerRadius, a_fHeight / 2, sin(i * angle*PI / 180)*a_fInnerRadius);
		pointsInnerUp.emplace_back(point);
	}

	//outer face of the tube
	//creating the quads for the outer surface of the tube
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		if (i < a_nSubdivisions - 1)
		{
			AddQuad(points[i + 1], points[i], pointsUp[i + 1], pointsUp[i]);
		}

		else
		{
			AddQuad(points[0], points[i], pointsUp[0], pointsUp[i]);
		}
	}

	//inner face of the tube
	//creating the quad for the inner surface of tube
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		if (i < a_nSubdivisions - 1)
		{
			AddQuad(pointsInner[i], pointsInner[i + 1], pointsInnerUp[i], pointsInnerUp[i + 1]);
		}

		else
		{
			AddQuad(pointsInner[i], pointsInner[0], pointsInnerUp[i], pointsInnerUp[0]);
		}
	}

	//bottom surface of tube
	//creating the quad for the bottom of the cube by using vertices from the inner bottom
	//and their parallel vertices from outer bottom
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		if (i < a_nSubdivisions - 1)
		{
			AddQuad(points[i + 1], pointsInner[i + 1], points[i], pointsInner[i]);
		}

		else
		{
			AddQuad(points[0], pointsInner[0], points[i], pointsInner[i]);
		}
	}

	//top surface of tube
	//creating the quad for the bottom of the cube by using vertices from the inner top
	//and their parallel vertices from outer top
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		if (i < a_nSubdivisions - 1)
		{
			AddQuad(pointsUp[i], pointsInnerUp[i], pointsUp[i + 1], pointsInnerUp[i + 1]);
		}

		else
		{
			AddQuad(pointsUp[i], pointsInnerUp[i], pointsUp[0], pointsInnerUp[0]);
		}
	}

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
	//subdivision a is the subdivisions on the tube circle
	//subdivision b is the subdivisions on the whole torus

	//minor radius is the radius of the tube of the torus
	//major radius is the distance between the center of the torus and the center of the tube
	float minorRadius = (a_fOuterRadius - a_fInnerRadius) / 2;
	float majorRadius = minorRadius + a_fInnerRadius;

	//calculating the angle in the tube and torus by using their respective number of subdivisions
	float angleInTube = 2 * PI / a_nSubdivisionsA;
	float angleInTorus = 2 * PI / a_nSubdivisionsB;

	//vertices to store the 4 vertices of torus that will make a quad
	vector3 v1;
	vector3 v2;
	vector3 v3;
	vector3 v4;

	//looping garound the torus
	for (float i = angleInTorus; i <= 2 * PI + angleInTorus; i += angleInTorus)
	{
		for (float j = 0; j <= 2 * PI; j += angleInTube)
		{
			//calculating the vertex of the torus using toroidal coordinates
			//the formula is x = major+(minor*cosine of the angle in torus)*cosine of angle in tube
			//y = minor*sin of angle in torus
			//z = major+(minor*cosine of the angle in torus)*sin of angle in tube

			//calcuating 4 adjacent vertices to make a quad using this formula
			v1.x = (majorRadius + (minorRadius * cos(i)))*cos(j);
			v1.y = minorRadius * sin(i);
			v1.z = (majorRadius + minorRadius * cos(i))*sin(j);

			v2.x = (majorRadius + (minorRadius * cos(i)))*cos(j+angleInTube);
			v2.y = minorRadius * sin(i); 
			v2.z = (majorRadius + minorRadius * cos(i))*sin(j + angleInTube);

			v3.x = (majorRadius + (minorRadius * cos(i-angleInTorus)))*cos(j+angleInTube);
			v3.y = minorRadius * sin(i - angleInTorus);
			v3.z = (majorRadius + minorRadius * cos(i - angleInTorus))*sin(j + angleInTube);

			v4.x = (majorRadius + (minorRadius * cos(i-angleInTorus)))*cos(j);
			v4.y = minorRadius * sin(i - angleInTorus);
			v4.z = (majorRadius + minorRadius * cos(i - angleInTorus))*sin(j);

			//adding quads around the torus using these coordinated to make the torus
			AddQuad(v3, v4, v2, v1);

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
	if (a_nSubdivisions > 12)
		a_nSubdivisions = 12;

	Release();
	Init();

	// Replace this with your code

	//latitude and longitude is the number of subdivision accross and verically on the circle
	float latitude = a_nSubdivisions;
	float longitude = a_nSubdivisions;

	//looping through latitude and longitide to calculate the vertex
	//spherical coordinates formula, x = sin theta* cosine phi
	//y = cosine theta
	//z = sin theta* sin phi
	//where theta = PI*(current subdivision/number of latitudes)
	//phi = 2*PI*(current subdivision/number of longitude)
	for (int i = 0; i < latitude; i++)
	{
		//calculating adjacent theta values
		float theta1 = (PI*((float)i / (float)latitude));
		float theta2 = (PI*((float)(i + 1) / (float)latitude));
		for (int j = 0; j < longitude; j++)
		{
			//calculating adjacent phi values
			float phi1 = 2 * PI*((float)j / (float)longitude);
			float phi2 = 2 * PI*((float)(j + 1) / (float)longitude);

			//spherical coordinates of 4 adjacent points on the sphere
			vector3 v1 = vector3(sin(theta1)*cos(phi1), cos(theta1), sin(theta1)*sin(phi1))*a_fRadius;
			vector3 v2 = vector3(sin(theta1)*cos(phi2), cos(theta1), sin(theta1)*sin(phi2))*a_fRadius;
			vector3 v3 = vector3(sin(theta2)*cos(phi2), cos(theta2), sin(theta2)*sin(phi2))*a_fRadius;
			vector3 v4 = vector3(sin(theta2)*cos(phi1), cos(theta2), sin(theta2)*sin(phi1))*a_fRadius;

			//using the 4 points to create two tris around the sphere
			AddTri(v3, v4, v2);
			AddTri(v2, v4, v1);

		}
	}

	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}