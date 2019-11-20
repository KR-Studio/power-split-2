#include "Network.h"

namespace PowerSplitNet
{
	bool Network::Initialize()
	{
		WORD wVersionRequested;
		WSADATA wsaData;

		/* Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h */
		wVersionRequested = MAKEWORD(2, 2);

		//// Initialize Winsock
		int error = WSAStartup(wVersionRequested, &wsaData);
		if (error != 0) {
			/* Tell the user that we could not find a usable */
			/* Winsock DLL.                                  */
			std::string dataStr = "WSAStartup failed with error: " + std::to_string(error) + "\r\n";
			OutputDebugString(dataStr.c_str());
			return false;
		}
		else {
			std::string dataStr = "WSAStartup starts okay\r\n";
			OutputDebugString(dataStr.c_str());
		}

		/* Confirm that the WinSock DLL supports 2.2.			*/
		/* Note that if the DLL supports versions greater than	*/
		/* 2.2 in addition to 2.2, it will still return	2.2		*/
		/* in wVersion since that is the version we requested.	*/

		if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
			/* Tell the user that we could not find a usable */
			/* WinSock DLL.                                  */
			std::string dataStr = "Could not find a usable version of Winsock.dll\r\n";
			OutputDebugString(dataStr.c_str());
			WSACleanup();
			return false;
		}
		else {
			std::string dataStr = "The Winsock 2.2 dll was found okay\r\n";
			OutputDebugString(dataStr.c_str());
		}

		return true;
	}

	void Network::Shutdown()
	{
		WSACleanup();
	}
}