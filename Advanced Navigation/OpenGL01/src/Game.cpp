#include "Game.h"

bool Game::InitApp()
{
	//Set up Camera
	FlyCamera* pCamera = new FlyCamera();

	pCamera->SetInputWindow(m_window);

	pCamera->SetupPerspective(glm::pi<float>() * 0.25f, 16.0f / 9.0f);
	pCamera->LookAt(glm::vec3(10, 10, 10), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

	m_pCamera = pCamera;

	m_pSponza = new FBXFile();
	m_pSponza->load("./res/models/SponzaSimple.fbx", FBXFile::UNITS_CENTIMETER);

	m_pNavMap = new FBXFile();
	m_pNavMap->load("./res/models/SponzaSimpleNavMesh.fbx", FBXFile::UNITS_CENTIMETER);

	return true;
}
void Game::DeInitApp()
{

}
bool Game::Update(double dt)
{
	m_camera->Update(dt);
	return true;
}
void Game::Draw()
{
	DisplayGrid(100);
}
void Game::ConnectNodes(std::vector<NavNode> a_vGraph)
{
	// connect edges
	for (auto& first : a_vGraph)
	{
		for (auto& second : a_vGraph)		{
			//don't link to self
			if (&first == &second)
			continue;

			//ABC XYZ
			if (A == X && B == Y) || (A == Y && B == Z) || (A == Z && B == X) ||
				(A == Y && B == X) || (A == Z && B == Y) || (A == X && B == Z)				{

				}
		}
	}
}