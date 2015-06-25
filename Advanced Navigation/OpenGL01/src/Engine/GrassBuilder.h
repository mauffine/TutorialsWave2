#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <vector>
#include <iostream>

#include <stb_image.h>
#include <Engine/gl_core_4_4.h>
struct Grass {
	glm::vec3 position;
	glm::vec2 texCoord;
};
struct GrassVertex
{
	glm::vec4 position;
	glm::vec2 texCoord;
};
class GrassBuilder
{
public:
	GrassBuilder(glm::vec3 a_position, unsigned int a_size);
	virtual ~GrassBuilder();

	void Update(float a_deltaTime, const glm::mat4& a_cameraTransform);
	void Draw(const glm::mat4& a_projectionView);
private:
	void CreateBuffers();
	void CreateDrawShader();
	
	Grass* grassPatch;
	GrassVertex* m_vertexData;

	unsigned int m_activeBuffer;
	unsigned int m_vao;
	unsigned int m_vbo;
	unsigned int m_ibo;

	unsigned int m_size;

	unsigned int m_drawShader;

	glm::vec3 m_position;

	unsigned int LoadShader(unsigned int a_type, const char* a_path);

	float m_lastDrawTime;

	unsigned int m_texture;
};