#include "TerrianGenerator.h"
#include "gl_core_4_4.h"
#include <glm/ext.hpp>
#include <fstream>
#include <iostream>
#include "STB libs\stb_image.h"
#include "Gizmos.h"

float TerrianGenerator::Amplitude = 0;
float TerrianGenerator::Frequancy = 0;
float TerrianGenerator::Seed = 0;

unsigned int TerrianGenerator::colNum = 1;
unsigned int TerrianGenerator::rowNum = 1;
unsigned int TerrianGenerator::offSet = 4;

glm::vec3 TerrianGenerator::LightDirection;
glm::vec3 TerrianGenerator::Ambient;
float TerrianGenerator::SpecPow = 1.00f;

TerrianGenerator::TerrianGenerator(unsigned int a_Row, unsigned int a_Col, unsigned int a_scale = 2)
{
	
	LoadTexture("Misc Textures/Grass.png");
	Regenerate(a_Row,a_Col,a_scale);
}

void TerrianGenerator::Regenerate()
{
	Regenerate(rowNum, colNum, offSet);
}

void TerrianGenerator::Regenerate(unsigned int a_Row, unsigned int a_Col, unsigned int a_scale)
{
	offSet = a_scale;
	rowNum = a_Row;
	colNum = a_Col;
	totalNum = a_Row * a_Col;
	if (beenCalled)
	{
		plainVerts.clear();
		indexVector.clear();
		Gizmos::clear();
		glDeleteShader(shaderProgram);
	}
	else
	{
		Amplitude = 1.20f;
		Frequancy = 0.9f;
		Seed = 6.28318530718f;
	}
	plainVerts.reserve(totalNum);
	//indexTotal = (totalNum - (a_Row + a_Col)) * 6;
	indexTotal = ((a_Row - 1) * (a_Col - 1)) * 6;
	indexVector.resize(indexTotal);
	perlin_data = nullptr;
	bindShaders();
	GeneratePlain();
	heightMap = GenerateHeightMap();
	BuildNormalMap();
	beenCalled = true;

}

void TerrianGenerator::GeneratePlain()
{
	for (int I = 0; I < rowNum; ++I)// LOOP
		for (int J = 0; J < colNum; ++J)
		{
			Vertex plainVert;
			plainVerts.push_back(plainVert);
		}
	glGenVertexArrays(1,&vertArray);
	glGenBuffers(1,&vertBuffer);
	glGenBuffers(1,&indexBuffer);
	glBindVertexArray(vertArray);
	//CREATE INDEX DATA FUNCTION
	CreateIndexData();
	glBindBuffer(GL_ARRAY_BUFFER,vertBuffer);
	glBufferData(GL_ARRAY_BUFFER,plainVerts.size() * sizeof(Vertex),plainVerts.data(),GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexTotal * sizeof(unsigned int), indexVector.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0); //position
	glEnableVertexAttribArray(1); //UV Data
	glEnableVertexAttribArray(2); //Normals

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(glm::vec4));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex), (void*)(sizeof(glm::vec4) + sizeof(glm::vec2)));
	
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

bool TerrianGenerator::bindShaders()
{
	const char* vsSource = LoadShader("Shaders/Terrain Gen.vert");
	const char* fsSource = LoadShader("Shaders/Terrain Gen.frag");
	//Set Vertex shader
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, (const char**)& vsSource, 0);
	glCompileShader(vertexShader);
	//Set Fragment shader
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, (const char**)&fsSource, 0);
	glCompileShader(fragmentShader);
	//Create a program to compile and link shaders
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	
	//Shader Tester
	int success = false;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (success == GL_FALSE)
	{
		int infoLogLength = 0;
		glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];
		glGetProgramInfoLog(shaderProgram, infoLogLength, 0, infoLog);
		printf("Error: Failed to link shader program!\n");
		printf("%s\n", infoLog);
		delete[] infoLog;
		delete vsSource, fsSource;
		return false;
	}
	delete vsSource, fsSource;
	return true;
}

void TerrianGenerator::PerlinNoise()
{
}

void TerrianGenerator::Draw(const glm::mat4 a_projView, glm::vec3 a_Pos)
{
	glUseProgram(shaderProgram);
	//TODO: DO NOT bind every frame
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, heightMap);
	//Pass uniforms
	int loc = glGetUniformLocation(shaderProgram, "ProjectionView");
	glm::mat4 projView = a_projView;
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(projView));

	//loc = glGetUniformLocation(shaderProgram, "perlin_texture");
	//glUniform1i(loc, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D,grassMap);
	loc = glGetUniformLocation(shaderProgram, "terrianTexture");
	glUniform1i(loc,1);

	loc = glGetUniformLocation(shaderProgram, "LightDir");
	glUniform3f(loc, LightDirection.x, LightDirection.y, LightDirection.z);

	loc = glGetUniformLocation(shaderProgram, "CameraPos");
	glUniform3f(loc, a_Pos.x, a_Pos.y, a_Pos.z);

	loc = glGetUniformLocation(shaderProgram, "SpecPow");
	glUniform1f(loc, SpecPow);

	loc = glGetUniformLocation(shaderProgram, "Ambient");
	glUniform3f(loc, Ambient.x, Ambient.y, Ambient.z);

	glBindVertexArray(vertArray);
	glDrawElements(GL_TRIANGLES, indexTotal, GL_UNSIGNED_INT, 0);
}

int TerrianGenerator::GenerateHeightMap()
{
	perlin_data = new float[totalNum];
	float scale = (1.0f / colNum)* 3.0f;
	int octaves = 14;

	float amplitude = Amplitude, persistance = Frequancy;
	for (int x = 0; x < rowNum; ++x)//LOOP
		for (int y = 0; y < colNum; ++y)
		{
			amplitude = Amplitude, persistance = Frequancy;
			perlin_data[y * rowNum + x] = 0;
			//REMEMBER AMPLITUDE MUST BE RESET HERE
			for (int I = 0; I < octaves; ++I)
			{
				float freq = powf(2,(float)I);
				float sample = glm::perlin(glm::vec3((float)x, Seed, (float)y) * scale * freq) * 0.5f;// +0.5f;
				perlin_data[y * rowNum + x] += sample * amplitude;
				amplitude *= persistance;
			}
		}
	/*
	unsigned int textureMap;
	glGenTextures(1, &textureMap);
	glBindTexture(GL_TEXTURE_2D, textureMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, rowNum, colNum, 0, GL_RED, GL_FLOAT, perlin_data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	return textureMap; 	*/
	//TODO: Change this to another texture
	for (unsigned int i = 0; i < totalNum; ++i)
		plainVerts[i].Position.y = perlin_data[i] * 5;

	return -1;
}

void TerrianGenerator::CreateIndexData()
{
	int halfRow = rowNum / 2;
	int halfCol = colNum / 2;
	int quatRow = halfRow / 2;
	int quatCol = halfCol / 2;
	for (int X_iter = 0; X_iter < rowNum; ++X_iter)//LOOP
		for (int Y_iter = 0; Y_iter < colNum; ++Y_iter)
		{
			// Position
			glm::vec4 position((float)(X_iter - halfRow), 0, (float)(Y_iter - halfCol), 1);

			// Texture coordinates
			glm::vec2 texCoord((float)X_iter, (float)Y_iter);
			//glm::vec2 texCoord((float)X_iter, (float)Y_iter);

			plainVerts[X_iter * colNum + Y_iter].Position = position * offSet;
			plainVerts[X_iter * colNum + Y_iter].UV = texCoord;
		}

	unsigned int index = 0;
	for (unsigned int X_iter = 0; X_iter < (rowNum - 1); ++X_iter)//LOOP
		for (unsigned int Y_iter = 0; Y_iter < (colNum - 1); ++Y_iter)
		{
			// Triangle 1
			int currVert = X_iter * colNum + Y_iter;
			indexVector[index++] = currVert;
			indexVector[index++] = currVert + colNum;
			indexVector[index++] = currVert + colNum + 1;

			// Triangle 2
			indexVector[index++] = currVert;
			indexVector[index++] = currVert + colNum + 1;
			indexVector[index++] = currVert + 1;
		}
}

TerrianGenerator::~TerrianGenerator()
{
	if (plainVerts.size() > 0)
		plainVerts.clear();
	if (indexVector.size() > 0)
		indexVector.clear();
}

bool TerrianGenerator::LoadTexture(const char* a_filePath)
{
	int imageWidth = 0, imageHeight = 0, imageFormat = 0;
	unsigned int textureInfo = 0;
	unsigned char* data = stbi_load(a_filePath, &imageWidth, &imageHeight, &imageFormat, STBI_default);
	if (data == nullptr)
		return false;
	glGenTextures(1, &textureInfo);
	glBindTexture(GL_TEXTURE_2D, textureInfo);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	stbi_image_free(data);
	grassMap = textureInfo;
	return true;
}

void TerrianGenerator::BuildNormalMap()
{
	// Generate new normals
	for (unsigned int i = 0; i < indexTotal; i += 3)
	{
		Vertex* vertex3 = &plainVerts[indexVector[i + 2]];
		Vertex* vertex2 = &plainVerts[indexVector[i + 1]];
		Vertex* vertex1 = &plainVerts[indexVector[i]];

		//Calculate face normal
		glm::vec3 d1(vertex3->Position - vertex1->Position);
		glm::vec3 d2(vertex2->Position - vertex1->Position);
		glm::vec3 crossProduct = glm::cross(d1, d2);
		//Normalise normals
		glm::vec3 normal = glm::normalize(crossProduct);
		vertex1->Normals = normal;
		vertex2->Normals = normal;
		vertex3->Normals = normal;
	}
	//Send to GPU
	glBindBuffer(GL_ARRAY_BUFFER, vertBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, totalNum * sizeof(Vertex), plainVerts.data());
}

const char* TerrianGenerator::LoadShader(const char* a_FileName)
{
	std::ifstream shaderFile(a_FileName, std::ios::binary);
	std::string shaderText("");
	std::string* shaderData = new std::string("");
	if (!shaderFile.good())
	{
		std::cout << "shader file path not found" << std::endl;
		return nullptr;
	}
	while (std::getline(shaderFile, shaderText))//LOOP
		*shaderData += shaderText + '\n';

	const char*  shaderReference = shaderData->c_str();
	return shaderReference;
}