#include "Texture2D.h"

#include <Engine/gl_core_4_4.h>

#include <stb_image.h>
Texture2D::Texture2D()
{}
Texture2D::~Texture2D()
{}
void Texture2D::LoadTexture(std::string a_file)
{
	unsigned char* data = stbi_load(a_file.c_str(), &m_imageWidth, &m_imageHeight, &m_imageType, STBI_default);

	glGenTextures(1, &m_ID);
	glBindTexture(GL_TEXTURE_2D, m_ID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_imageWidth, m_imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(data);
}
void Texture2D::UseTexture()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_ID);
}