// power-split-6-server-cli.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <string>
#include <iostream>
#include <sstream>
#include <math.h>  
#include <cmath>
#include <power-split-net/Including.h>
#include <list>
#include <iterator>
#include <regex>
#include <future>

using namespace PowerSplitNet;

//std::vector<std::thread> threads;
std::vector<std::thread> threads2;
std::mutex mtx;
// Check number of messages
int numberOfMessage = 0;
// Save rectangles square data
uint32_t rectangle1Square = 0, rectangle2Square = 0, rectangle3Square = 0, rectangle4Square = 0;
uint32_t rectanglesSquare = 0;

void thread_rect_square(std::string color, uint32_t height, uint32_t width) {
	std::lock_guard<std::mutex> guard(mtx);
	int squareNum = width * height;
	if (color == "Brown")
	{
		rectangle1Square = squareNum;
	}
	else if (color == "Black")
	{
		rectangle2Square = squareNum;
	}
	else if (color == "Blue")
	{
		rectangle3Square = squareNum;
	}
	else if (color == "Green")
	{
		rectangle4Square = squareNum;
	}
	std::this_thread::sleep_for(std::chrono::seconds(2));
}

int main()
{
	if (Network::Initialize()) {
		std::string outputDataStr = "WinSock API successfully initialized";
		std::cout << outputDataStr << std::endl;

		NetSocket socketListener;
		if (socketListener.Create() == NetResult::Net_Success)
		{
			std::string outputDataStr = "Socket successfully created";
			std::cout << outputDataStr << std::endl;

			if (socketListener.Listen(IPEndpoint("0.0.0.0", 80)) == NetResult::Net_Success) // 0.0.0.0 means that access can be obtained form any IP
			{
				std::string outputDataStr = "Socket successfully listening on port 80";
				std::cout << outputDataStr << std::endl;

				NetSocket socketConnected;
				if (socketListener.Accept(socketConnected) == NetResult::Net_Success)
				{
					std::string outputDataStr = "New connection accepted";
					std::cout << outputDataStr << std::endl;

					uint32_t messageNumberReceived, messageDataSizeReceived;
					uint32_t rectangleHeight, rectangleWidth;
					std::string messageDataReceived;
					NetPacket packetReceived;

					uint32_t messageNumberToSend, messageDataSizeToSend;
					uint32_t rectangleSquare = 0;
					std::string messageDataToSend;
					NetPacket packetToSend;

					std::vector<std::thread> threads;
					
					NetResult resultReceived = NetResult::Net_Success;
					while (resultReceived == NetResult::Net_Success)
					{
						resultReceived = socketConnected.Receive(packetReceived);
						if (resultReceived != NetResult::Net_Success)
							break;
						else
						{
							std::cout << "Data received: ";
							try
							{
								packetReceived >> messageNumberReceived >> messageDataSizeReceived >> messageDataReceived;
							}
							catch (NetPacketException& exception)
							{
								std::cout << exception.CStr() << std::endl;
							}

							if (messageNumberReceived <= 1) {

								std::string outputDataStr = std::to_string(messageNumberReceived) + " " + std::to_string(messageDataSizeReceived) + " " + messageDataReceived;
								std::cout << outputDataStr << std::endl;

								messageDataToSend = "Hello, Client!";
								messageNumberToSend = ++numberOfMessage;
								messageDataSizeToSend = messageDataToSend.size();
								packetToSend << messageNumberToSend << messageDataSizeToSend << messageDataToSend;
							}
							else {
								try
								{
									packetReceived >> rectangleHeight >> rectangleWidth;
								}
								catch (NetPacketException& exception)
								{
									std::cout << exception.CStr() << std::endl;
								}

								std::string outputDataStr = std::to_string(messageNumberReceived) + " " + std::to_string(messageDataSizeReceived) + " " + std::to_string(rectangleHeight) + " " + std::to_string(rectangleWidth) + " " + messageDataReceived;
								std::cout << outputDataStr << std::endl;

								if (messageDataReceived == "Brown")
								{
									if (rectangle1Square == 0)
									{
										messageDataToSend = messageDataReceived + " " + "Rectangle sized";
										std::thread thr(thread_rect_square, messageDataReceived, rectangleHeight, rectangleWidth);
										threads.emplace_back(std::move(thr));

										for (auto& thread : threads) {
											thread.join();
										}
										rectangleSquare = rectangle1Square;
										rectanglesSquare += rectangle1Square;

										std::string outputDataStr = messageDataToSend + " - " + std::to_string(rectangle1Square) + " px";
										std::cout << outputDataStr << std::endl;
									}
									else
									{
										messageDataToSend = messageDataReceived + " " + "Rectangle already sized";
										rectangleSquare = rectangle1Square;

										std::string outputDataStr = messageDataToSend + " - " + std::to_string(rectangle1Square) + " px";
										std::cout << outputDataStr << std::endl;
									}
								}
								else if (messageDataReceived == "Black")
								{
									if (rectangle2Square == 0)
									{
										messageDataToSend = messageDataReceived + " " + "Rectangle sized";
										std::thread thr(thread_rect_square, messageDataReceived, rectangleHeight, rectangleWidth);
										threads.emplace_back(std::move(thr));

										for (auto& thread : threads) {
											thread.join();
										}
										rectangleSquare = rectangle2Square;
										rectanglesSquare += rectangle2Square;

										std::string outputDataStr = messageDataToSend + " - " + std::to_string(rectangle2Square) + " px";
										std::cout << outputDataStr << std::endl;
									}
									else
									{
										messageDataToSend = messageDataReceived + " " + "Rectangle already sized";
										rectangleSquare = rectangle2Square;

										std::string outputDataStr = messageDataToSend + " - " + std::to_string(rectangle2Square) + " px";
										std::cout << outputDataStr << std::endl;
									}
								}
								else if (messageDataReceived == "Blue")
								{
									if (rectangle3Square == 0)
									{
										messageDataToSend = messageDataReceived + " " + "Rectangle sized";
										std::thread thr(thread_rect_square, messageDataReceived, rectangleHeight, rectangleWidth);
										threads.emplace_back(std::move(thr));

										for (auto& thread : threads) {
											thread.join();
										}
										rectangleSquare = rectangle3Square;
										rectanglesSquare += rectangle3Square;

										std::string outputDataStr = messageDataToSend + " - " + std::to_string(rectangle3Square) + " px";
										std::cout << outputDataStr << std::endl;
									}
									else
									{
										messageDataToSend = messageDataReceived + " " + "Rectangle already sized";
										rectangleSquare = rectangle3Square;

										std::string outputDataStr = messageDataToSend + " - " + std::to_string(rectangle3Square) + " px";
										std::cout << outputDataStr << std::endl;
									}
								}
								else if (messageDataReceived == "Green")
								{
									if (rectangle4Square == 0)
									{
										messageDataToSend = messageDataReceived + " " + "Rectangle sized";
										std::thread thr(thread_rect_square, messageDataReceived, rectangleHeight, rectangleWidth);
										threads.emplace_back(std::move(thr));

										for (auto& thread : threads) {
											thread.join();
										}
										rectangleSquare = rectangle4Square;
										rectanglesSquare += rectangle4Square;

										std::string outputDataStr = messageDataToSend + " - " + std::to_string(rectangle4Square) + " px";
										std::cout << outputDataStr << std::endl;
									}
									else
									{
										messageDataToSend = messageDataReceived + " " + "Rectangle already sized";
										rectangleSquare = rectangle4Square;

										std::string outputDataStr = messageDataToSend + " - " + std::to_string(rectangle4Square) + " px";
										std::cout << outputDataStr << std::endl;
									}
								}
								else
								{
									messageDataToSend = "Undefined rectangle";
									rectangleSquare = 0;

									std::string outputDataStr = messageDataToSend;
									std::cout << outputDataStr << std::endl;
								}

								messageNumberToSend = ++numberOfMessage;
								messageDataSizeToSend = messageDataToSend.size();
								packetToSend << messageNumberToSend << messageDataSizeToSend << messageDataToSend << rectangleSquare << rectanglesSquare ;
							}

							NetResult resultSent = NetResult::Net_Success;
							while (resultSent == NetResult::Net_Success)
							{
								std::string outputDataStr = "Attempting to send set of data...";
								std::cout << outputDataStr << std::endl;

								resultSent = socketConnected.Send(packetToSend);
								if (resultSent != NetResult::Net_Success)
									break;

								packetToSend.Clear();
								threads.clear();
								outputDataStr = "Data has been sent to client";
								std::cout << outputDataStr << std::endl;
								break;
							}
						}
					}

					/*socketConnected.Close();
					outputDataStr = "NewConnectionSocket successfully closed";
					std::cout << outputDataStr << std::endl;*/
				}
				else
				{
					std::string outputDataStr = "Failed to accept new connection";
					std::cout << outputDataStr << std::endl;
				}
			}
			else
			{
				std::string outputDataStr = "Failed to listen socket on port 80";
				std::cout << outputDataStr << std::endl;
			}

			socketListener.Close();
			outputDataStr = "Socket successfully closed";
			std::cout << outputDataStr << std::endl;
		}
		else
		{
			std::string outputDataStr = "Socket failed to create";
			std::cout << outputDataStr << std::endl;
		}
	}
	else {
		std::string outputDataStr = "WSAStartup failed with error";
		std::cout << outputDataStr << std::endl;
	}

	Network::Shutdown();
	std::string outputDataStr = "WinSock API successfully closed";
	std::cout << outputDataStr << std::endl;

	system("pause");
	return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
