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
		void PageLoaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void SendButtonClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void RandomButtonClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void SizeButtonClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	};
}
