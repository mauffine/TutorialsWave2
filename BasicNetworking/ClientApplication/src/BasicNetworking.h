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
};