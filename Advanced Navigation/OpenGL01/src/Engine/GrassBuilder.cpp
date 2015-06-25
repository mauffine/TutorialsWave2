#include "Engine/GrassBuilder.h"

GrassBuilder::GrassBuilder(glm::vec3 a_position, unsigned int a_size)
{
	m_size = a_size;
	m_position = a_position;
	grassPatch = new Grass[m_size * 4];

	CreateDrawShader();
	CreateBuffers();
}
GrassBuilder::~GrassBuilder(){}

unsigned int GrassBuilder::LoadShader(unsigned int a_type, const char* a_path)
{
	FILE* file = fopen(a_path, "rb");
	if (file == nullptr)
		return 0;

	//read the shader source
	fseek(file, 0, SEEK_END);
	unsigned int length = ftell(file);
	fseek(file, 0, SEEK_SET);
	char* source = new char[length + 1];
	memset(source, 0, length + 1);
	fread(source, sizeof(char), length, file);
	fclose(file);

	unsigned int shader = glCreateShader(a_type);
	glShaderSource(shader, 1, &source, 0);
	glCompileShader(shader);
	delete[] source;

	return shader;
}

void GrassBuilder::CreateBuffers(){


	unsigned int* indexData = new unsigned int[m_size * 6];
	for (unsigned int i = 0; i < m_size; ++i) {
		indexData[i * 6 + 0] = i * 4 + 0;
		indexData[i * 6 + 1] = i * 4 + 1;
		indexData[i * 6 + 2] = i * 4 + 2;
		indexData[i * 6 + 3] = i * 4 + 0;
		indexData[i * 6 + 4] = i * 4 + 2;
		indexData[i * 6 + 5] = i * 4 + 3;
	}
	//Create OpenGL buffers
	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);
	glGenBuffers(1, &m_ibo);

	//setup the buffers
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, m_size * sizeof(Grass), grassPatch, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_size * sizeof(Grass), indexData, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);//position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,	sizeof(float) * 5, 0);

	glEnableVertexAttribArray(1); //Texture UV
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, ((char*)0) + 16);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	delete[] indexData;

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture);

	int loc = glGetUniformLocation(m_drawShader, "diffuse");
	glUniform1i(loc, 0);
}
void GrassBuilder::CreateDrawShader(){
	//load grass Texture
	int imageWidth = 0, imageHeight = 0, imageFormat = 0;

	unsigned char* data = stbi_load("./res/textures/Grass and shit.png", &imageWidth, &imageHeight, &imageFormat, STBI_default);

	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(data);

	unsigned int vs = LoadShader(GL_VERTEX_SHADER, "./res/shaders/Grass.vert");
	unsigned int fs = LoadShader(GL_FRAGMENT_SHADER, "./res/shaders/Grass.frag");

	glCompileShader(vs);
	glCompileShader(fs);

	m_drawShader = glCreateProgram();
	glAttachShader(m_drawShader, vs);
	glAttachShader(m_drawShader, fs);
	glLinkProgram(m_drawShader);

	// remove unneeded handles
	glDeleteShader(vs);
	glDeleteShader(fs);

	int shaderStatus = GL_FALSE;
		glGetShaderiv(fs, GL_COMPILE_STATUS, &shaderStatus);
	
		if (shaderStatus == GL_FALSE)
		{
			int maxLength = 0;
	
			glGetShaderiv(vs, GL_INFO_LOG_LENGTH, &maxLength);
	
			std::vector<char> errorLog(maxLength);
			glGetShaderInfoLog(vs, maxLength, &maxLength, &errorLog[0]);
	
			std::cerr << "Vertex Shader Error" << errorLog.data() << std::endl;
		}
	
		shaderStatus = GL_FALSE;
		glGetShaderiv(fs, GL_COMPILE_STATUS, &shaderStatus);
		
		if (shaderStatus == GL_FALSE)
		{
			int maxLength = 0;
	
			glGetShaderiv(fs, GL_INFO_LOG_LENGTH, &maxLength);
	
			std::vector<char> errorLog(maxLength);
			glGetShaderInfoLog(fs, maxLength, &maxLength, &errorLog[0]);
	
			std::cerr << "Vertex Shader Error" << errorLog.data() << std::endl;
		}
		shaderStatus = GL_FALSE;
		int shaderLinked = GL_FALSE;
		glGetProgramiv(m_drawShader, GL_LINK_STATUS, &shaderStatus);
		
		if (shaderStatus == GL_FALSE)
		{
			int maxLength = 0;
	
			glGetProgramiv(m_drawShader, GL_INFO_LOG_LENGTH, &maxLength);
	
			std::vector<char> errorLog(maxLength);
			glGetProgramInfoLog(m_drawShader, maxLength, &maxLength, &errorLog[0]);
	
			std::cerr << "Program Linker Error" << errorLog.data() << std::endl;
		}
}
void GrassBuilder::Draw(const glm::mat4& a_projectionView){

	// draw the particle
	glUseProgram(m_drawShader);

	int loc = glGetUniformLocation(m_drawShader, "projectionView");
	glUniformMatrix4fv(loc, 1, false, &a_projectionView[0][0]);

	loc = glGetUniformLocation(m_drawShader, "GrassID");
	glUniform1i(loc, 1);

	// draw particles in the buffer
	glBindVertexArray(m_vao);

	glDrawArrays(GL_POINTS, 0, m_size);

}
void GrassBuilder::Update(float a_Update, const glm::mat4& a_cameraTransform)
{
	using glm::vec3;
	using glm::vec4;
	Grass* grass = nullptr;
	unsigned int quad = 0;
	for (unsigned int i = 0; i < m_size; ++i)
	{

		float halfSize = 0.5f;
		m_vertexData[quad * 4 + 0].position = vec4(halfSize,
			halfSize, 0, 1);
		m_vertexData[quad * 4 + 0].texCoord = grass->texCoord;
		m_vertexData[quad * 4 + 1].position = vec4(-halfSize,
			halfSize, 0, 1);
		m_vertexData[quad * 4 + 1].texCoord = grass->texCoord;
		m_vertexData[quad * 4 + 2].position = glm::vec4(-halfSize,
			-halfSize, 0, 1);
		m_vertexData[quad * 4 + 2].texCoord = grass->texCoord;
		m_vertexData[quad * 4 + 3].position = vec4(halfSize,
			-halfSize, 0, 1);
		m_vertexData[quad * 4 + 3].texCoord = grass->texCoord;
		// create billboard transform
		vec3 zAxis = glm::normalize(vec3(a_cameraTransform[3]) -
			grass->position);
		vec3 xAxis = glm::cross(vec3(a_cameraTransform[1]), zAxis);
		vec3 yAxis = glm::cross(zAxis, xAxis);
		glm::mat4 billboard(vec4(xAxis, 0),
			vec4(yAxis, 0),
			vec4(zAxis, 0),
			vec4(0, 0, 0, 1));

		m_vertexData[quad * 4 + 0].position = billboard *
			m_vertexData[quad * 4 + 0].position +
			vec4(grass->position, 0);
		m_vertexData[quad * 4 + 1].position = billboard *
			m_vertexData[quad * 4 + 1].position +
			vec4(grass->position, 0);
		m_vertexData[quad * 4 + 2].position = billboard *
			m_vertexData[quad * 4 + 2].position +
			vec4(grass->position, 0);
		m_vertexData[quad * 4 + 3].position = billboard *
			m_vertexData[quad * 4 + 3].position +
			vec4(grass->position, 0);
		++quad;
	}
}