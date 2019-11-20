//
// MainPage.xaml.h
// Declaration of the MainPage class.
//

#pragma once

#include "MainPage.g.h"

namespace PowerSplitClient
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	public ref class MainPage sealed
	{
	public:
		MainPage();

	private:
		void ConnectButtonClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void DisconnectButtonClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void StartTask1Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void StartTask2Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void StartTask3Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void StartTask4Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void PageLoaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	};
}
