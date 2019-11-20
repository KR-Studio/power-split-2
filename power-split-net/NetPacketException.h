#pragma once
#include <string>
#include "Constants.h"

namespace PowerSplitNet
{
	class NetPacketException
	{
	public:
		NetPacketException(std::string exception)
			:exception(exception) {}
		const char* CStr()
		{
			return exception.c_str();
		}
		std::string ToString()
		{
			return exception;
		}
	private:
		std::string exception;
	};
}