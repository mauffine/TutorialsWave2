#pragma once
#include <vector>
#include <glm/glm.hpp>

class TerrianGenerator
{
public:
	//Row = X, Col = Y
	TerrianGenerator(unsigned int a_row, unsigned int a_col, unsigned int a_scale);
	//Doctor who noises
	void Regenerate(unsigned int a_row, unsigned int a_col, unsigned int a_scale = 2);
	void Regenerate();
	~TerrianGenerator();
	static unsigned int rowNum, colNum, offSet;
	static float Amplitude, Frequancy, Seed, SpecPow;
	static glm::vec3 LightDirection, Ambient;
	void Draw(const glm::mat4, glm::vec3 a_Pos);
private:
	struct Vertex
	{
		glm::vec4 Position;
		glm::vec2 UV;
		glm::vec3 Normals;
	};
	void BuildNormalMap();
	int shaderProgram = -1, heightMap = 0,grassMap = 0;
	std::vector<Vertex> plainVerts;
	std::vector<unsigned int> indexVector;
	unsigned int totalNum = 0, indexTotal = 0;
	unsigned int vertBuffer = 0, indexBuffer = 0, vertArray = 0;
	const char* LoadShader(const char *);
	float* perlin_data;
	bool bindShaders();
	int GenerateHeightMap();
	bool LoadTexture(const char* a_filePath);
	void CreateIndexData(), GeneratePlain(), PerlinNoise();
	bool beenCalled = false;

};

