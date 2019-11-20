#pragma once
#include "IPVersion.h"
#include <WS2tcpip.h>
#include <string>
#include <vector>

namespace PowerSplitNet
{
	class IPEndpoint
	{
	public:
		IPEndpoint(const char* ip, unsigned short port);
		IPEndpoint(sockaddr* addr);
		IPVersion GetIpVersion();
		std::string GetIpStr();
		std::vector<uint8_t> GetIpBytes();
		std::string GetHostname();
		unsigned short GetPort();
		sockaddr_in GetSockaddrIPv4();
		void PrintIPInfo();
	private:
		IPVersion ipVersion = IPVersion::Unknown;
		std::string ipStr = "";
		std::vector<uint8_t> ipBytes;
		std::string hostname = "";
		unsigned short port = 0;
	};
}