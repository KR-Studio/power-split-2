#pragma once

#include "IPEndpoint.h"
#include "IPVersion.h"
#include "NetResult.h"
#include "SocketOption.h"
#include "SocketHandler.h"
#include "Constants.h"
#include "NetPacket.h"

namespace PowerSplitNet
{
	class NetSocket
	{
	public:
		NetSocket(IPVersion ipVersion = IPVersion::IPv4, SocketHandler handler = INVALID_SOCKET);
		NetResult Create();
		NetResult Close();
		NetResult Bind(IPEndpoint endpoint);
		NetResult Listen(IPEndpoint endpoint, int backlog = 5);
		NetResult Accept(NetSocket& outsocket);
		NetResult Receive(void * dataDestination, int numberOfBytes, int & bytesRecieved);
		NetResult ReceiveAll(void * dataDestination, int numberOfBytes);
		NetResult Connect(IPEndpoint endpoint);
		NetResult Send(const void * dataSource, int numberOfBytes, int & bytesSent);
		NetResult SendAll(const void * dataSource, int numberOfBytes);
		SocketHandler GetHandler();
		IPVersion GetIPVersion();
		NetResult Send(NetPacket & packet);
		NetResult Receive(NetPacket& packet);
	private:
		NetResult SetSocketOption(SocketOption option, BOOL value);
		IPVersion ipVersion = IPVersion::IPv4;
		SocketHandler handler = INVALID_SOCKET;
	};
}