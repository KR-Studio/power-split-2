#pragma once

#define WIN32_LEAN_AND_MEAN

#include <WinSock2.h>
#include <string>

//// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
//#pragma comment (lib, "Ws2_32.lib")
//#pragma comment (lib, "Mswsock.lib")
//#pragma comment (lib, "AdvApi32.lib")

namespace PowerSplitNet
{
	class Network
	{
	public:
		static bool Initialize();
		static void Shutdown();

	private:

	};

	
}