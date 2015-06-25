#include "GLApplication.h"

#include <string>
#include <iostream>
#include <vector>
#include <fstream>

#include "Gizmos.h"
#include "BaseCamera.h"
#include "GLFW\glfw3.h"
#include <tiny_obj_loader.h>

GLApplication::GLApplication(std::string a_appName, unsigned int a_screenWidth, unsigned int a_screenHeight)
{
	m_appName = a_appName;
	m_camera = nullptr;

	m_screenWidth = a_screenWidth;
	m_screenHeight = a_screenHeight;

	m_totalRunTime = glfwGetTime();
	m_deltaTime = 0.0f;
}

GLApplication::~GLApplication()
{
}

bool GLApplication::InitGL()
{
	if (glfwInit() == false)
	return false;

	m_window = glfwCreateWindow(m_screenWidth, m_screenHeight, m_appName.c_str(), nullptr, nullptr);

	if (m_window == nullptr)
	{
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(m_window);

	if (ogl_LoadFunctions() == ogl_LOAD_FAILED)
	{
		glfwDestroyWindow(m_window);
		glfwTerminate();
		return false;
	}
	
	Gizmos::create();

	glClearColor(0.25f, 0.25f, 0.25f, 1);
	glEnable(GL_DEPTH_TEST); //enables depth buffer, never forget this or weird shit will happen and the game will trip out
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	return true;
}

void GLApplication::Run()
{
	InitGL();
	InitApp();

	assert(m_camera != NULL && "Camera not initialized!");
	assert(m_camera->GetPerspectiveSet() == true && "Camera Perspective is not set");

	bool running = true;
	while (!glfwWindowShouldClose(m_window) && running)
	{
		double currentTime = glfwGetTime();
		m_deltaTime = currentTime - m_totalRunTime;
		m_totalRunTime = currentTime;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		Gizmos::clear();

		running = Update(m_deltaTime);
		Draw();

		Gizmos::draw(m_camera->GetProjectionView());

		glfwSwapBuffers(m_window);
		glfwPollEvents();
	}

	DeInitApp();

	Gizmos::destroy();

	glfwDestroyWindow(m_window);
	glfwTerminate();
}

void GLApplication::DisplayGrid(int a_size)
{
	assert(a_size > 0 && "Grid size must be a positive number");

	Gizmos::addTransform(glm::mat4(1));

	glm::vec4 white(1);
	glm::vec4 black(0,0,0,1);

	int halfSize = a_size/2;
	for (int i = 0; i < a_size + 1; ++i)
	{
		Gizmos::addLine(glm::vec3(-halfSize + i, 0, halfSize),
							glm::vec3(-halfSize + i, 0, -halfSize),
							i== halfSize ? white : black);
			Gizmos::addLine(glm::vec3(halfSize, 0, -halfSize + i),
							glm::vec3(-halfSize, 0, -halfSize + i),
							i == halfSize ? white : black);
	}
}
bool GLApplication::LoadObj(std::string a_fileName)
{
	std::vector<tinyobj::material_t> materials;
	a_fileName = "./res/models/" + a_fileName + ".obj";
	std::string err = tinyobj::LoadObj(shapes, materials, a_fileName.c_str());

	if (!err.empty())
	{
		std::cerr << err << std::endl;
		exit(1);
	}
	return true;
}
void GLApplication::CreateOpenGLBuffers(std::vector<tinyobj::shape_t>& shapes)
{
	m_gl_info.resize(shapes.size());

	for (unsigned int meshIndex = 0;
		meshIndex < shapes.size();
		++meshIndex)
	{
		glGenVertexArrays(1, &m_gl_info[meshIndex].m_VAO);
		glGenBuffers(1, &m_gl_info[meshIndex].m_VBO);
		glGenBuffers(1, &m_gl_info[meshIndex].m_IBO);
		glBindVertexArray(m_gl_info[meshIndex].m_VAO);

		unsigned int floatCount = shapes[meshIndex].mesh.positions.size();
		floatCount += shapes[meshIndex].mesh.normals.size();
		floatCount += shapes[meshIndex].mesh.texcoords.size();

		std::vector<float> vertexData;
		vertexData.reserve(floatCount);

		vertexData.insert(vertexData.end(), shapes[meshIndex].mesh.positions.begin(),
			shapes[meshIndex].mesh.positions.end());

		vertexData.insert(vertexData.end(), shapes[meshIndex].mesh.normals.begin(),
			shapes[meshIndex].mesh.normals.end());

		m_gl_info[meshIndex].m_index_count = shapes[meshIndex].mesh.indices.size();

		glBindBuffer(GL_ARRAY_BUFFER, m_gl_info[meshIndex].m_VBO);
		glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(),
			GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_gl_info[meshIndex].m_IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, shapes[meshIndex].mesh.indices.size() * sizeof(unsigned int),
			shapes[meshIndex].mesh.indices.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0); //position
		glEnableVertexAttribArray(1); //normal data

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 0,
			(void*)(sizeof(float)*shapes[meshIndex].mesh.positions.size()));

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	}
}
bool GLApplication::CreateGLLightShaders()
{
	const char* m_vsSource = "#version 410\n \
						   layout(location=0) in vec4 Position; \
						   layout(location=1) in vec4 Normal; \
						   out vec4 vNormal; \
						   out vec4 vPosition; \
						   uniform mat4 ProjectionView; \
						   void main() { \
						   vNormal = Normal; \
						   vPosition = Position; \
						   gl_Position= ProjectionView * Position; \
						   }";
						   
	const char* m_fsSource = "#version 410\n \
						   in vec4 vNormal; \
						   in vec4 vPosition; \
						   out vec4 FragColor; \
						   uniform vec3 LightDirection; \
						   uniform vec3 LightColor; \
						   uniform vec3 CameraPos; \
						   uniform float SpecPow; \
						   void main() { float d = max(0, dot(normalize(vNormal.xyz), LightDirection)); \
						   vec3 E = normalize( CameraPos - vPosition.xyz);\
						   vec3 R = reflect( -LightDirection, vNormal.xyz); \
						   float s = max( 0, dot(E, R));\
						   s = pow(s, SpecPow); \
						   FragColor = vec4(LightColor * d + LightColor * s, 1); \
						   }";

	
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER); 
	glShaderSource(vertexShader, 1, (const char**)&m_vsSource, 0);
	glCompileShader(vertexShader);

	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, (const char**)&m_fsSource, 0);
	glCompileShader(fragmentShader);

	m_program = glCreateProgram();
	glAttachShader(m_program, vertexShader);
	glAttachShader(m_program, fragmentShader);
	glLinkProgram(m_program);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	
	int loc = glGetUniformLocation(m_program, "ProjectionView");
	glUniformMatrix4fv(loc, 1, GL_FALSE, &(m_camera->GetProjectionView()[0][0]));

	//tell the shader where it is
	loc = glGetUniformLocation(m_program, "diffuse");
	glUniform1i(loc, 0);


	int shaderStatus = GL_FALSE;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &shaderStatus);

	if (shaderStatus == GL_FALSE)
	{
		int maxLength = 0;

		glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<char> errorLog(maxLength);
		glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &errorLog[0]);

		std::cerr << "Vertex Shader Error" << errorLog.data() << std::endl;
	}

	shaderStatus = GL_FALSE;
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &shaderStatus);
	
	if (shaderStatus == GL_FALSE)
	{
		int maxLength = 0;

		glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<char> errorLog(maxLength);
		glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &errorLog[0]);

		std::cerr << "Fragment Shader Error" << errorLog.data() << std::endl;
	}
	int shaderLinked = GL_FALSE;
	glGetProgramiv(m_program, GL_LINK_STATUS, &shaderStatus);
	
	if (shaderStatus == GL_FALSE)
	{
		int maxLength = 0;

		glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<char> errorLog(maxLength);
		glGetProgramInfoLog(m_program, maxLength, &maxLength, &errorLog[0]);

		std::cerr << "Program Linker Error" << errorLog.data() << std::endl;
	}
	return true;
}
const char** GLApplication::LoadShader(const char* a_FileName)
{
	std::ifstream shaderFile(a_FileName, std::ios::binary);
	std::string shaderText("");
	std::string* shaderData = new std::string("");
	if (!shaderFile.good())
	{
		std::cout << "shader file path not found" << std::endl;
		return nullptr;
	}
	while (std::getline(shaderFile, shaderText))
	{
		*shaderData += shaderText + '\n';
	}
	const char** shaderReference = new const char*;
	*shaderReference = shaderData->c_str();
	return shaderReference;
}

void GLApplication::CreateFBXBuffers(FBXFile* a_fbx)
{
	//Create the gl VAO/VBO/IBO data for each mesh
	for (unsigned int i = 0; i < a_fbx->getMeshCount(); ++i)
	{
		FBXMeshNode* mesh = a_fbx->getMeshByIndex(i);

		//storage for the gl data in 3 unsigned int
		unsigned int* glData = new unsigned int[3];

		glGenVertexArrays(1, &glData[0]);
		glBindVertexArray(glData[0]);

		glGenBuffers(1, &glData[1]);
		glGenBuffers(1, &glData[2]);

		glBindBuffer(GL_ARRAY_BUFFER, glData[1]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glData[2]);

		glBufferData(GL_ARRAY_BUFFER,
			mesh->m_vertices.size() * sizeof(FBXVertex),
			mesh->m_vertices.data(), GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			mesh->m_indices.size() * sizeof(unsigned int),
			mesh->m_indices.data(), GL_STATIC_DRAW);

		//position
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), 0);

		//normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(FBXVertex), ((char*)0) + FBXVertex::NormalOffset);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		mesh->m_userData = glData;
	}
}
void GLApplication::CleanUpFBXBuffers(FBXFile* a_fbx)
{
	//clean up the vertex data attached to each mesh
	for (unsigned int i = 0; i < a_fbx->getMeshCount(); ++i)
	{
		FBXMeshNode* mesh = a_fbx->getMeshByIndex(i);

		unsigned int* glData = (unsigned int*)mesh->m_userData;

		glDeleteVertexArrays(1, &glData[0]);
		glDeleteBuffers(1, &glData[1]);
		glDeleteBuffers(1, &glData[2]);

		delete[] glData;
	}
}

