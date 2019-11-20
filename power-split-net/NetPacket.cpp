#include "NetPacket.h"
#include "Constants.h"
#include <iostream>

namespace PowerSplitNet
{
	void NetPacket::Clear()
	{
		buffer.clear();
		extractionOffset = 0;
	}

	void NetPacket::Append(const void* data, uint32_t size)
	{
		if (buffer.size() + size > PowerSplitNet::MAX_PACKETSIZE)
			throw NetPacketException("[NetPacket::Append(const void*, uint32_t)] - Packet size ecxeeded max packet size.");

		buffer.insert(buffer.end(), (char *)data, (char*)data + size);
	}

	NetPacket& NetPacket::operator<<(uint32_t data)
	{
		data = htonl(data);
		Append(&data, sizeof(uint32_t));

		return *this;
	}

	NetPacket& NetPacket::operator>>(uint32_t& data)
	{
		if (extractionOffset + sizeof(uint32_t) > buffer.size())
			throw NetPacketException("[NetPacket::operator>>(uint32_t&)] - Packet size ecxeeded max packet size.");

		data = *reinterpret_cast<uint32_t*>(&buffer[extractionOffset]);
		data = ntohl(data);
		extractionOffset += sizeof(uint32_t);

		return *this;
	}

	NetPacket& NetPacket::operator<<(const std::string& data)
	{
		*this << (uint32_t)data.size();
		Append(data.data(), data.size());

		return *this;
	}

	NetPacket& NetPacket::operator>>(std::string& data)
	{
		data.clear();

		uint32_t stringSize = 0;
		*this >> stringSize;

		if (extractionOffset + (uint32_t)stringSize > buffer.size())
			throw NetPacketException("[NetPacket::operator>>(std::string&)] - Packet size ecxeeded max packet size.");

		data.resize(stringSize);
		data.assign(&buffer[extractionOffset], stringSize);
		extractionOffset += stringSize;

		return *this;
	}
}

