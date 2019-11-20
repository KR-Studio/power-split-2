#include "IPEndpoint.h"
#include "assert.h"
#include <iostream>

namespace PowerSplitNet
{
	IPEndpoint::IPEndpoint(const char* ip, unsigned short port)
	{
		this->port = port;
		in_addr addr; // location to store IPv4 address

		int result = inet_pton(AF_INET, ip, &addr); // presentation format to network format
		if (result == 1)
		{
			if (addr.S_un.S_addr != INADDR_NONE)
			{
				ipStr = ip;
				ipVersion = IPVersion::IPv4;
				ipBytes.resize(sizeof(ULONG));
				memcpy(&ipBytes[0], &addr.S_un.S_addr, sizeof(ULONG));
				hostname = ip;
				return;
			}
		}

		// Attempt to resolve hostname to IPv4 address
		addrinfo hints = {}; // hints will filter the results we get back for getaddrinfo()
		hints.ai_family = AF_INET; // IPv4 addresses only
		addrinfo* hostinfo = nullptr;

		result = getaddrinfo(ip, NULL, &hints, &hostinfo);
		if (result == 0)
		{
			sockaddr_in* hostaddr = reinterpret_cast<sockaddr_in*> (hostinfo->ai_addr);

			//host_addr->sin_addr.S_un.S_addr
			ipStr.resize(16);
			inet_ntop(AF_INET, &hostaddr->sin_addr, &ipStr[0], sizeof(16));

			hostname = ip;

			ULONG ipLong = hostaddr->sin_addr.S_un.S_addr;
			ipBytes.resize(sizeof(ULONG));
			memcpy(&ipBytes[0], &ipLong, sizeof(ULONG));

			ipVersion = IPVersion::IPv4;

			freeaddrinfo(hostinfo); // memory clean up from getaddrinfo()
			return;
		}
	}

	IPEndpoint::IPEndpoint(sockaddr* addr)
	{
		assert(addr->sa_family == AF_INET);
		sockaddr_in* addrv4 = reinterpret_cast<sockaddr_in*>(addr);
		ipVersion = IPVersion::IPv4;
		port = ntohs(addrv4->sin_port);
		ipBytes.resize(sizeof(ULONG));
		memcpy(&ipBytes[0], &addrv4->sin_addr, sizeof(ULONG));
		ipStr.resize(16);
		inet_ntop(AF_INET, &addrv4->sin_addr, &ipStr[0], 16);
		hostname = ipStr;
	}

	IPVersion IPEndpoint::GetIpVersion()
	{
		return ipVersion;
	}

	std::string IPEndpoint::GetIpStr()
	{
		return ipStr;
	}

	std::vector<uint8_t> IPEndpoint::GetIpBytes()
	{
		return ipBytes;
	}

	std::string IPEndpoint::GetHostname()
	{
		return hostname;
	}

	unsigned short IPEndpoint::GetPort()
	{
		return port;
	}

	sockaddr_in IPEndpoint::GetSockaddrIPv4()
	{
		assert(ipVersion == IPVersion::IPv4);

		sockaddr_in addr = {};
		addr.sin_family = AF_INET;
		memcpy(&addr.sin_addr, &ipBytes[0], sizeof(ULONG));
		addr.sin_port = htons(port); // host to network by short

		return addr;
	}

	void IPEndpoint::PrintIPInfo()
	{
		std::string dataStr = "New connection::";
		std::cout << dataStr << std::endl;
		switch (GetIpVersion())
		{
			case IPVersion::IPv4:
			{
				dataStr = "IP Version: IPv4";
				std::cout << dataStr << std::endl;
				break;
			}
			case IPVersion::IPv6:
			{
				dataStr = "IP Version: IPv6";
				std::cout << dataStr << std::endl;
				break;
			}
			default:
			{
				dataStr = "Error occurs when trying to get access to Unknown address protocol";
				std::cout << dataStr << std::endl;
			}
		}
		dataStr = "Hostname: " + GetHostname();
		std::cout << dataStr << std::endl;
		dataStr = "IP: " + GetIpStr();
		std::cout << dataStr << std::endl;
		dataStr = "Port: " + std::to_string(GetPort());
		std::cout << dataStr << std::endl;
		dataStr = "Bytes... ";
		for (auto& digit : GetIpBytes())
		{
			dataStr += std::to_string((int)digit) + " ";
		}
		std::cout << dataStr << std::endl;

		
	}
}