#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "gl_core_4_4.h"

#include <fstream>
#include <iostream>
#include <vector>
enum ShaderType
{
	VERTEX,
	FRAGMENT
};
class Shader
{
private:
	unsigned int m_program;

	std::string LoadFile(const std::string &a_filePath, bool &a_status);
	bool CheckErrors(const unsigned int &a_shader);
public:
	Shader();
	~Shader();

	bool AddShader(const std::string &a_file, const ShaderType &a_type);
	void LinkProgram();

	void Bind() const;
	unsigned int GetUniform(const std::string &a_name) const;
};