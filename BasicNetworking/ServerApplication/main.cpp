#include <iostream>
#include <string>
#include <thread>

#include "RakPeerInterface.h"
#include "MessageIdentifiers.h"
#include "Bitstream.h"
#include "GameMessages.h"

int main()
{
	const unsigned short PORT = 5456;
	RakNet::RakPeerInterface* pPeerInterface = nullptr;

	//Start the server and start listening to clients
	std::cout << "Starting Server..." << std::endl;

	//Initialize the RakNet peer interface first
	pPeerInterface = RakNet::RakPeerInterface::GetInstance();

	//Create a socket descriptor to describe the connection
	RakNet::SocketDescriptor sd(PORT, 0);

	//Now call startup - max of 32 connections, on the assigned port
	pPeerInterface->Startup(32, &sd, 1);
	pPeerInterface->SetMaximumIncomingConnections(32);

	std::cout << "Server Started." << std::endl;

	RakNet::Packet* packet = nullptr;

	std::thread pingThread(SendClientPing, pPeerInterface);
	while (1)
	{
		for (packet = pPeerInterface->Receive(); packet; pPeerInterface->DeallocatePacket(packet),
			packet = pPeerInterface->Receive())
		{
			switch (packet->data[0])
			{
			case ID_NEW_INCOMING_CONNECTION:
				std::cout << "Client Connected.\n";
				break;
			case ID_DISCONNECTION_NOTIFICATION:
				std::cout << "Client disconnected(leaving).\n";
				break;
			case ID_CONNECTION_LOST:
				std::cout << "Client disconnected(timed out).\n";
				break;
			default:
				std::cout << "Recieved a message with an unknown ID: " << packet->data[0];
				break;
			}
		}
	}
}