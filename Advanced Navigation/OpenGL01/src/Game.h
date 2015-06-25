#include <iostream>

#include <Gizmos.h>

#include <Engine/GLApplication.h>
#include <Engine/FlyCamera.h>
#include <Engine/TerrainGen.h>
#include <Engine/DirectionalLight.h>
struct NavNode
{
	glm::vec3 position;
	glm::vec3 verticies[3];
	NavNode* edgeTargets[3];

	//values usable by A*
	unsigned int flags;
	float edgeCost;
};
class Game : public GLApplication
{
private:
	FlyCamera* m_pCamera;

	FBXFile* m_pSponza;
	FBXFile* m_pNavMap;

	std::vector<NavNode> m_vGraph;
	void ConnectNodes(std::vector<NavNode> a_pGraph);
protected:
public:
	Game() : GLApplication("Virtual World") {};
	bool InitApp();
	void DeInitApp();
	bool Update(double dt);
	void Draw();

};