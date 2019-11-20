#pragma once
#include <vector>
#include <winsock.h>
#include <string>

#include "NetPacketException.h"	

namespace PowerSplitNet
{
	class NetPacket
	{
	public:
		void Clear();
		void Append(const void * data, uint32_t size);

		NetPacket& operator << (uint32_t data);
		NetPacket& operator >> (uint32_t & data);

		NetPacket& operator << (const std::string & data);
		NetPacket& operator >> (std::string & data);

		uint32_t extractionOffset = 0;
		std::vector<char> buffer;
	};
}