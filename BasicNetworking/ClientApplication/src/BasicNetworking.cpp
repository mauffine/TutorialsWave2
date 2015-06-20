#include "BasicNetworking.h"

#include <iostream>
#include <string>

#include "RakPeerInterface.h"
#include "MessageIdentifiers.h"
#include "BitStream.h"


BasicNetworkingApplication::BasicNetworkingApplication()
{

}

BasicNetworkingApplication::~BasicNetworkingApplication()
{

}

bool BasicNetworkingApplication::startup()
{
	//Setup the basic window
	createWindow("Client Application", 1280, 720);

	return true;
}

void BasicNetworkingApplication::shutdown()
{

}

bool BasicNetworkingApplication::update(float deltaTime)
{

	return true;
}

void BasicNetworkingApplication::draw()
{

}