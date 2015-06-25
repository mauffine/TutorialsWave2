#include "Engine/Shader.h"

Shader::Shader(){

}
Shader::~Shader(){

}
std::string Shader::LoadFile(const std::string &a_file, bool &a_status){
	std::ifstream file;

	file.open((a_file).c_str());

	std::string output;
	std::string line;

	if (file.is_open())
	{
		while (file.good())
		{
			getline(file, line);
			output.append(line + "\n");
		}
	}
	else
	{
		std::cerr << "Shader load error: " << a_file.c_str() << std::endl;
	}

	return output;
}

bool Shader::CheckErrors(const unsigned int &a_shader){

	int status = GL_FALSE;
	glGetShaderiv(a_shader, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE)
	{
		int maxLength = 0;
		glGetShaderiv(a_shader, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<char> errorLog(maxLength);
		glGetShaderInfoLog(a_shader, maxLength, &maxLength, &errorLog[0]);

		std::cerr << "Shader Compiler Error: " << errorLog.data() << std::endl;
		return true;
	}

	return false;
}

bool Shader::AddShader(const std::string &a_file, const ShaderType &a_type){
	// Load shader file
	bool loadFail = false;
	std::string fileString = LoadFile(a_file, loadFail);
	const char* file = fileString.c_str();
	if (loadFail)
		return false;// Load error

	// Create shader
	unsigned int shader = 0;
	switch (a_type)
	{
	case ShaderType::VERTEX:
		shader = glCreateShader(GL_VERTEX_SHADER);
		break;

	case ShaderType::FRAGMENT:
		shader = glCreateShader(GL_FRAGMENT_SHADER);
		break;

	}

	// Pass shader to GL and compile
	glShaderSource(shader, 1, &file, NULL);
	glCompileShader(shader);
	if (CheckErrors(shader))
	{
		glDeleteShader(shader); // Compilation error...
		return false;
	}

	// Link to program
	glAttachShader(m_program, shader);
}

void Shader::LinkProgram(){

	glLinkProgram(m_program);

	// Check for errors
	int isLinked = 0;
	glGetProgramiv(m_program, GL_LINK_STATUS, &isLinked);
	if (isLinked == GL_FALSE)
	{
		int maxLength = 0;
		glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<char> errorLog(maxLength);
		glGetProgramInfoLog(m_program, maxLength, &maxLength, &errorLog[0]);

		std::cerr << "Shader Program Link Failed: " << errorLog.data() << std::endl;

		// Recreate shader, to erase link errors
		glDeleteProgram(m_program);
		m_program = glCreateProgram();
	}
}

void Shader::Bind() const
{
	glUseProgram(m_program);
}

unsigned int Shader::GetUniform(const std::string &a_name) const
{
	return glGetUniformLocation(m_program, a_name.c_str());
}