#pragma once

#include <fstream>
#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <Engine/gl_core_4_4.h>

#include <stb_image.h>

#include "Engine/FlyCamera.h"
#include "Engine/DirectionalLight.h"
#include "Engine/GLApplication.h"
#include "Engine/DirectionalLight.h"
#include "Engine/Shader.h"


class TerrainGen

{
public:
	TerrainGen(const unsigned int& a_size, DirectionalLight* a_pDirLight);
	~TerrainGen();

	bool Update(double dt);
	void Draw(const FlyCamera& a_camera);

	void GenerateEnvironment();
private:
	struct Vertex
	{
		glm::vec4 position;
		glm::vec3 normal;
		glm::vec2 texCoord;
	};
	Vertex* m_vertexData;
	unsigned int* m_indicies;

	unsigned int m_size;
	unsigned int m_vao, m_vbo, m_ibo;

	float m_seed;
	

	void GeneratePlane();
	void GenerateBuffers();

	unsigned int m_diffuseID;
	int m_diffuseHeight, m_diffuseWidth, m_imageType;

	Shader m_shaders;

	DirectionalLight* m_pDirLight;

};