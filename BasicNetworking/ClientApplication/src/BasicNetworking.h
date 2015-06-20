#pragma once

#include "BaseApplication.h"

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

	RakNet::RakPeerInterface* m_pPeerInterface;

	const char* IP = "192.168.0.2";
	const unsigned short PORT = 5456;
};