#ifndef _GL_APPLICATION_H_
#define _GL_APPLICATION_H_

#include "gl_core_4_4.h"
#include "GLFW/glfw3.h"

#include "glm/glm.hpp"
#include "glm/ext.hpp"

#include <FBXFile.h>
#include <vector>
#include <tiny_obj_loader.h>
struct OpenGLInfo
{
unsigned int m_VAO;
unsigned int m_VBO;
unsigned int m_IBO;
unsigned int m_index_count;
};
class BaseCamera;

class GLApplication
{
private:
	//thou shalt not call base constructor
	GLApplication();

	//initialize OpenGL
	bool InitGL();

	unsigned int m_screenWidth;
	unsigned int m_screenHeight;

	double m_totalRunTime;
	double m_deltaTime;

protected:
	//Helper functions to be used by derived classes
	void DisplayGrid(int a_size);

	bool CreateGLLightShaders();
	const char** LoadShader(const char* a_FileName);

	bool LoadObj(std::string a_fileName);
	
	unsigned int GetScreenWidth() const {return m_screenWidth;}
	unsigned int GetScreenHeight() const {return m_screenHeight;}
	unsigned int m_program;

	std::string m_appName;
	GLFWwindow* m_window;
	
	BaseCamera* m_camera;
	//shape vector
	std::vector<tinyobj::shape_t> shapes;

	std::vector<OpenGLInfo> m_gl_info;
public:
	GLApplication(std::string a_appName, unsigned int a_screenWidth = 1280, unsigned int a_screenheight = 720);
	~GLApplication();

	virtual bool InitApp() = 0;
	virtual void DeInitApp() = 0;

	virtual bool Update(double dt) = 0;
	virtual void Draw() = 0;

	void Run();
	void CreateOpenGLBuffers(std::vector<tinyobj::shape_t>& shapes);
	void CreateFBXBuffers(FBXFile* a_fbx);
	void CleanUpFBXBuffers(FBXFile* a_fbx);

}; 


#endif