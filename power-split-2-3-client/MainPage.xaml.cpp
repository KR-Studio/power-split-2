//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "MainPage.xaml.h"
#include <string>
#include <sstream>

#include <power-split-net/Including.h>

using namespace PowerSplitClient;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

using namespace PowerSplitNet;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

MainPage::MainPage()
{
	InitializeComponent();
}

std::wstring s2ws(const std::string& outputDataStr)
{
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, &outputDataStr[0], (int)outputDataStr.size(), NULL, 0);
	std::wstring dataWstr(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, &outputDataStr[0], (int)outputDataStr.size(), &dataWstr[0], size_needed);
	return dataWstr;
}


String^ s2ps(const std::string& outputDataStr)
{
	std::wstring dataWstr = s2ws(outputDataStr);
	String^ dataPstr = ref new String(dataWstr.c_str());
	return dataPstr;
}


std::string ws2s(const std::wstring& dataWstr)
{
	auto dataWide = dataWstr.c_str();
	int bufferSize = WideCharToMultiByte(CP_UTF8, 0, dataWide, -1, nullptr, 0, NULL, NULL);
	auto dataUtf8 = std::make_unique<char[]>(bufferSize);
	if (0 == WideCharToMultiByte(CP_UTF8, 0, dataWide, -1, dataUtf8.get(), bufferSize, NULL, NULL))
		throw std::exception("Can't convert string to UTF8");
	return std::string(dataUtf8.get());
}


std::string ps2s(String^& dataPstr)
{
	std::wstring dataPstrWstr(dataPstr->Data());
	auto outputDataStr = ws2s(dataPstrWstr);
	return outputDataStr;
}


int ps2i(String^& dataPstr)
{
	std::wstring dataPstrWstr(dataPstr->Data());
	auto dataInt = std::stoi(dataPstrWstr);
	return dataInt;
}


double ps2d(String^& dataPstr)
{
	std::wstring dataPstrWstr(dataPstr->Data());
	auto dataDouble = std::stod(dataPstrWstr);
	return dataDouble;
}


std::wstring i2ws(const int& dataInt)
{
	std::wstring dataWstr = std::to_wstring(dataInt);
	return dataWstr;
}


String^ i2ps(const int& dataInt)
{
	String^ dataPstr = ref new String(i2ws(dataInt).c_str());
	return dataPstr;
}


// Main client Socket
NetSocket socketListener;
// Check connection with server
NetResult ifConnected = NetResult::Net_NotYetImplemented;
// Check number of messages
int numberOfMessage = 0;


void PowerSplitClient::MainPage::PageLoaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	// Clearing server output area
	textBlockInfoOutput->Text = "";
	// Clearing computing output area
	textBlockComputingOutput->Text = "";
}


void PowerSplitClient::MainPage::RandomButtonClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{

}


void PowerSplitClient::MainPage::ConnectButtonClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	// Check active connection with server
	if (ifConnected == NetResult::Net_Success)
	{
		std::string dataStr = "Please first disconnect from server\r\n";
		textBlockInfoOutput->Text += s2ps(dataStr);
	}
	else
	{
		if (Network::Initialize()) {
			std::string outputDataStr = "WinSock API successfully initialized\r\n";
			textBlockInfoOutput->Text += s2ps(outputDataStr);

			if (socketListener.Create() == NetResult::Net_Success)
			{
				std::string outputDataStr = "Socket successfully created\r\n";
				textBlockInfoOutput->Text += s2ps(outputDataStr);

				String^ ipPstr = hostTextBox->Text;
				std::string ipStr = ps2s(ipPstr);
				const char* ipChar = ipStr.c_str();

				String^ portPstr = portTextBox->Text;
				int portInt = ps2i(portPstr);

				if (socketListener.Connect(IPEndpoint(ipChar, portInt)) == NetResult::Net_Success)
				{
					ifConnected = NetResult::Net_Success;

					std::string outputDataStr = "Succesfully connected to server!\r\n";
					textBlockInfoOutput->Text += s2ps(outputDataStr);

					// Prepare sending data to check connection
					uint32_t messageNumberToSend, messageDataSizeToSend;
					std::string messageDataToSend;
					messageDataToSend = "Hello, Server!";
					messageNumberToSend = ++numberOfMessage;
					messageDataSizeToSend = messageDataToSend.size();
					NetPacket packetToSend;
					packetToSend << messageNumberToSend << messageDataSizeToSend << messageDataToSend;

					// Send the prepared packet with data
					NetResult resultSent = NetResult::Net_Success;
					while (resultSent == NetResult::Net_Success)
					{
						std::string outputDataStr = "Attempting to send set of data...\r\n";
						textBlockInfoOutput->Text += s2ps(outputDataStr);

						resultSent = socketListener.Send(packetToSend);
						if (resultSent != NetResult::Net_Success)
							break;

						outputDataStr = "Data has been sent to server\r\n";
						textBlockInfoOutput->Text += s2ps(outputDataStr);
						break;
					}

					// Wait for response data from server
					uint32_t messageNumberReceived, messageDataSizeReceived;
					std::string messageDataReceived;
					NetPacket packetReceived;
					NetResult resultReceived = NetResult::Net_Success;
					while (resultReceived == NetResult::Net_Success)
					{
						resultReceived = socketListener.Receive(packetReceived);
						if (resultReceived != NetResult::Net_Success)
							break;

						std::string outputDataStr = "Data received:\r\n";
						textBlockInfoOutput->Text += s2ps(outputDataStr);
						try
						{
							packetReceived >> messageNumberReceived >> messageDataSizeReceived >> messageDataReceived;
						}
						catch (NetPacketException& exception)
						{
							std::string outputDataStr = exception.ToString() + "\r\n";
							textBlockInfoOutput->Text += s2ps(outputDataStr);
						}
						outputDataStr = std::to_string(messageNumberReceived) + " " + std::to_string(messageDataSizeReceived) + " " + messageDataReceived + "\r\n";
						textBlockInfoOutput->Text += s2ps(outputDataStr);
						break;
					}
					outputDataStr = "Data has just been received from the server\r\n";
					textBlockInfoOutput->Text += s2ps(outputDataStr);
				}
				else
				{
					std::string outputDataStr = "Failed to connect to server!\r\n";
					textBlockInfoOutput->Text += s2ps(outputDataStr);

					socketListener.Close();

					ifConnected = NetResult::Net_NotYetImplemented;

					outputDataStr = "Socket successfully closed\r\n";
					textBlockInfoOutput->Text += s2ps(outputDataStr);
				}
			}
			else
			{
				std::string outputDataStr = "Socket failed to create\r\n";
				textBlockInfoOutput->Text += s2ps(outputDataStr);

				socketListener.Close();

				ifConnected = NetResult::Net_NotYetImplemented;

				outputDataStr = "Socket successfully closed\r\n";
				textBlockInfoOutput->Text += s2ps(outputDataStr);
			}
		}
		else {
			std::string outputDataStr = "WSAStartup failed with error\r\n";
			textBlockInfoOutput->Text += s2ps(outputDataStr);

			Network::Shutdown();

			outputDataStr = "WinSock API successfully closed\r\n";
			textBlockInfoOutput->Text += s2ps(outputDataStr);
		}
	}
}


void PowerSplitClient::MainPage::DisconnectButtonClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	// Check active connection with server
	if (ifConnected == NetResult::Net_NotYetImplemented)
	{
		std::string outputDataStr = "Please first connect to server\r\n";
		textBlockInfoOutput->Text += s2ps(outputDataStr);
	}
	else
	{
		socketListener.Close();

		ifConnected = NetResult::Net_NotYetImplemented;

		std::string outputDataStr = "Socket successfully closed\r\n";
		textBlockInfoOutput->Text += s2ps(outputDataStr);

		Network::Shutdown();

		outputDataStr = "WinSock API successfully closed\r\n";
		textBlockInfoOutput->Text += s2ps(outputDataStr);
	}
}


void PowerSplitClient::MainPage::SendButtonClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	// Check active connection with server
	if (ifConnected == NetResult::Net_NotYetImplemented)
	{
		std::string dataStr = "Please first connect to server\r\n";
		textBlockInfoOutput->Text += s2ps(dataStr);
	}
	else
	{
		// Prepare sending data
		uint32_t rectangleHeight, rectangleWidth;
		uint32_t messageNumberToSend, messageDataSizeToSend;
		std::string messageDataToSend;
		rectangleHeight = 150;
		rectangleWidth = 60;
		messageDataToSend = "Brown";
		messageNumberToSend = ++numberOfMessage;
		messageDataSizeToSend = messageDataToSend.size();
		NetPacket packetToSend;
		packetToSend << messageNumberToSend << messageDataSizeToSend << messageDataToSend << rectangleHeight << rectangleWidth;

		// Send the prepared packet with data
		NetResult resultSent = NetResult::Net_Success;
		while (resultSent == NetResult::Net_Success)
		{
			std::string outputDataStr = "Attempting to send set of data...\r\n";
			textBlockInfoOutput->Text += s2ps(outputDataStr);

			resultSent = socketListener.Send(packetToSend);
			if (resultSent != NetResult::Net_Success)
				break;

			outputDataStr = "Data has been sent to server\r\n";
			textBlockInfoOutput->Text += s2ps(outputDataStr);
			break;
		}

		// Wait for response data from server
		uint32_t messageNumberReceived, messageDataSizeReceived;
		uint32_t rectangleSquare, rectanglesSquare;
		std::string rectangleSquareStr, rectanglesSquareStr;
		std::string messageDataReceived;
		NetPacket packetReceived;
		NetResult resultReceived = NetResult::Net_Success;
		while (resultReceived == NetResult::Net_Success)
		{
			resultReceived = socketListener.Receive(packetReceived);
			if (resultReceived != NetResult::Net_Success)
				break;

			std::string outputDataStr = "Data received:\r\n";
			textBlockComputingOutput->Text += s2ps(outputDataStr);
			try
			{
				packetReceived >> messageNumberReceived >> messageDataSizeReceived >> messageDataReceived >> rectangleSquare >> rectanglesSquare;
			}
			catch (NetPacketException & exception)
			{
				std::string outputDataStr = exception.ToString() + "\r\n";
				textBlockComputingOutput->Text += s2ps(outputDataStr);
			}

			rectangleSquareStr = std::to_string(rectangleSquare);
			rectanglesSquareStr = std::to_string(rectanglesSquare);

			/*taskControl1Square->Text = s2ps(rectangleSquareStr);
			generalSquareValue->Text = s2ps(rectanglesSquareStr);*/

			outputDataStr = std::to_string(messageNumberReceived) + " " + std::to_string(messageDataSizeReceived) + " " + rectangleSquareStr + " " + rectanglesSquareStr + " " + messageDataReceived + "\r\n";
			textBlockComputingOutput->Text += s2ps(outputDataStr);
			break;
		}
		std::string outputDataStr = "Data has just been received from the server\r\n";
		textBlockInfoOutput->Text += s2ps(outputDataStr);
	}
}


void PowerSplitClient::MainPage::SizeButtonClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{

}
