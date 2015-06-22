#pragma once

#include "gl_core_4_4.h"
#include "BitStream.h"

#include "BaseApplication.h"
#include "Camera.h"
#include "Gizmos.h"

namespace RakNet
{
	class RakPeerInterface;
}

class BasicNetworkingApplication : public BaseApplication
{
public:
	BasicNetworkingApplication();
	~BasicNetworkingApplication();

	virtual bool startup();

	virtual void shutdown();

	virtual bool update(float deltaTime);

	virtual void draw();

private:
	//initialise the connection
	void HandleNetworkConnection();
	void InitialiseClientConnection();

	//Handle incoming messages
	void HandleNetworkMessages();

	void readObjectDataFromServer(RakNet::BitStream& bsIn);
	void createGameObject();

	RakNet::RakPeerInterface* m_pPeerInterface;

	const char* IP = "192.168.0.2";
	const unsigned short PORT = 5456;

	unsigned int m_uiClientId;
	unsigned int m_uiclientObjectIndex;

	Camera* m_pCamera;

	glm::vec3 m_myColour;
	std::vector<GameObject> m_gameObjects;
};