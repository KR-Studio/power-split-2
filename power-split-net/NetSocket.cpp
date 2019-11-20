#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include "NetSocket.h"
#include "assert.h"
#include <iostream>

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

namespace PowerSplitNet
{
	NetSocket::NetSocket(IPVersion ipVersion, SocketHandler handler)
		:ipVersion(ipVersion), handler(handler)
	{
		assert(ipVersion == IPVersion::IPv4);
	}

	NetResult NetSocket::Create()
	{
		assert(ipVersion == IPVersion::IPv4);

		if (handler != INVALID_SOCKET)
		{
			int error = WSAGetLastError();
			return NetResult::Net_GenericError;
		}

		handler = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		if (handler == INVALID_SOCKET)
		{
			int error = WSAGetLastError();
			return NetResult::Net_GenericError;
		}

		if (SetSocketOption(SocketOption::TCP_NoDelay, TRUE) != NetResult::Net_Success)
		{
			return NetResult::Net_GenericError;
		}

		return NetResult::Net_Success;
	}

	NetResult NetSocket::Close()
	{
		// Close the socket to release the resources associated
		// Normally an application calls shutdown() before closesocket 
		// to disables sends or receives on a socket first

		if (handler == INVALID_SOCKET)
		{
			int error = WSAGetLastError();
			return NetResult::Net_GenericError;
		}

		int result = closesocket(handler);

		if (result != 0) // if error occurred while trying to close socket
		{
			int error = WSAGetLastError();
			return NetResult::Net_GenericError;
		}

		handler = INVALID_SOCKET;
		return NetResult::Net_Success;
	}

	NetResult NetSocket::Bind(IPEndpoint endpoint)
	{
		sockaddr_in addr = endpoint.GetSockaddrIPv4();
		int result = bind(handler, (sockaddr*)(&addr), sizeof(sockaddr_in));
		if (result != 0)
		{
			int error = WSAGetLastError();
			return NetResult::Net_GenericError;
		}

		return NetResult::Net_Success;
	}

	NetResult NetSocket::Listen(IPEndpoint endpoint, int backlog)
	{
		if (Bind(endpoint) != NetResult::Net_Success)
		{
			return NetResult::Net_GenericError;
		}

		int result = listen(handler, backlog); // backlog - the number of possible connections
		if (result != 0)
		{
			int error = WSAGetLastError();
			return NetResult::Net_GenericError;
		}
		return NetResult();
	}

	NetResult NetSocket::Accept(NetSocket& outsocket)
	{
		sockaddr_in addr = {};
		int length = sizeof(sockaddr_in);
		SocketHandler acceptedConnectionHandler = accept(handler, (sockaddr*)(&addr), &length);
		if (acceptedConnectionHandler == INVALID_SOCKET)
		{
			int error = WSAGetLastError();
			return NetResult::Net_GenericError;
		}
		IPEndpoint newConnectionEndpoint((sockaddr*)(&addr));
		newConnectionEndpoint.PrintIPInfo();
		outsocket = NetSocket(IPVersion::IPv4, acceptedConnectionHandler);
		return NetResult::Net_Success;
	}

	NetResult NetSocket::Receive(void * dataDestination, int numberOfBytes, int & bytesRecieved)
	{
		bytesRecieved = recv(handler, (char *)dataDestination, numberOfBytes, 0);

		if (bytesRecieved == 0) // connection closed
		{
			return NetResult::Net_GenericError;
		}

		if (bytesRecieved == SOCKET_ERROR)
		{
			int error = WSAGetLastError();
			return NetResult::Net_GenericError;
		}

		return NetResult::Net_Success;
	}

	NetResult NetSocket::ReceiveAll(void * dataDestination, int numberOfBytes)
	{
		int totalBytesRecieved = 0;

		while (totalBytesRecieved < numberOfBytes)
		{
			int bytesRemaining = numberOfBytes - totalBytesRecieved;
			int bytesRecieved = 0;
			char* bufferOffset = (char*)dataDestination + totalBytesRecieved;
			NetResult result = Receive(bufferOffset, bytesRemaining, bytesRecieved);
			if (result != NetResult::Net_Success)
			{
				return NetResult::Net_GenericError;
			}
			totalBytesRecieved += bytesRecieved;
		}
		return NetResult::Net_Success;
	}

	NetResult NetSocket::Connect(IPEndpoint endpoint)
	{
		sockaddr_in addr = endpoint.GetSockaddrIPv4();
		int result = connect(handler, (sockaddr*)(&addr), sizeof(sockaddr_in));
		if (result != 0)
		{
			int error = WSAGetLastError();
			return NetResult::Net_GenericError;
		}
		return NetResult::Net_Success;
	}

	NetResult NetSocket::Send(const void * dataSource, int numberOfBytes, int & bytesSent)
	{
		bytesSent = send(handler, (const char*)dataSource, numberOfBytes, 0);
		if (bytesSent == SOCKET_ERROR)
		{
			int error = WSAGetLastError();
			return NetResult::Net_GenericError;
		}
		return NetResult::Net_Success;
	}

	NetResult NetSocket::SendAll(const void * dataSource, int numberOfBytes)
	{
		int totalBytesSent = 0;

		while (totalBytesSent < numberOfBytes)
		{
			int bytesRemaining = numberOfBytes - totalBytesSent;
			int bytesSent = 0;
			char* bufferOffset = (char*)dataSource + totalBytesSent;
			NetResult result = Send(bufferOffset, bytesRemaining, bytesSent);
			if (result != NetResult::Net_Success)
			{
				return NetResult::Net_GenericError;
			}
			totalBytesSent += bytesSent;
		}
		return NetResult::Net_Success;
	}

	SocketHandler NetSocket::GetHandler()
	{
		return handler;
	}

	IPVersion NetSocket::GetIPVersion()
	{
		return ipVersion;
	}

	NetResult NetSocket::Send(NetPacket& packet)
	{
		uint32_t encodedPacketSize = htonl(packet.buffer.size());
		NetResult result = SendAll(&encodedPacketSize, sizeof(uint32_t));
		if (result != NetResult::Net_Success)
			return NetResult::Net_GenericError;

		result = SendAll(packet.buffer.data(), packet.buffer.size());
		if (result != NetResult::Net_Success)
			return NetResult::Net_GenericError;

		return NetResult::Net_Success;
	}

	NetResult NetSocket::Receive(NetPacket& packet)
	{
		packet.Clear();

		uint32_t encodedSize = 0;
		NetResult result = ReceiveAll(&encodedSize, sizeof(uint32_t));
		if (result != NetResult::Net_Success)
			return NetResult::Net_GenericError;

		uint32_t bufferSize = ntohl(encodedSize);

		if (bufferSize > PowerSplitNet::MAX_PACKETSIZE)
			return NetResult::Net_GenericError;

		packet.buffer.resize(bufferSize);
		result = ReceiveAll(&packet.buffer[0], bufferSize);
		if (result != NetResult::Net_Success)
			return NetResult::Net_GenericError;

		return NetResult::Net_Success;
	}

	NetResult NetSocket::SetSocketOption(SocketOption option, BOOL value)
	{
		int result = 0;
		switch (option)
		{
			case SocketOption::TCP_NoDelay:
			{
				result = setsockopt(handler, IPPROTO_TCP, TCP_NODELAY, (const char*)&value, sizeof(value));
				break;
			}
			default:
			{
				return NetResult::Net_GenericError;
			}
		}

		if (result != 0)
		{
			int error = WSAGetLastError();
			return NetResult::Net_GenericError;
		}

		return NetResult::Net_Success;
	}
}